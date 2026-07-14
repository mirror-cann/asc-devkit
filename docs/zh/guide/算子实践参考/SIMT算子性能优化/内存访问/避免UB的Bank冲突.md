# 避免UB的Bank冲突

【优先级】高

【描述】SIMT编程模式下，一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB。对于使用UB中转做转置、重排等操作的算子，即使GM读写已经连续，仍需要分析UB访问模式，避免UB内部访问成为新的性能瓶颈。

<!-- npu="950" id1 -->以Ascend 950PR/Ascend 950DT为例，<!-- end id1 -->下图为UB bank结构示意图。UB总大小为256KB，划分为16个bank，每个bank包含512行，每行32B，共16KB。这16个bank进一步组织为8个bank group，每个bank group包含2个bank，bank i与bank i+8同属bank group i，即 `bank_group_id = bank_id % 8`。在SIMT编程模式下，每个bank又会被划分为4个subbank，每个subbank的宽度为8B。

SIMT场景下，同一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB；当这些访问数据属于同一个bank group的相同编号的subbank资源时，硬件无法在一个周期内处理全部请求，需要排队等待，从而形成subbank冲突并增加访问延迟；如果这些访问数据属于同一个subbank的同一行内的8B地址范围，硬件会将这些请求合并处理，不会形成subbank冲突。

**图 1** bank结构示意图
![img](../../../figures/bank结构示意图.png "bank结构示意图")

UB地址采用低位交织，如下图所示，连续地址按32B粒度映射到bank0到bank15：第1个32B地址段映射到bank0，第2个映射到bank1，依次类推；第16个映射到bank15，第17个映射回bank0的下一行。

**图 2** bank内存排布示意图
![img](../../../figures/bank内存排布示意图.png "bank内存排布示意图")

SIMT编程方式下subbank冲突主要有以下几种：

- **写写冲突**：多个写操作同时尝试访问同一个bank group的相同编号的subbank。
- **读读冲突**：多个读操作同时尝试访问同一个bank group的相同编号的subbank。

【样例介绍】以[矩阵转置](https://gitcode.com/cann/asc-devkit/tree/master/examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice)为例，输入矩阵shape为1024×1024，数据类型为`float`。样例采用32×32的tile，每个ThreadBlock负责处理一个tile，ThreadBlock配置为`(32, 32, 1)`，每个thread处理tile中的1个元素。全局访存合并版本先用UB暂存tile，使GM读写尽量保持连续；在此基础上，避免UB Bank冲突版本通过给UB tile增加padding，调整转置读阶段的UB行跨度，减少同一个Warp内线程集中访问相同subbank资源。

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

上述实现通过UB中转保证了GM读写连续，这里的UB中的`tile`数组表示用于暂存32×32矩阵块的二维UB数组。`tile`数组的数据在UB中的排布为32×32。每行32个`float`，共128B，恰好跨越4个bank。转置写回时，同一个Warp的线程会读取UB中的一列元素，即访问`tile[threadIdx.x][threadIdx.y]`。由于行跨度固定为32个`float`，同一列的32个元素会集中访问两个bank group的subbank 0，属于读读冲突场景。

下图展示了UB中的`tile`数组前10行元素按照行优先存储的排布，其中蓝色标记表示每行的第一个元素。按照地址低位交织规则，tile数组的第1行覆盖bank0到bank3，第2行覆盖bank4到bank7，第3行覆盖bank8到bank11，其余行依次类推。在32×32的UB tile中，同一个Warp的32个线程读取同一列时，会集中访问两个bank group的subbank 0。

<img src="../../../figures/避免Bank冲突反例.png">

在1024×1024矩阵转置样例中，该实现的性能数据如下：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      31.926      |    1.606    |     2650.112     |      1.442      |     0.900     |        0.152        |      0.092      |

【正例】为UB tile增加padding，打散列方向访问的bank分布。

```
constexpr int TILE_DIM = 32;

__global__ void transpose_avoid_bank_conflicts_kernel(float *output, const float *input, int width, int height)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM + 2];

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

上述实现将UB中的`tile`数组从32×32调整为32×34。算法、ThreadBlock切分、线程映射和GM访问方式都不变，只改变UB中每行的物理跨度。增加2列padding后，每行包含34个`float`数据，行跨度变为17个subbank，同一列元素会错开排布到不同的subbank。下图为增加padding后的内存布局；从地址映射看，同一个Warp的32个线程的UB访问会分布到各个bank group的不同subbank中，即同一条访问指令下每个subbank仅有一个线程访问，从而避免上述读读冲突。

<img src="../../../figures/避免Bank冲突正例.png">

在1024×1024矩阵转置样例中，该实现的性能数据如下：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.483      |    1.055    |     1740.038     |      0.883      |     0.847     |        0.151        |      0.138      |

从Task Duration看，增加padding后的执行耗时为23.483μs，相比未加padding的UB中转版本31.926μs，下降约26.4%。这说明在GM读写已经连续的前提下，UB内部的bank冲突仍会影响算子端到端耗时；通过将UB tile从32×32调整为32×34，列方向访问被分散到各个bank group的不同subbank上，避免了上述读读冲突，因此算子的整体运行时间进一步下降。

【总结】当SIMT算子已经通过UB中转解决GM非连续访问后，应继续分析UB访问是否会产生bank冲突。若同一个Warp沿列方向访问二维UB数组，且行跨度容易让访问集中到相同bank资源，可以通过增加padding、调整行跨度或重新排布数据来避免bank冲突。
