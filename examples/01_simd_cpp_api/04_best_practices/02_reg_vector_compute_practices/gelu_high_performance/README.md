# Gelu性能调优样例

## 概述

本样例以Gelu计算为例，介绍RegBase的向量性能调优方法，样例展示使能VF融合和循环展开之后的性能收益情况。

**优化路径**：
- Case 0: Gelu未使能VF融合能力（基准）
- Case 1: 启用RegBase API和VF融合能力
- Case 2: 启用RegBase API、VF融合能力和循环展开优化

## 支持的产品
- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```
├── gelu_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gelu.asc                // Ascend C样例实现（包含2个优化case）
```

## 样例描述

**样例功能**：

Gelu近似公式计算为：

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right) \tag{1}
$$

tanh的计算公式为：

$$
\tanh(u) = \frac{e^{2u} - 1}{e^{2u} + 1} \tag{2}
$$

其中，$u = \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)$。

将tanh公式代入Gelu公式并化简，得到：

$$
GELU(x) \approx \frac{x}{1 + e^{-2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)}} \tag{3}
$$

其中，$-2 \cdot \sqrt{\frac{2}{\pi}} \approx -1.595769$。

本样例以公式（3）进行编程计算，用户在设计向量算子时，需要考虑对原始计算进行化简，可以有效减少计算步骤和内存占用。

## 计算步骤分析

为说明公式化简对算子性能的影响，对比三种不同的Gelu计算实现方式。

**方式1：**

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \frac{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} - 1}{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} + 1}\right)
$$

**方式2：**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

**方式3：**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

| 计算方式 | 计算指令数 | Unified Buffer（UB）内存份数 |
|:---|:---:|:---:|
| 方式1 | 13 | 5 |
| 方式2 | 8 | 3 |
| 方式3 | 8 | 2 |

令输入UB内存为xLoacal，输出UB内存为yLocal，临时UB内存统一用tmp0、tmp1、tmp2等表示

**方式1计算步骤分解**（共13条指令，需5份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | xLocal, yLocal |
| 5 | yLocal = √(2/π) * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = 2u | Muls | yLocal |
| 7 | tmp0 = e^(2u) | Exp | tmp0, yLocal |
| 8 | tmp1 = e^(2u) - 1 | Adds | tmp1, yLocal |
| 9 | tmp2 = e^(2u) + 1 | Adds | tmp2, yLocal |
| 10 | tmp1 = tanh(u) = (e^(2u) - 1) / (e^(2u) + 1) | Div | tmp1, tmp2 |
| 11 | yLocal = 1 + tanh(u) | Adds | yLocal, tmp1 |
| 12 | yLocal = x * (1 + tanh(u)) | Mul | xLocal, yLocal |
| 13 | yLocal = 0.5 * x * (1 + tanh(u)) | Muls | yLocal |

**方式2计算步骤分解**（共8条指令，需3份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = -0.071405 * x³ | Muls | yLocal |
| 4 | tmp0 = -1.595769 * x | Muls | tmp0, xLocal |
| 5 | yLocal = -1.595769 * x + (-0.071405 * x³) | Add | yLocal, tmp0 |
| 6 | yLocal = e^(-1.595769 * x - 0.071405 * x³) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**方式3计算步骤分解**（共8条指令，需2份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | yLocal, xLocal |
| 2 | yLocal = x³ | Mul | yLocal, xLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | yLocal |
| 5 | yLocal = -1.595769 * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = e^(-1.595769 * (x + 0.044715 * x³)) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**对比总结**：

- **计算指令数**：方式2和方式3均比方式1减少 **5条指令**
- **内存份数**：方式2比方式1减少 **1份内存**，方式3比方式1减少 **3份内存**
- **实现选择**：方式3是最优实现，计算指令数最少，内存占用最低，本样例采用方式3实现

**样例规格**：

<table border="2">
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Gelu</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gelu_custom</td></tr>
</table>

## 样例实现

### 性能指标说明

**表1 AI Core 性能指标字段说明表**

| 字段名 | 字段含义 |
|:---|:---|
|Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
|aiv_time(μs)|Task在AI Vector Core上的执行时间。|
|aiv_vec_time(μs)|vec类型指令（向量类运算指令）耗时。|
|aiv_vec_ratio|vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_scalar_time(μs)|scalar类型指令（标量类运算指令）耗时。|
|aiv_scalar_ratio|scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_mte2_time(μs)|mte2类型指令（GM->UB搬运类指令）耗时。|
|aiv_mte2_ratio|mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。|
|aiv_mte3_time(μs)|mte3类型指令（UB->GM搬运类指令）耗时。|
|aiv_mte3_ratio|mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。|

### Case 0: Gelu未使能VF融合能力

**实现方式**：参考 `KernelGelu::GeluCompute()` 函数实现

基准程序使用Ascend C基础API实现Gelu计算，包括Mul、Muls、Add、Exp、Adds、Div等向量指令。

**关键代码**：
```cpp
__aicore__ inline void GeluCompute(
        const AscendC::LocalTensor<float>& xLocal, const AscendC::LocalTensor<float>& yLocal, uint32_t n)
{
    // yLocal = x * x = x²
    AscendC::Mul(yLocal, xLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x² * x = x³
    AscendC::Mul(yLocal, yLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x³ * 0.044715 = 0.044715 * x³
    AscendC::Muls(yLocal, yLocal, COEFF_A, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x + 0.044715 * x³
    AscendC::Add(yLocal, xLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = -1.595769 * (x + 0.044715 * x³)
    AscendC::Muls(yLocal, yLocal, COEFF_B, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Exp(yLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = 1 + e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Adds(yLocal, yLocal, (float)1.0, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x / (1 + e^(-1.595769 * (x + 0.044715 * x³)))
    AscendC::Div(yLocal, xLocal, yLocal, n);
}
```

**样例配置**：
- 多核切分：M方向分2份，N方向32份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 351.525 | 350.64 | 147.895 | 0.422 | 9.513 | 0.027 | 320.283 | 0.913 | 303.647 | 0.866 |

**优化效果分析**：
- 端到端耗时：**351.525μs**
- 向量指令耗时：147.895μs，占比 **42.2%**
- 数据搬运耗时：320.283μs（读）+ 303.647μs（写），搬运占比超过 **90%**

**原理说明**：
- 计算流程按照公式(3)执行，包含8次向量运算，使用基础API进行向量计算
- 每次向量计算内部会进行load -> compute -> store操作，每次计算数据均需要在Unified Buffer(UB)和Reg之间交互

**下一步优化方向**：
- 启用RegBase API和VF融合能力减少数据在UB和Reg之间的交互，在Reg内部完成最终计算结果的生成

---

### Case 1: 启用RegBase API和VF融合能力

**实现方式**：参考 `KernelGelu::GeluVfBasic()` 函数实现

将基础API转换为RegBase API，使用寄存器级别的向量计算接口，减少数据在UB和Reg之间的交互。

**关键代码**：
```cpp
__simd_vf__ inline void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**样例配置**：
- 多核切分：M方向分2份，N方向32份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

**优化手段**：
- **RegBase API优势**：
  - 寄存器级别的数据访问，减少中间数据的Load/Store开销
  - 支持Hardware Loop优化，循环可被优化为硬件级向量循环

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 348.868 | 347.99 | 66.277 | 0.19 | 3.03 | 0.009 | 320.543 | 0.921 | 314.547 | 0.904 |

**优化效果分析**：
- 端到端耗时：**348.868μs**，相比Case 0耗时减少 **0.76%**，本样例为MTE2 bound，因此端到端收益不明显
- 向量指令耗时：66.277μs，相比Case 0耗时减少 **55.2%**

---

### Case 2: 启用RegBase API、VF融合能力和循环展开优化

**实现方式**：参考 `KernelGelu::GeluVfBasic()` 函数实现，添加 `#pragma unroll 6` 循环展开优化

在Case 1中由于Gelu计算依赖路径较长，使用循环展开优化，提高指令级并行度。

**关键代码**：
```cpp
__simd_vf__ inline static void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    #pragma unroll 6  // 循环展开优化
    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        // ……
    }
}
```

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

**优化手段**：
- **循环展开优化原理**：
  - 通过 `#pragma unroll 6` 告诉编译器展开循环，每次展开6个迭代
  - 提高指令级并行度，使更多VF指令能够连续发射

- **循环展开收益分析**：
  - 展开因子选择：`unroll 6` 为经验值，需要根据实际场景调优
  - 展开过多：可能增加寄存器压力，导致性能劣化，同时代码大小也会增加
  - 展开过少：优化效果不明显
  - 建议：用户可以采用逐次尝试的方法，找到最优的循环展开次数

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 344.436 | 343.82 | 63.655 | 0.185 | 4.757 | 0.014 | 315.468 | 0.918 | 306.069 | 0.89 |

**优化效果分析**：
- 端到端耗时：**344.436μs**，相比Case 0耗时减少 **2.0%**，相比Case 1耗时减少 **1.3%**（MTE2 bound，端到端收益不明显）
- 向量指令耗时：63.655μs，相比Case 0耗时减少 **56.9%**，相比Case 1耗时减少 **4.6%**

---

## 性能对比总结

### Ascend 950PR性能对比

下表为本样例在Ascend 950系列产品上运行的性能数据对比：

| Case | 优化策略 | 核数 | tileLen | Task Duration(μs) | aiv_vec_time(μs) | 理论vector耗时(μs) | 端到端耗时相对Case 0 | vector耗时相对Case 0 |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 0 | Gelu未使能VF融合能力（基准） | 64 | 8192 | 351.525 | 147.895 | 139.02 | 1x | 1x |
| 1 | 启用RegBase API和VF融合能力 | 64 | 8192 | 348.868 | 66.277 | NA | 1.01x | 2.23x |
| 2 | 启用RegBase API、VF融合和循环展开 | 64 | 8192 | 344.436 | 63.655 | NA | 1.02x | 2.32x |

> **注意：** 该样例为MTE2 bound，性能瓶颈为数据搬运，下述分析主要针对vector耗时计算，方便用户分析使能VF融合和循环展开的性能收益。Case 2相比Case 1，vector耗时减少 **4.6%**。

### 理论性能分析

下表为本样例的vector计算指令的并行度：

| 序号 | 指令 | 数据类型 | 计算并行度(bytes/cycle) |
|:---:|:---:|:---:|:---:|
| 1 | Mul | float | 256 |
| 2 | Mul | float | 256 |
| 3 | Muls | float | 256 |
| 4 | Add | float | 256 |
| 5 | Muls | float | 256 |
| 6 | Exp | float | 64 |
| 7 | Adds | float | 256 |
| 8 | Div | float | 64 |

**理论vector耗时计算**：

**Step 1: 计算Gelu计算流程的等效每cycle数据处理量（不考虑VF融合指令双发特性）**

Gelu计算包含8条向量指令，各指令的并行度如下：
- 6条线性运算指令（Mul×2、Muls×2、Add×1、Adds×1）：并行度256 bytes/cycle
- 2条非线性运算指令（Exp×1、Div×1）：并行度64 bytes/cycle

处理1 byte数据需要的cycle数：
- 线性运算指令：每条需要 1/256 cycles
- 非线性运算指令：每条需要 1/64 cycles

处理1 byte数据所需总cycle数：

$$
Cycles_{\text{per\_byte}} = 6 \times \frac{1}{256} + 2 \times \frac{1}{64} = \frac{6}{256} + \frac{2}{64} = \frac{14}{256}
$$

等效每cycle处理数据量：

$$
P_{\text{bytes}} = \frac{256}{14} = \frac{128}{7} \approx 18.29 \text{ bytes/cycle}
$$

**Step 2: 计算理论vector耗时（不考虑VF融合指令双发特性）**

本样例在Ascend 950PR系列产品上运行，硬件参数：
- 主频 $f = 1650 \text{ MHz} = 1.65 \times 10^9 \text{ Hz}$
- AIV核数 $N_{\text{core}} = 64$
- 数据shape $M = N = 8192$，总数据量 $D = M \times N \times 4 = 268435456 \text{ bytes}$

理论vector耗时计算公式：

$$
T_{\text{theory}} = \frac{D}{P_{\text{bytes}} \times f \times N_{\text{core}}}
$$

代入数值：

$$
T_{\text{theory}} = \frac{268435456}{\frac{128}{7} \times 1.65 \times 10^9 \times 64} = \frac{268435456 \times 7}{128 \times 1.056 \times 10^{11}} = \frac{1879048192}{1.35168 \times 10^{11}} \approx 139.02 \times 10^{-6} \text{ s} = 139.02 \text{ μs}
$$

**Step 3: 计算VF融合场景下理论vector耗时**

在VF融合场景下，vector指令具有双发特性，即常规计算指令并行度可以达到512 bytes/cycle，但是exp和div在双发场景下不能简单按照128 bytes/cycle进行估计。
如下图所示，在time=0时刻，同时发射Exp和Mul指令，Mul只需要1个cycle执行完成，但是Exp需要4个cycle执行完成，在Exp执行期间，time=1时刻同时发射了Mul和Muls并行执行，并以此类推继续发射其它指令。

```
Time:     0     1     2     3     4     5     6     7
          |-----|-----|-----|-----|-----|-----|-----|
Exp:      |<=====================>|
Mul:      |<--->|
Mul:            |<--->|
Muls:           |<--->|
Add:                  |<--->|
Adds:                 |<--->|
Muls:                       |<--->|
Adds:                       |<--->|
```
因此在VF融合场景下，更多采用IPC(Instructions Per Cycle，每cycle指令发射数量)来衡量性能。

case 1场景，单核vector计算指令数量：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

式中：
- $N_{\text{VF}} = 128$：VF函数调用次数
- $N_{\text{loop}} = 128$：VF内部循环次数
- $N_{\text{op}} = 8$：VF内计算指令数（Mul×2、Muls×2、Add×1、Adds×1、Exp×1、Div×1）

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 66.277 \text{ μs} \times 1650 \text{ MHz} = 109357
$$

式中：
- $T_{\text{vec}} = 66.277 \text{ μs}$：实测vector计算耗时
- $f = 1650 \text{ MHz}$：硬件主频

IPC计算：

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{109357} \approx 1.20
$$

式中：
- $N_{\text{instr}} = 131072$：单核vector计算指令数量
- $Cycles_{\text{vec}} = 109357$：vector计算cycle数

IPC越高越好，理论极限接近2。

**Case 2: 循环展开优化后的IPC分析**

case 2场景，在VF融合基础上添加循环展开优化，单核vector计算指令数量保持不变：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

式中各参数与Case 1相同，循环展开不影响总指令数量。

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 63.655 \text{ μs} \times 1650 \text{ MHz} = 105031
$$

IPC计算：

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{105031} \approx 1.25
$$

**循环展开对IPC的影响分析**：
- Case 2的IPC为1.25，相比Case 1的IPC（1.20）提升了 **4.2%**
- 循环展开优化提高了vector指令发射效率，使得更多的指令能够并行执行

**IPC优化建议**：
- 在VF融合场景下，IPC是衡量vector指令发射效率的重要指标
- 理想IPC极限接近2.0，大多数情况下达成1.4~1.5则表示取得较优性能
- 当一个VF函数loop循环内包含的指令依赖链过长时，会导致执行队列中存放的循环次数太少，从而导致每cycle可以双发的指令数变少，导致性能下降。此时可以将过长的loop循环切分成多个，比如可以选择reduce结束，或者长latency指令的结束点（比如div、exp），将一个loop循环拆分成2~3个loop循环。本样例给出的示例loop循环较短，不适用于该优化项。

### 优化要点总结

| 优化手段 | 核心原理 | 使用建议 |
|:---|:---|:---|
| 公式化简 | 减少计算步骤，降低计算开销 | 优先进行公式推导和化简 |
| RegBase API + VF融合 | 寄存器级计算减少中间Load/Store，利用双发特性提升性能 | 使用asc_vf_call调用VF函数，利用双发特性提高IPC |
| 循环展开 | 提高指令发射并行度 | 使用`#pragma unroll N`，N需根据实际调优 |

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的 case，各 case 说明：
  - `0`: Gelu未使能VF融合能力（需设置 `-DCMAKE_VF_MODE=false`）
  - `1`: 启用RegBase API和VF融合能力
  - `2`: 启用RegBase API、VF融合能力和循环展开优化

  > **注意：** 编译器有VF自动融合的能力，默认会使能VF自动融合，本样例为方便对比分析性能，在case 0时需要关闭VF自动融合能力。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
  - 默认路径，root用户安装CANN软件包
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - 默认路径，非root用户安装CANN软件包
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - 指定路径install_path，安装CANN软件包
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- 样例执行

  ```bash
  SCENARIO_NUM=0
  CMAKE_VF_MODE=false
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或Case前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1`、`2`　　　　　| Case编号：0=Gelu未使能VF融合，1=启用RegBase API和VF融合，2=启用RegBase API、VF融合和循环展开 |
  | `CMAKE_VF_MODE` | `true`、`false`　　　　　| VF融合模式：case 0时需设置为false关闭VF自动融合 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

### 性能分析

使用 `msprof` 工具获取详细性能数据：

```bash
msprof ./demo   # 分析性能
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