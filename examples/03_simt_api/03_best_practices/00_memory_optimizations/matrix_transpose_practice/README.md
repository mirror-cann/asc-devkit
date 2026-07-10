# MatrixTranspose性能调优样例

## 概述

本样例以矩阵转置为例，介绍Ascend C SIMT编程方式下的访存优化思路。样例包含1个矩阵复制基准版本以及3个逐步优化的转置kernel版本，从直接索引转置开始，逐步引入UB中转、全局访存合并以及通过padding降低UB Bank冲突的优化方法，展示SIMT编程方式下矩阵转置的调优路径。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```text
├── matrix_transpose_practice
│   ├── figures                // README中的图片资源
│   ├── CMakeLists.txt         // 编译工程文件
│   ├── matrix_transpose.asc   // SIMT矩阵转置样例实现
│   └── README.md
```

## 样例描述

- 计算公式：

  $$
  output(x, y) = input(y, x)
  $$

  - input为输入矩阵，形状为[H,W]，数据类型为float
  - output为输出矩阵，形状为[W,H]，数据类型为float
- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">copy_kernel / transpose_naive_kernel / transpose_coalesced_kernel / transpose_avoid_bank_conflicts_kernel</td></tr>
  </table>

## 样例实现

### Case实现说明

本样例通过4个独立的kernel实现不同的访存策略，每个kernel对应特定的Case版本。

| Case   | 实现特点                                           | 使用的核函数                          | 优化特性                                |
| ------ | -------------------------------------------------- | ------------------------------------- | --------------------------------------- |
| Case 0 | 按相同坐标读写，不执行转置，GM连续读、连续写       | copy_kernel                           | 矩阵复制版本（基准）                    |
| Case 1 | 直接按照转置公式计算输出坐标                       | transpose_naive_kernel                | 直接索引转置版本                        |
| Case 2 | 通过UB暂存tile并交换读写方向，GM读写更接近连续访问 | transpose_coalesced_kernel            | UB中转 + 全局访存合并实现转置           |
| Case 3 | 在UB tile中增加padding降低bank冲突                 | transpose_avoid_bank_conflicts_kernel | UB中转 + 全局访存合并 + 避免UB Bank冲突 |

#### 线程块布局

为方便对比各个版本的性能差异，本样例仅支持方阵，且矩阵宽高需要是32的整数倍，所有Case采用完全一致的线程块布局：

- 对矩阵转置这类二维规则计算，可以将大矩阵分解为若干局部tile子块，使每个block只处理一个局部区域，便于索引计算和线程协作。
- 每个tile大小为32×32，一个block处理一个tile。grid配置为`(matrix_width/32, matrix_height/32, 1)`。
- block配置固定为`(32, 32, 1)`，单个block总线程数为32×32=1024，每个线程只处理tile内1个元素。
- `threadIdx.x`方向有32个线程，对应tile内的列索引；`threadIdx.y`方向有32个线程，对应tile内的行索引。

基于上述切分方式，`blockIdx`用于定位当前block负责的tile，`threadIdx`用于定位当前线程在tile内处理的元素。将tile坐标和tile内局部坐标相加，可以得到该元素在原矩阵中的全局坐标`x_index`和`y_index`；再按照行优先布局展开，即可得到GM中的线性索引`index`。

```cpp
int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
int index = x_index + width * y_index;
```

下图给出了一个更直观的映射示意。左侧表示1024×1024的矩阵被划分为32×32个tile。右侧表示其中一个block需要处理的一个tile，其大小为32×32。

<img src="./figures/blockMapping.png" width="60%">

以图中绿色元素为例，可以直接用kernel里的内置变量来推导它在GM中的input坐标。

- 图中绿色元素所在的tile坐标是(2,1)，对应：
  - blockIdx.y = 2
  - blockIdx.x = 1
- 图中绿色元素在tile内的局部坐标是(2,29)，对应：
  - threadIdx.y = 2
  - threadIdx.x = 29

代入本例中的变量值：

- x_index = blockIdx.x × TILE_DIM + threadIdx.x = 1 × 32 + 29 = 61
- y_index = blockIdx.y × TILE_DIM + threadIdx.y = 2 × 32 + 2 = 66

因此，该线程在GM中处理的输入元素坐标就是：

- input[y_index,x_index] = input[66,61]

如果继续代入一维地址公式，则有：

- index = x_index + width × y_index = 61 + 1024 × 66

### 性能指标说明

| 指标                | 说明                                                                                      |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)   | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间                  |
| aiv_time(μs)        | Task在AI Vector Core上的理论执行时间，单位为μs                                            |
| aiv_total_cycles    | 该Task被分配到每个AI Vector Core计算单元上后，每个AI Vector Core计算单元上的执行cycle总数 |
| aiv_vec_time(μs)    | vec类型指令（向量类运算指令）耗时，单位为μs                                               |
| aiv_vec_ratio       | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比                           |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位为μs                                            |
| aiv_scalar_ratio    | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比                        |

除Task Duration外，本例中其余指标展示的是所有block上性能指标的平均值。

### Case 0: 矩阵复制版本

**样例目标**：建立矩阵复制场景的耗时基线，为后续转置版本提供性能参照

**核心实现**：

- 每个block处理一个32×32 tile
- 每个线程处理tile内1个元素
- block根据`blockIdx`定位到当前tile，线程根据`threadIdx`定位到tile内的一个元素
- 线程从GM读取`input[index]`，并直接写回`output[index]`

整个过程中没有坐标交换，也没有UB参与，该版本不涉及矩阵转置，同一个Warp中的相邻线程GM读写方向一致，内存访问模式连续，可作为后续转置版本的耗时基线。

**关键代码**：

```cpp
int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
int index = x_index + width * y_index;

output[index] = input[index];
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      21.751      |    0.919    |     1515.731     |      0.743      |     0.817     |        0.164        |      0.169      |

**分析**：

- Case 0的Task Duration为21.751μs，作为连续GM读写场景的耗时基线
- 后续转置版本需要在完成坐标交换的同时尽量接近该基线

---

### Case 1: 直接索引转置版本

**优化目标**：实现最直接的矩阵转置功能，并观察直接写回转置地址带来的耗时变化

**核心实现**：

- 每个block处理一个32×32 tile
- 每个线程处理tile内1个元素
- 线程先按原坐标从GM读取输入元素，再计算该元素转置后的输出位置，并把数据直接写到转置后的GM地址
- GM读取方向连续，GM写回方向不连续

下图展示了Case 1的数据流，其中标红元素表示一个Warp在读取GM和写入GM时处理的元素。同一个Warp的线程读取GM输入中tile的一行元素，写回到GM输出中tile的一列。在读取GM输入时，相邻线程访问的元素地址连续，为连续读；在写回到输出时，相邻线程访问的地址分散到输出矩阵的不同行上，为不连续写。因此，这一版的核心问题是转置后的写回地址不再连续，整体吞吐受到影响。

<img src="./figures/case1.png" width="60%">

**关键代码**：

```cpp
int index_in = x_index + width * y_index;
int index_out = y_index + height * x_index;

output[index_out] = input[index_in];
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      61.508      |    3.609    |     5954.297     |      3.441      |     0.954     |        0.156        |      0.043      |

**分析**：

- 与Case 0的copy基线相比，Task Duration从21.751μs增加到61.508μs，约为copy版本的2.83倍
- 直接索引转置本身计算量很小，但转置后的GM写回变为跨行、非连续访问，因此端到端耗时明显高于copy基线
- 该版本GM读取仍然是连续读，但GM写回地址不连续，同一个Warp的写请求难以高效合并，这是Task Duration上升的主要原因

---

### Case 2: UB中转 + 全局访存合并的转置版本

**优化目标**：通过UB中转调整转置写回方式，使GM读写更接近连续访问，降低端到端耗时

**核心实现**：

- 每个线程从GM读取tile中的1个元素，一个Warp会读取一个tile的一行元素
- 按原坐标把元素写入UB中的tile，一个Warp会将读取的一行写入到UB中的tile的一行
- 同步后，每个线程从UB中取数，一个Warp会读取UB中tile的一列元素
- 将取出的值写回GM中的转置位置，一个Warp会将读取UB的一列元素写入到GM输出中tile的一行

下图展示了Case 2的数据流，其中标红和标黄的元素展示了一个Warp的线程在读取GM和写入GM时处理的元素。读取GM输入时，整个tile会按照GM的排布搬到UB，在写入GM输出时一个Warp的线程会读取UB上的一列元素写回到其对应的转置后的位置。

<img src="./figures/case2.png" width="60%">

与Case 1不同的是，Case 1中线程是“直接把输入元素写到转置后的GM位置”，所以相邻线程访问的输出地址分散到矩阵的不同行上；Case 2中，线程先把数据放到UB里，把原来不连续的全局写访问转移到UB内的不连续读。因此，这一版的核心收益是：虽然增加了一次UB读写和一次同步，但换来了GM侧“读连续、写也连续”的访问模式，本组数据中整体耗时明显低于Case 1。

**关键代码**：

```cpp
tile[threadIdx.y][threadIdx.x] = input[index_in];
asc_syncthreads();

x_index = blockIdx.y * TILE_DIM + threadIdx.x;
y_index = blockIdx.x * TILE_DIM + threadIdx.y;
int index_out = x_index + y_index * height;

output[index_out] = tile[threadIdx.x][threadIdx.y];
```

**优化手段**：

- 使用UB作为tile中转区，把Case 1中的非连续GM写转移为UB侧访问
- 交换输出tile的block坐标，使写回GM时同一个Warp更接近按行连续写入
- 通过`asc_syncthreads()`保证整个tile写入UB后再执行转置方向读取

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      31.926      |    1.606    |     2650.112     |      1.442      |     0.900     |        0.152        |      0.092      |

**分析**：

- 相比Case 1的naive transpose，Task Duration从61.508μs降低到31.926μs，耗时下降约48.1%，整体性能提升约1.93倍
- Case 2通过UB中转把Case 1中的非连续GM写转移为UB侧访问，使GM读写都更接近连续访问，因此Task Duration明显降低
- 与Case 0的copy基线相比，Case 2的Task Duration仍高约46.8%。Case 2相比copy版本额外引入了UB读写、同步以及转置方向的UB访问开销

---

### Case 3: UB中转 + 全局访存合并 + 避免UB Bank冲突的转置版本

**优化目标**：在全局访存合并版本的基础上，通过UB padding避免转置读阶段的bank冲突

**核心实现**：

- GM到UB阶段与Case 2完全一致
- 差异只在UB布局，从32×32改成32×34
- 同步后，从UB写回到GM与Case 2完全一致
- 该版本没有改变算法路径，也没有改变block和线程切分，只是调整了UB中的物理布局

下面以Ascend 950PR/Ascend 950DT的UB划分规则为例，说明本样例中bank冲突是如何产生的，以及Case 2和Case 3理论上的冲突强度差异。

UB中的bank划分如下图所示。UB总大小256KB，划分为16个bank（每个bank512行 × 32B = 16KB）；这16个bank进一步组织为8个bank group，每个bank group包含2个bank（bank i与bank i+8同属bank group i，即 `bank_group_id = bank_id % 8`）。在SIMT编程模式下，每个bank又会被划分为4个subbank，每个subbank的宽度为8B。

SIMT场景下，同一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB；当这些访问数据属于同一个bank group的相同编号的subbank资源时，硬件无法在一个周期内处理全部请求，需要排队等待，从而形成subbank冲突并增加访问延迟；如果这些访问数据属于同一个subbank的同一行内的8B地址范围，硬件会将这些请求合并处理，不会形成subbank冲突。

<img src="./figures/bank结构示意图.png">

**地址采用低位交织：** 如下图所示，UB内连续地址按32B粒度映射到bank0~bank15 —— 第1个32B地址段映射到bank0、第2个映射到bank1、……、第16个映射到bank15，第17个映射回bank0的下一行，依次类推。

<img src="./figures/bank内存排布示意图.png">

SIMT编程方式下subbank冲突主要有以下几种：

- **写写冲突**：多个写操作同时尝试访问同一个bank group的相同编号的subbank。
- **读读冲突**：多个读操作同时尝试访问同一个bank group的相同编号的subbank。

Case 2中UB的tile数组前10行元素按照行优先存储的排布图如下，按照地址低位交织的规则，tile数组的第一行row 1刚好覆盖bank 0～bank 3，row 2覆盖bank 4～bank 7，row 3则覆盖bank 8～bank 11，为了展示方便这里只展示前10行元素排布，其余行依次类推。每行32个float数据会恰好跨越4个bank存储，其中每行第一个元素用了蓝色标记。在Case 2中，一个Warp的线程会读取tile的一列元素写回到GM输出；访问UB时，32个线程会集中访问两个bank group的subbank 0，属于读读冲突场景。

<img src="./figures/case2bank.png">

Case 3中对UB中的tile数组增加两列padding，由每行32个元素改为34个元素，其在UB上的排列如下图所示。由于每行有34个元素，行跨度变为17个subbank，同一列元素会错开排布到不同的subbank。Case 3访问UB时，32个线程的访问会分布到各个bank group的每个subbank中，即同一条访问指令下每个subbank仅有一个线程访问，从而避免上述读读冲突。

<img src="./figures/case3bank.png">

**关键代码**：

```cpp
__ubuf__ float tile[TILE_DIM][TILE_DIM + 2];

tile[threadIdx.y][threadIdx.x] = input[index_in];
asc_syncthreads();

x_index = blockIdx.y * TILE_DIM + threadIdx.x;
y_index = blockIdx.x * TILE_DIM + threadIdx.y;
int index_out = x_index + y_index * height;

output[index_out] = tile[threadIdx.x][threadIdx.y];
```

**优化手段**：

- 对UB tile增加+2 padding，使每行stride从32个float变为34个float
- 通过改变UB中同一列元素的bank和subbank分布，避免同一Warp转置读阶段的读读冲突

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.483      |    1.055    |     1740.038     |      0.883      |     0.847     |        0.151        |      0.138      |

**分析**：

- 相比Case 2的transpose_coalesced_kernel，Task Duration从31.926μs降低到23.483μs，耗时下降约26.4%，整体性能提升约1.36倍
- Case 3在Case 2的基础上通过padding避免UB转置读阶段的bank冲突，因此端到端Task Duration继续下降
- 相比Case 1的naive transpose，Case 3的Task Duration下降约61.8%，整体性能提升约2.62倍，说明“GM访存合并 + UB bank冲突避免”两步优化在端到端耗时上叠加生效
- 与Case 0的copy基线相比，Case 3的Task Duration只高约8.0%，已经接近连续GM读写的基准水平。Case 3相比copy版本仍需执行UB中转、同步和UB转置读，仅凭汇总指标不再进一步拆分归因

---

## 性能对比总结

### Ascend 950PR性能数据

**综合优化效果**：

- 从Case 1直接索引转置到Case 3完整优化版本，Task Duration从61.508μs降低到23.483μs，耗时下降约61.8%，整体性能提升约2.62倍
- Case 3相比Case 0 copy基线仅高约8.0%，说明通过GM访存合并和避免UB bank冲突后，矩阵转置已经接近连续GM读写基线

| Case version | Task Duration(μs) | Task Duration相对Case 0 | 优化点                                |
| ------------ | ----------------- | ----------------------- | ------------------------------------- |
| Case 0       | 21.751            | **1x**            | 矩阵复制基线，GM连续读、连续写        |
| Case 1       | 61.508            | **2.83x耗时**     | 直接索引转置，GM连续读、非连续写      |
| Case 2       | 31.926            | **1.47x耗时**     | UB中转，全局访存合并                  |
| Case 3       | 23.483            | **1.08x耗时**     | UB中转，全局访存合并，避免UB Bank冲突 |

## 调优建议

1. **先建立copy基线**：在分析访存类算子性能时，建议先测量copy场景耗时，再用实际算子的性能与其做对比。
2. **优先关注GM访存连续性**：矩阵转置计算量很小，端到端耗时主要受读写访存模式影响。
3. **使用UB中转改善写回模式**：当直接转置导致GM非连续写时，可以将不连续访问转移到UB侧，换取GM侧连续读写。
4. **继续分析UB Bank冲突**：GM访存合并后，UB转置读阶段的bank冲突可能成为下一层瓶颈，可以通过padding等方式调整UB物理布局，避免同一Warp的并发线程集中访问相同bank资源。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=3                       # 选择执行场景，可选0-3
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  ./matrix_transpose                   # 执行样例
  ```
- 编译选项说明

  | 选项                        | 可选值       | 说明                                                         |
  | --------------------------- | ------------ | ------------------------------------------------------------ |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM`            | `0`-`3`  | 样例类型，默认为3                                            |

  执行结果如下，说明精度对比成功。


  ```text
  [Success] Case accuracy is verification passed.
  ```

## 性能分析

使用 `msprof` 工具获取详细性能数据：

```bash
msprof op ./matrix_transpose   # 分析case的性能
```

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```
