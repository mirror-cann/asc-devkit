# MatrixTranspose样例

## 概述

本样例以矩阵转置为例，介绍Ascend C SIMD与SIMT混合编程场景下的访存合并优化思路。样例包含2个kernel版本，从直接索引转置开始，再通过UB中转调整数据写回方式，使GM读写更接近连续访问，从而展示混合编程下矩阵转置的全局访存优化方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.0.0

## 目录结构介绍

```text
├── matrix_transpose
│   ├── CMakeLists.txt         // 编译工程文件
│   ├── matrix_transpose.asc   // 矩阵转置样例实现
│   ├── figures                // README中的图片资源
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
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">transpose_naive_kernel / transpose_coalesced_kernel</td></tr>
  </table>

## 样例实现

### Case实现说明

本样例通过两个独立的kernel实现不同的访存策略，每个kernel对应特定的Case版本。

| Case   | 实现特点                                           | 使用的核函数               | 优化特性              |
| ------ | -------------------------------------------------- | -------------------------- | --------------------- |
| Case 0 | 直接按照转置公式计算输出坐标，GM连续读、非连续写   | transpose_naive_kernel     | 直接索引转置版本      |
| Case 1 | 通过UB暂存tile并交换读写方向，GM读写更接近连续访问 | transpose_coalesced_kernel | UB中转 + 全局访存合并 |

#### 线程块布局

本样例采用SIMD与SIMT混合编程方式，核函数内部通过 `asc_vf_call` 调用 `__simt_vf__` 函数。为方便对比各个版本的性能差异，本样例仅支持方阵，且矩阵宽高需要是32的整数倍，所有Case采用完全一致的线程块布局：

- 本样例将二维tile坐标展平到一维 `blockIdx.x`，并将tile内32×32个元素展平到一维 `threadIdx.x`。
- 每个tile大小为32×32，一个block处理一个tile。block数量为 `bn = grid_x * grid_y`，其中 `grid_x = matrix_width / TILE_DIM`，`grid_y = matrix_height / TILE_DIM`。

基于上述切分方式，`blockIdx.x`表示展平后的一维tile编号。通过对 `grid_width` 整除和取模，可以还原出当前block负责的二维tile坐标 `(block_row, block_col)`。`threadIdx.x`表示tile内展平后的一维元素编号，通过对 `TILE_DIM` 整除和取模，可以还原出线程在tile内处理元素的局部坐标 `(tile_row, tile_col)`。

```cpp
int block_row = blockIdx.x / grid_width;
int block_col = blockIdx.x % grid_width;

int tile_row = threadIdx.x / TILE_DIM;
int tile_col = threadIdx.x % TILE_DIM;
```

下图给出了一个更直观的映射示意。左侧表示1024×1024的矩阵被划分为32×32个tile。右侧表示其中一个block需要处理的一个tile，其大小为32×32。

<img src="./figures/blockMapping.png" width="60%">

以图中绿色元素为例，可以直接用kernel里的内置变量来推导它在GM中的input坐标。

- tile坐标是(2,1)，对应：
  - block_row = 2
  - block_col = 1
  - blockIdx.x = block_row × grid_width + block_col = 2 × 32 + 1 = 65
- 元素在tile内的局部坐标是(2,29)，对应：
  - threadIdx.x = tile_row × TILE_DIM + tile_col = 2 × 32 + 29 = 93

结合代码中的索引计算：

```cpp
int input_row = block_row * TILE_DIM + tile_row;
int input_col = block_col * TILE_DIM + tile_col;
int index_in = input_col + width * input_row;
```

代入本例中的变量值：

- block_row = 65 / 32 = 2
- block_col = 65 % 32 = 1
- tile_row = 93 / 32 = 2
- tile_col = 93 % 32 = 29
- input_row = block_row × TILE_DIM + tile_row = 2 × 32 + 2 = 66
- input_col = block_col × TILE_DIM + tile_col = 1 × 32 + 29 = 61

因此，该线程在GM中处理的输入元素坐标就是：

- input[input_row,input_col] = input[66,61]

如果继续代入一维地址公式，则有：

- index_in = input_col + width × input_row = 61 + 1024 × 66

### 性能指标说明

| 指标                | 说明                                                                                      |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
| aiv_time|Task在AI Vector Core上的理论执行时间，单位为μs。|
| aiv_vec_time(μs) | vec类型指令（向量类运算指令）耗时，单位μs。 |
| aiv_vec_ratio | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位μs。 |
| aiv_scalar_ratio | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令（GM->UB搬运类指令）耗时，单位μs。 |
| aiv_mte2_ratio | mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令（UB->GM搬运类指令）耗时，单位μs。 |
| aiv_mte3_ratio | mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。 |

### Case 0: 直接索引转置版本

**样例目标**：实现基础的矩阵转置功能，作为后续优化版本的耗时对比基线

**核心实现**：

- 每个block处理一个32×32 tile。
- 每个SIMT线程处理tile内1个元素。
- 线程先按原坐标从GM读取输入元素，再计算该元素转置后的输出位置，并把数据直接写到转置后的GM地址。
- GM读取方向连续，GM写回方向不连续。

下图展示了Case 0的数据流，其中标红展示了一个Warp在读取GM和写入GM时处理的元素。对于同一个Warp的线程会读取GM输入中tile的一行元素，写回到GM输出中tile的一列。在读取GM输入时，相邻线程访问的元素地址连续，为连续读，在写回到输出时，相邻线程却被拆散到输出矩阵的不同行上，为不连续写。因此，这一版的核心问题是转置后的写回地址不再连续，这通常会显著影响整体吞吐。

<img src="./figures/case0.png" width="60%">

**关键代码**：

```cpp
int index_in = input_col + width * input_row;
int index_out = input_row + height * input_col;

output[index_out] = input[index_in];
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 63.731 | 62.810 | 58.143 | 0.926 | 2.439 | 0.039 | 0.059 | 0.001 | 0.02 | 0.0 |

**分析**：

- Case 0的Task Duration为63.731μs，作为直接索引转置版本，是后续优化版本的对比基线。
- 该版本GM读取仍然是连续读，但GM写回是跨行、非连续写，同一个Warp的写请求难以高效合并，因此整体耗时主要受限于写回访存模式。

---

### Case 1: UB中转 + 全局访存合并的转置版本

**优化目标**：通过UB中转调整转置写回方式，使GM读写更接近连续访问，降低端到端耗时

**核心实现**：

- 将输入数据的tile从GM搬运到UB上。
- 每个线程从UB中取数，一个Warp会读取UB中tile的一列元素。
- 将取出的值写回GM中的转置位置，一个Warp会将读取UB的一列元素写入到GM输出中tile的一行。

下图展示了Case 1的数据流，其中标红和标黄的元素展示了一个Warp的线程在读取GM和写入GM时处理的元素。读取GM输入时，整个tile会按照GM的排布搬到UB，在写入GM输出时一个Warp的线程会读取UB上的一列元素写回到其对应的转置后的位置。

<img src="./figures/case1.png" width="60%">

与Case 0不同的是，Case 0中线程是“直接把输入元素写到转置后的GM位置”，所以相邻线程会被打散到输出矩阵的不同行上，而Case 1中，核函数先通过 `asc_copy_gm2ub_align` 将输入tile按GM布局搬到UB，再由SIMT线程从UB中按转置方向读取并写回GM。因此，这一版的核心收益是：把原来不连续的GM写回转移为UB侧转置读取，换来GM侧“读连续、写也连续”的访问模式，整体耗时明显低于Case 0。

**关键代码**：

```cpp
uint32_t tile_row = threadIdx.x / TILE_DIM;
uint32_t tile_col = threadIdx.x % TILE_DIM;

output_tile[tile_col + tile_row * height] = input_tile[tile_col * TILE_DIM + tile_row];
```

**优化手段**：

- 使用UB作为tile中转区，把Case 0中的非连续GM写转移为UB侧转置读取。
- 调整输出tile的block坐标，使写回GM时同一个Warp更接近按行连续写入。
- 通过 `asc_sync_notify()` / `asc_sync_wait()` 保证GM到UB的搬运完成后再启动SIMT VF处理。

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 30.602 | 29.6 | 16.942 | 0.572 | 3.404 | 0.115 | 6.912 | 0.234 | 0.022 | 0.001 |

**分析**：

- 相比Case 0的直接索引转置版本，Case 1的Task Duration从63.731μs降低到30.602μs，耗时下降约52.0%。
- 按Task Duration计算，Case 1整体约为Case 0的2.08倍性能，说明通过UB中转改善GM写回连续性后，端到端耗时有明显下降。
- Case 1降低了Case 0中代价较高的非连续GM写回开销，但代码中仍存在GM到UB的tile搬运、流水同步以及SIMT线程对UB的转置方向读取，因此优化后的Task Duration仍不会等同于单纯连续GM读写的理想耗时。

---

## 性能对比总结

### Ascend 950PR性能数据

**综合优化效果**：

- 通过Case 0到Case 1的访存合并优化，样例Task Duration从63.731μs降低到30.602μs，耗时下降约52.0%。
- Case 1相对Case 0性能提升约2.08倍，说明通过UB中转改善GM写回连续性后，端到端耗时有明显收益。

| Case version | Task Duration(μs) | 端到端耗时相对Case 0 | 优化点                           |
| ------------ | ----------------- | -------------------- | -------------------------------- |
| Case 0       | 63.731            | **1x**         | 直接索引转置，GM连续读、非连续写 |
| Case 1       | 30.602            | **2.08x**      | UB中转，全局访存合并             |

## 调优建议

1. **优先关注GM访存连续性**：矩阵转置计算量很小，端到端耗时主要受读写访存模式影响。
2. **使用UB中转改善写回模式**：当直接转置导致GM非连续写时，可以将不连续访问转移到UB侧，换取GM侧连续读写。
3. **注意搬运与流水同步开销**：UB中转需要从GM搬运到UB，并通过流水同步保证tile数据可被SIMT线程读取，优化时需要同时关注GM访存收益、UB转置读和流水同步开销之间的平衡。

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
  SCENARIO_NUM=0                       # 选择执行场景，可选0-1
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  ./demo                               # 执行样例
  ```

  编译选项说明

  | 选项             | 可选值      | 说明              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM` | `0`-`1` | 样例类型，默认为0 |

  执行结果如下，说明精度对比成功。


  ```text
  [Success] Case accuracy is verification passed.
  ```

## 性能分析

使用 `msprof` 工具获取详细性能数据：

```bash
msprof ./demo   # 分析case的性能
```

当前目录下会生成PROF_前缀的文件夹，`mindstudio_profiler_output`目录保存Host和各个Device的性能数据汇总，性能数据分析推荐查看该目录下文件

```bash
PROF_xxxx_XXXXXX
├── device_{id}
└── host
└── mindstudio_profiler_log
└── mindstudio_profiler_output    # 保存Host和各个Device的性能数据汇总
    ├── msprof_*.json
    ├── xx_*.csv
    └── README.txt
```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./PROF_*/mindstudio_profiler_output/op_summary_*.csv
```
