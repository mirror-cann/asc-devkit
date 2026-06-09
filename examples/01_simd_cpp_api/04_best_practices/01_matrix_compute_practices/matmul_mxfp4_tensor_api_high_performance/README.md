# MxFP4 Matmul Tensor API高性能样例

## 概述

本样例介绍如何基于Ascend C Tensor API和静态Tensor编程方式，通过L1 Buffer/L0 Buffer双缓冲机制、大包搬运、细粒度流水同步等多种优化手段，实现一个高性能的MxFP4 Matmul kernel。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构

```text
├── matmul_mxfp4_tensor_api_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   └── mmad_mx.asc             // Ascend C Tensor API 样例实现
```

## MxMatmul介绍

普通Matmul只有两路矩阵输入：

$$
C = A * B
$$

MxMatmul在此基础上额外引入两路ScaleA/ScaleB输入。A、B为MxFP4低比特数据，ScaleA、ScaleB为缩放因子。计算时，矩阵数据与对应ScaleA/ScaleB共同参与运算：

$$
C = (\text{ScaleA} \otimes A) \times (\text{ScaleB} \otimes B)
$$

在MxMatmul中，K方向每32个元素共享一个scale。也就是说，scale的数量远少于矩阵元素数量，但它会沿K方向广播到对应的一组MxFP4数据上，如图所示：

**图 1**  MxMatmul计算示意图

![](figure/MxMatmul.png)

## 样例规格

本样例实现固定shape为`8192 x 8192 x 8192`的MxFP4 Matmul，输出数据类型为`bfloat16_t`。

### 输入输出

| 输入/输出 | 逻辑形状 | 数据类型 | 数据排布类型 | 说明 |
|------|------|----------|------|------|
| A | `[M, K]` | `fp4x2_e1m2_t` | ND | 左矩阵，每个字节打包2个`fp4x2_e1m2_t`元素 |
| ScaleA | `[M, scaleK]` | `fp8_e8m0_t` | ND | A矩阵的缩放因子，A矩阵在K方向每32个元素共享一个scale |
| B | `[N, K]` | `fp4x2_e1m2_t` | ND | 右矩阵，按 `[N, K]` 形式输入 kernel |
| ScaleB | `[N, scaleK]` | `fp8_e8m0_t` | ND | B矩阵的缩放因子，B矩阵在K方向每32个元素共享一个scale |
| C | `[M, N]` | `bfloat16_t` | ND | 输出矩阵 |

其中：

- 两个`fp4x2_e1m2_t`打包存储在一个字节中，因此当数据类型为`fp4x2_e1m2_t`时，K需要为偶数
- `scaleK=align_even(ceil(K/32))`，表示先向上整除再对齐到2的倍数。由于硬件约束要求scale数据在K方向满足2Byte连续对齐，因此scaleK必须为偶数
- 由于硬件约束，当ScaleB矩阵为`[scaleK, N]`输入时，需要K方向2Byte连续，因此推荐ScaleB采用`[N, scaleK]`输入

#### 四路输入说明

与普通Matmul相比，MxMatmul在kernel中多了scale的搬运、加载和参与计算路径。A/B与ScaleA/ScaleB必须按相同的K block节奏进入计算流水，否则Cube侧拿不到匹配的缩放信息。

- 四路输入的数据排布格式如下图所示：

  **图 2**  四路输入数据排布格式

  ![](figure/formatOfMx.png)

- 四路输入的搬运与计算如下图所示：

  **图 3**  四路输入搬运与计算

  ![](figure/InputOfMxMatmul.png)

### 关键参数

| 参数 | 值 | 说明 |
|------|----|------|
| `M` | `8192` | 矩阵M维度规模 |
| `N` | `8192` | 矩阵N维度规模 |
| `K` | `8192` | 矩阵K维度规模 |
| `baseM` | `256` | Cube计算基本块M维度大小 |
| `baseK` | `256` | Cube计算基本块K维度大小 |
| `baseN` | `256` | Cube计算基本块N维度大小 |
| `singleCoreM` | `2048` | 单核M方向计算范围 |
| `singleCoreN` | `1024` | 单核N方向计算范围 |
| `singleCoreK` | `8192` | 单核K方向计算范围 |
| `stepK` | `2` | Global Memory->L1 Buffer搬运中，A/B在K方向的大包搬运步长 |
| `scaleFactorK` | `4` | Global Memory->L1 Buffer搬运中，ScaleA/ScaleB相对A/B的K方向搬运比例 |
| `Block Num` | `32` | 使用核数 |

> **约束说明**：本样例暂时不支持K方向存在尾块的场景，即要求`K`能被`baseK`整除。

## 样例实现

整个kernel的过程为：多核切分后，每个核负责一个`singleCoreM * singleCoreN`子矩阵，在M/N方向上循环，将A/B与ScaleA/ScaleB搬入L1 Buffer，再在K方向循环，加载到L0A Buffer/L0B Buffer和L0ScaleA Buffer/L0ScaleB Buffer，完成Cube累加计算，最后把结果写回Global Memory。

### 数据流路径

整体数据流如下：

```text
物理地址                               流水
  Global Memory
  |  A:CopyGM2L1 / ScaleA:CopyGM2L1   MTE2
  |  B:CopyGM2L1 / ScaleB:CopyGM2L1   MTE2
  v
  L1 Buffer
  |  A:CopyL10L0A / ScaleA:CopyL10L0ScaleA        MTE1
  |  B:CopyL10L0B / ScaleB:CopyL10L0ScaleB        MTE1
  v
  L0A Buffer / L0B Buffer / L0ScaleA Buffer / L0ScaleB Buffer
  |
  |  Mmad                              M
  v
  L0C Buffer
  |
  |  CopyL0C2GM: float -> bfloat16_t     FIX
  v
  Global Memory
```

#### 多核切分：把大矩阵拆成32个子任务

样例使用32核并行。`M=8192`按`singleCoreM=2048`切成4份，`N=8192`按`singleCoreN=1024`切成8份，正好形成`4 x 8 = 32`个子矩阵。

```text
M direction: 8192 / 2048 = 4 blocks
N direction: 8192 / 1024 = 8 blocks

Total blocks = 4 * 8 = 32
```

每个核只处理自己负责的`2048 x 1024`输出区域。核内按`baseK=256`分块在K方向循环完成累加。

对应的核索引计算如下：

```cpp
constexpr uint32_t mIter = AscendC::DivCeil(M, singleCoreM);
uint32_t mIterIdx = AscendC::GetBlockIdx() % mIter;
uint32_t nIterIdx = AscendC::GetBlockIdx() / mIter;
```

#### 2. GM 到 L1：矩阵和 scale 一起进流水

MxMatmul 的输入不是两路，而是四路：A、B、ScaleA、ScaleB。因此每个核 GM 到 L1 的搬运也分成四类：

| 数据 | 搬运函数 | Global Memory数据shape | L1 Buffer数据shape |
|------|----------|-----------|---------|
| A | `Copy` | `[singleCoreM, singleCoreK]` | `[baseM, stepK * baseK]` |
| B | `Copy` | `[singleCoreN, singleCoreK]` | `[baseN, stepK * baseK]` |
| ScaleA | `Copy` | `[singleCoreM, singleCoreSK]` | `[baseM, scaleFactorK * stepK * baseK]` |
| ScaleB | `Copy` | `[singleCoreN, singleCoreSK]` | `[baseN, scaleFactorK * stepK * baseK]` |

通过`stepK=2`一次搬入`stepK * baseM * baseK`的A矩阵数据和`stepK * baseN * baseK`的B矩阵数据，减少搬运指令数量，提高Global Memory到L1 Buffer的搬运效率。

scale的搬运粒度由`scaleFactorK=4`控制，一次搬入`scaleFactorK * stepK * baseM * baseSK`的ScaleA矩阵数据和`scaleFactorK * stepK * baseN * baseSK`的ScaleB矩阵数据，其中`baseSK=baseK/32`。它的作用是让ScaleA/ScaleB在K方向相比于A/B一次覆盖更大的范围，减少scale反复搬运带来的MTE2压力。

以A矩阵数据搬运为例，`Copy`一次搬入`stepK`个K方向base块，每个base块为`baseM * baseK`：

```cpp
AscendC::Te::Copy(gm2L1Atom, l1TensorAPing, a.Slice(AscendC::Te::MakeCoord(mBlockIdx * baseM, dataNextKChunkIdx * baseK), AscendC::Te::MakeShape(curM, stepCurK)));
```

以ScaleA矩阵数据搬运为例。`Copy`一次搬入`scaleFactorK * stepK`个K方向base块，每个base块为`baseM * baseK`：

```cpp
AscendC::Te::Copy(gm2L1Atom, l1TensorAsPing, as.Slice(AscendC::Te::MakeCoord(mBlockIdx * baseM, scaleNextKChunkIdx * baseScaleK), AscendC::Te::MakeShape(curM, stepCurScaleK)));
```

#### 3. L1 到 L0 进入 MX 计算路径

普通Matmul只需要加载A/B；MxMatmul还需要加载对应的ScaleA/ScaleB，每次搬运一个base块。

当A/B和ScaleA/ScaleB进入L0 Buffer后，Cube计算单元就可以按照MX语义完成带缩放的矩阵乘累加。

以A矩阵数据搬运为例，`Copy`一次搬入1个`baseM * baseK`大小的base块：

```cpp
AscendC::Te::Copy(l12l0AAtom, l0TensorA, l1ReadBufA.Slice(AscendC::Te::MakeCoord(0, kOffsetInDataChunk * baseK), AscendC::Te::MakeShape(curM, curK)));
```

以ScaleA矩阵数据搬运为例，`Copy`一次搬入1个`baseM * baseSK`大小的base块：

```cpp
AscendC::Te::Copy(l12l0ScaleAAtom, l0TensorAs, l1ReadBufAs.Slice(AscendC::Te::MakeCoord(0, kOffsetInScaleChunk * baseScaleK), AscendC::Te::MakeShape(curM, curScaleK)));
```

#### 4. L0 双缓冲：让加载和计算交叠

样例在L1 Buffer和L0 Buffer都使用Ping-Pong双缓冲。这样做的核心目标是让搬运、加载、计算尽量交叠，减少等待。

本样例的主要缓冲区布局如下：

| 层级 | 缓冲区 | 内容 | 作用 |
|------|--------|------|------|
| L1 Buffer | `A Ping/Pong` | A的大包数据 | 存放Global Memory->L1 Buffer的A数据 |
| L1 Buffer | `B Ping/Pong` | B的大包数据 | 存放Global Memory->L1 Buffer的B数据 |
| L1 Buffer | `ScaleA Ping/Pong` | A对应的scale数据 | 存放A侧MXScale |
| L1 Buffer | `ScaleB Ping/Pong` | B对应的scale数据 | 存放B侧MXScale |
| L0A Buffer | `A Ping/Pong` | 当前K block的A数据 | Cube `Mmad`左操作数 |
| L0B Buffer | `B Ping/Pong` | 当前K block的B数据 | Cube `Mmad`右操作数 |
| L0C Buffer | `C` | float累加结果 | `Mmad`输出，供写回Global Memory |

一个简化的流水节奏如下：

```text
time     |---------------------------------------------------------------------------->

GM->L1   | A/B/ScaleA/ScaleB Ping | A/B/ScaleA/ScaleB Pong | A/B/ScaleA/ScaleB Ping |
L1->L0                            | L0 Ping load --|       | L0 Pong load --|
Cube                                               | Mmad Ping ---|   | Mmad Pong ---|
L0C->GM                                                           | Copy L0C to GM ---
```

图中横线长度只用于表达不同阶段耗时可能不同，并不代表实测比例；真实耗时需要以 `msprof` 采集结果为准。

在K循环中，当前K block进入Cube计算时，下一批A/B/ScaleA/ScaleB可以提前发起搬运。Ping和Pong交替使用，生产者和消费者通过事件同步确认缓冲区是否可写、数据是否可读。

#### 5. Cube 计算：按 K block 累加

每个输出子矩阵会在最内层先沿K方向循环。每一轮处理一个`baseK=256`的K block，再沿M/N方向循环：

```text
for nBlock in N blocks:
  for mBlock in M blocks:
    for kBlock in K blocks:
        Copy(A block, ScaleA block)
        Copy(B block, ScaleB block)
        Mmad accumulate
```

`Mmad`的输入来自L0A Buffer/L0B Buffer，输出累加到L0C Buffer。第一轮K block初始化累加结果，后续K block持续累加，直到完整K方向计算完成。

计算阶段的关键参数包括`m/n/k`尺寸、是否初始化C矩阵：

```cpp
AscendC::Te::MmadParams mmadParams{curM, curN, curK, 0, true};
mmadParams.cmatrixInitVal = (kBlockIdx == 0);
AscendC::Te::Mmad(mmadAtom.with(mmadParams), l0TensorC, l0TensorA, l0TensorB);
```

其中`cmatrixInitVal`用于控制第一轮K block初始化累加结果，后续K block在已有L0C Buffer数据上继续累加。

#### 6. L0C 写回 GM：从 float 转换到 bfloat16_t 输出

Cube侧累加结果保存在L0C Buffer中，数据类型为float。计算结束后，`Copy`负责把结果转换为`bfloat16_t`并写回Global Memory中。

```text
L0C float result
      |
      | Copy, F32 -> BF16
      v
GM C [M, N]
```

```cpp
AscendC::Te::FixpipeParams fixpipeParams;
AscendC::Te::Copy(l0c2GmAtom.with(fixpipeParams), c.Slice(AscendC::Te::MakeCoord(mBlockIdx * baseM, nBlockIdx * baseN), AscendC::Te::MakeShape(curM, curN)), l0TensorC);
```

### 事件同步：把流水并行起来

使用静态Tensor编程时，同步是性能和正确性的关键。本样例主要使用四类事件，eventID 控制在
`EVENT_ID0` 到 `EVENT_ID3`：

| 事件 | 方向 | 作用 | flag 编号 |
|------|------|------|----------|
| `MTE2_MTE1` | GM->L1 通知 L1->L0 | GM->L1 Copy 完成后，通知 L1->L0 Copy 可以读取 L1 数据 | `EVENT_ID0/1`: A+B Data Ping/Pong；`EVENT_ID2/3`: As+Bs Scale Ping/Pong |
| `MTE1_MTE2` | L1->L0 通知 GM->L1 | L1->L0 Copy 消费完 L1 数据后，通知 GM->L1 Copy 可以覆盖该缓冲区 | 同上 |
| `MTE1_M` | L1->L0 通知 Cube | L1->L0 Copy 完成后，通知 Mmad 可以开始计算 | `EVENT_ID0/1`: L0 Ping/Pong |
| `M_MTE1` | Cube 通知 L1->L0 | Mmad 消费完 L0 缓冲区后，通知下一轮 L1->L0 Copy 可以写入 | `EVENT_ID0/1`: L0 Ping/Pong |

A/B data 的生命周期一致，因此按 Ping/Pong 相位绑定到同一组事件；ScaleA/ScaleB 的生命周期一致，但
scale chunk 通常比 data chunk 更大，因此单独使用另一组 Ping/Pong 事件。

反向同步需要在进入主循环前预置，否则首次等待可写缓冲区时会阻塞：

```cpp
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID0);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID1);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID2);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID3);
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID1);
```

大包粒度的同步只在大包首元素等待数据就绪，在大包末元素释放缓冲区：

```cpp
if (kOffsetInDataChunk == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(GetDataEventId(dataReadIdx));
}
if (((kOffsetInDataChunk + 1) == dataChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(GetDataEventId(dataReadIdx));
}
if (kOffsetInScaleChunk == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(GetScaleEventId(scaleReadIdx));
}
if (((kOffsetInScaleChunk + 1) == scaleChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(GetScaleEventId(scaleReadIdx));
}
```

从流水角度看，这些事件串起了三个阶段：

```text
Copy(GM->L1)
        |
        | MTE2_MTE1
        v
Copy(L1->L0)
        |
        | MTE1_M
        v
Mmad(Cube)
        |
        | M_MTE1
        v
next LoadData
```

反向事件也很重要。比如`MTE1_MTE2`表示L1 Buffer中某个Ping/Pong缓冲已经被消费完，可以被下一次Global Memory->L1 Buffer搬运覆盖。没有这些反向同步，流水很容易出现覆盖未消费数据或等待不完整数据的问题。

## 性能优化总结

本样例的性能优化围绕三件事展开：

- **多核切分**：把`8192 x 8192`输出矩阵拆到32核并行计算。
- **大包搬运**：A/B按多个K block搬运，ScaleA/ScaleB按更大的K覆盖范围搬运，降低MTE2指令和数据搬运压力。
- **双缓冲流水**：L1 Buffer和L0 Buffer都采用Ping-Pong，让MTE2、MTE1、Cube、Fixpipe流水尽量交叠执行。

## 理论性能对比

性能指标说明表：

| 指标 | 说明 |
|------|------|
| `Task Duration(μs)` | 整个任务执行的总时间，算子端到端执行时间以该参数为准 |
| `Block Num` | 使用的核数，也就是 kernel 启动的 block 数量 |
| `aicore_time(μs)` | AI Core 的平均执行时间 |
| `aic_mac_time(μs)` | Cube 计算单元执行时间，主要对应 `Mmad` 阶段 |
| `aic_mac_ratio` | Cube 计算单元时间占比，反映计算单元利用率 |
| `aic_scalar_time(μs)` | Scalar 指令执行时间，反映循环调度、地址计算、参数配置等开销 |
| `aic_scalar_ratio` | Scalar 时间占比 |
| `aic_mte1_time(μs)` | MTE1执行时间，主要对应L1 Buffer到L0A Buffer/L0B Buffer的`Copy` |
| `aic_mte1_ratio` | MTE1时间占比，反映L1 Buffer到L0 Buffer的数据搬运压力 |
| `aic_mte2_time(μs)` | MTE2执行时间，主要对应Global Memory到L1 Buffer的`Copy` |
| `aic_mte2_ratio` | MTE2时间占比，反映Global Memory到L1 Buffer的数据加载压力 |
| `aic_fixpipe_time(μs)` | L0C->GM的`Copy`执行时间，主要对应L0C Buffer到Global Memory的结果写回 |
| `aic_fixpipe_ratio` | L0C->GM的`Copy`时间占比，反映结果写回的访存压力 |

Ascend 950PR 芯片性能数据如下：

| Case version | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Tensor API MxMatmul | 682.316 | 32 | 681.54 | 640.591 | 0.94 | 63.466 | 0.093 | 315.894 | 0.464 | 589.308 | 0.865 | 31.646 | 0.046 |

可以看到，本样例已达到理论性能峰值的 `94.0%`（即表中的`aic_mac_ratio`）。

### Cube计算性能分析

本样例的性能数据在 Ascend 950PR 上运行得到，该处理器的主频为 1.65GHz，对于 MX-FP4 的数据类型，每 cycle 处理 16×64×16 次乘加运算。

Cube 理论运算时间 $T_{cube}$ 为：

$$T_{cube} = \frac{M \times N \times K}{16 \times 64 \times 16 \times 1.65 \times 10^9 \times \text{核数}} = \frac{8192 \times 8192 \times 8192}{4096 \times 1.65 \times 10^9 \times 32} = 635.5 μs$$

从表中可以看出 `aic_mac_time` 为 `640.16 μs`，相对理论值 `635.5 μs`，误差 $E_{cube}$ 为：

$$E_{cube} = \frac{T_{actual} - T_{cube}}{T_{cube}} = \frac{640.16 - 635.5}{635.5} = 0.73 \%$$

### MTE2 带宽分析

**数据复用原理**：

矩阵乘法中，输出矩阵 C 的每个元素 $C_{i,j}$ 需要 A 的第 i 行和 B 的第 j 列参与计算。分块计算时，同一输入数据块会被多个输出块复用：

- A 矩阵按 M 方向切成 `M/baseM` 个行块，每个 A 行块参与 N 方向 `N/baseN` 个输出块的计算
- B 矩阵按 N 方向切成 `N/baseN` 个列块，每个 B 列块参与 M 方向 `M/baseM` 个输出块的计算

MxFP4 Matmul 还包含两路 ScaleA/ScaleB 输入，K 方向每 32 个元素共享一个 scale，因此 `scaleK = K/32`：

- ScaleA 形状 `[M, scaleK]`，按 M 方向切成 `M/baseM` 个行块，每个 ScaleA 行块参与 N 方向 `N/baseN` 个输出块的计算
- ScaleB 形状 `[scaleK, N]`，按 N 方向切成 `N/baseN` 个列块，每个 ScaleB 列块参与 M 方向 `M/baseM` 个输出块的计算

由于 L1/L2Cache 容量有限，无法缓存所有输入数据，同一数据块会被多次从 HBM 搬运到 L2Cache/L1，导致数据重复搬运。

**读入数据总量**：

MxFP4 Matmul 的输入包含四路数据：A、B、ScaleA、ScaleB。A/B 矩阵使用 `fp4x2_e1m2_t`，每 2 个`fp4x2_e1m2_t`元素打包成 1 byte，单元素 `sizeof = 0.5B`；ScaleA/ScaleB 使用 `fp8_e8m0_t`，单元素 `sizeof = 1B`。

本样例参数 `M=N=K=8192`，`scaleK=256`，分块参数 `baseM=baseN=256`。

读入数据总量 $D_{total}$ 为：

$$D_{total} = \frac{N}{baseN} \times M \times K \times 0.5B + \frac{M}{baseM} \times K \times N \times 0.5B + \frac{N}{baseN} \times M \times scaleK \times 1B + \frac{M}{baseM} \times scaleK \times N \times 1B$$

$$= (32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 256 \times 1 + 32 \times 256 \times 8192 \times 1) B$$

$$= (1GB + 1GB + 64MB + 64MB) = 2.125GB$$

**MTE2 理论耗时**：

Ascend 950PR 芯片：L2Cache 峰值带宽约 5TB/s，HBM（对应 Global Memory）带宽约 1.6TB/s。最理想情况下首次访问从 HBM 获取数据并缓存到 L2Cache，后续访问直接从 L2Cache 读取。

> **单位说明**：带宽单位采用十进制，1 TB/s = 10^12 B/s。

首次从 HBM 读入的数据总量 $D_{HBM}$ 为：

$$D_{HBM} = M \times K \times 0.5B + K \times N \times 0.5B + M \times scaleK \times 1B + N \times scaleK \times 1B = 32MB + 32MB + 2MB + 2MB = 68MB$$

从 L2Cache 读入的数据总量 $D_{L2Cache}$ 为：

$$D_{L2Cache} = D_{total} - D_{HBM} = 2.125GB - 68MB \approx 2.057GB$$

MTE2 理论耗时 $T_{MTE2}$ 为：

$$T_{MTE2} = \frac{D_{HBM}}{1.6TB/s} + \frac{D_{L2Cache}}{5TB/s} = \frac{68MB}{1.6TB/s} + \frac{2.057GB}{5TB/s} \approx 42.5μs + 411.4μs = 453.9μs$$

MTE2 耗时误差 $E_{MTE2}$ 为：

$$E_{MTE2} = \frac{T_{actual} - T_{MTE2}}{T_{MTE2}} = \frac{588.736μs - 453.9μs}{453.9μs} = 29.7\%$$

Ascend 950PR中 L2Cache 大小为 128MB，无法缓存所有输入数据，部分数据在搬运时会发生 L2Cache miss，需要从 HBM 获取。用户可进一步优化 L2Cache 切分策略以提高 MTE2 带宽。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

> **注意事项**：本样例使用的 `en_dtypes` 库需要版本 `0.0.4`。安装命令为

```bash
pip3 install en_dtypes==0.0.4
```

### 配置环境变量

请根据当前环境上 CANN 开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令，**当前仅支持使用CANN master**。

- 默认路径，root 用户安装 CANN 软件包

  ```bash
  source /usr/local/Ascend/cann/set_env.sh
  ```

- 默认路径，非 root 用户安装 CANN 软件包

  ```bash
  source $HOME/Ascend/cann/set_env.sh
  ```

- 指定路径 install_path，安装 CANN 软件包

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

### 样例执行

```bash
mkdir -p build && cd build;  # 创建并进入 build 目录
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j;
python3 ../scripts/gen_data.py
./demo
python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
```

使用 NPU 仿真模式时，设置 `-DCMAKE_ASC_RUN_MODE=sim` 即可。

```bash
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU 模式
cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU 仿真模式
```

编译选项说明：

| 参数 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_RUN_MODE` | `npu`/`sim` | 运行模式：NPU运行、NPU仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 目标 SoC 架构 |

> **注意：** 切换`CMAKE_ASC_RUN_MODE`/`CMAKE_ASC_ARCHITECTURES`前需清理CMake缓存，可在build目录下执行`rm CMakeCache.txt`后重新执行CMake。

执行结果如下，说明精度对比成功。

```bash
test pass!
```

### NPU模式性能分析

使用 `msprof` 工具获取NPU模式运行时详细的性能数据：

```bash
msprof ./demo
```

当前目录下会生成 PROF_ 前缀的文件夹，`mindstudio_profiler_output` 目录保存性能数据汇总，性能数据分析推荐查看该目录下文件：

```bash
PROF_xxxx_XXXXXX
├── device_{id}
├── host
├── mindstudio_profiler_log
└── mindstudio_profiler_output
    ├── msprof_*.json
    ├── xx_*.csv
    └── README.txt
```

查看具体的性能分析结果：

```bash
cat ./PROF_*/mindstudio_profiler_output/op_summary_*.csv
```
