# 避免UB的Bank冲突

【优先级】高

【描述】SIMT编程模式下，一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB。若这些线程集中访问少数几个bank或bank group，会产生bank冲突，导致访问请求排队，影响整体吞吐。对于使用UB中转做转置、重排等操作的算子，即使GM读写已经连续，仍需要分析UB访问模式，避免UB内部访问成为新的性能瓶颈。

以Ascend 950PR/Ascend 950DT为例，下图为UB bank结构示意图，图中箭头方向表示内存排布的顺序。UB总大小为256KB，可以划分为16个bank，并组织为8个bank group。bank0和bank8属于同一个bank group，bank1和bank9属于同一个bank group，依次类推。SIMT场景下需要重点关注同一个Warp内的并发线程是否集中访问UB的少数bank。

**图1**  bank结构示意图

![bank结构示意图（图中箭头方向表示内存排布的顺序）-71](../../../figures/bank结构示意图（图中箭头方向表示内存排布的顺序）-71.png)

每个bank可以独立地接受数据的读写操作，允许多个数据访问请求同时进行。然而，当多个读写操作试图同时访问同一个bank时，由于硬件资源的限制，这些操作必须排队等待，这会导致bank冲突以及算子性能下降。

- **读写冲突**：读操作和写操作同时尝试访问同一个bank。
- **写写冲突**：多个写操作同时尝试访问同一个bank group。
- **读读冲突**：两个读操作同时尝试访问同一个bank，或者两个以上读操作同时尝试访问同一个bank group。

【反例】矩阵转置使用UB做中转，转置读阶段产生较严重bank冲突。

```
constexpr int TILE_DIM = 32;

__global__ void transpose_coalesced_kernel(float *output, const float *input, int width, int height)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM];

    int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
    int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
    int index_in = x_index + y_index * width;

    tile[threadIdx.y][threadIdx.x] = input[index_in];
    asc_syncthreads();

    x_index = blockIdx.y * TILE_DIM + threadIdx.x;
    y_index = blockIdx.x * TILE_DIM + threadIdx.y;
    int index_out = x_index + y_index * height;

    output[index_out] = tile[threadIdx.x][threadIdx.y];
}
```

上述实现通过UB中转保证了GM读写连续，这里的UB中的`tile`数组表示用于暂存32 x 32矩阵块的二维UB数组。 `tile`数组的数据在UB中的排布为32 x 32。每行32个`float`，共128B，恰好跨越4个bank。转置写回时，同一个Warp的线程会读取UB中的一列元素，即访问`tile[threadIdx.x][threadIdx.y]`。由于行跨度固定为32个`float`，同一列的32个元素会集中落到少数bank上，容易出现读读冲突。

下图展示了UB中的`tile`数组前10行元素按照行优先存储的排布，其中蓝色标记表示每行的第一个元素。在32 x 32的UB tile中，同一个Warp中32个线程读取同一列时可能集中访问两个bank，即每个bank约有16个线程同时访问，冲突规模较大。

<img src="../../../figures/避免Bank冲突反例.png">

在1024 x 1024矩阵转置样例中，该实现的性能数据如下。当前msprof op暂无法单独给出bank冲突耗时，因此本文仅基于Task Duration对执行耗时进行对比分析：

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      35.945      |    1.814    |     2993.315     |      1.646      |     0.910     |        0.156        |      0.083      |

【正例】为UB tile增加padding，打散列方向访问的bank分布。

```
constexpr int TILE_DIM = 32;

__global__ void transpose_avoid_bank_conflicts_kernel(float *output, const float *input, int width, int height)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM + 1];

    int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
    int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
    int index_in = x_index + y_index * width;

    tile[threadIdx.y][threadIdx.x] = input[index_in];
    asc_syncthreads();

    x_index = blockIdx.y * TILE_DIM + threadIdx.x;
    y_index = blockIdx.x * TILE_DIM + threadIdx.y;
    int index_out = x_index + y_index * height;

    output[index_out] = tile[threadIdx.x][threadIdx.y];
}
```

上述实现将UB中的`tile`数组从32 x 32调整为32 x 33。算法路径、ThreadBlock切分、线程映射和GM访问方式都不变，只改变UB中每行的物理跨度。增加1列padding后，每行包含33个`float`数据，列方向会访问逐行错开的UB地址，使同一个Warp读取一列时分布到更多bank上。下图为增加padding后的内存布局；从地址映射看，32 x 33布局可以将同一Warp的32个线程分散到8个bank中，即每个bank约有4个线程同时访问，相比32 x 32布局下每个bank约16个线程访问，冲突规模明显降低。

<img src="../../../figures/避免Bank冲突正例.png">

在1024 x 1024矩阵转置样例中，该实现的性能数据如下：

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      26.725      |    1.224    |     2018.943     |      1.059      |     0.869     |        0.152        |      0.121      |

从Task Duration看，增加padding后的执行耗时为26.725us，相比未加padding的UB中转版本35.945us，下降约25.7%。这说明在GM读写已经连续的前提下，UB内部的bank冲突仍会影响算子端到端耗时；通过将UB tile从32 x 32调整为32 x 33，列方向访问被分散到更多bank上，访问UB时的排队等待情况减少，因此算子的整体运行时间进一步下降。

【总结】当SIMT算子已经通过UB中转解决GM非连续访问后，应继续分析UB访问是否会产生bank冲突。若同一个Warp沿列方向访问二维UB数组，且行跨度容易让访问集中到少数bank，可以通过增加padding、调整行跨度或重新排布数据来降低bank冲突。
