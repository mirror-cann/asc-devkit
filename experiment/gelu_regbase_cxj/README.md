# Gelu性能调优样例

## 概述

本样例以Gelu计算为例，介绍RegBase矢量计算的开发过程。

## 支持的产品
- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```
├── gelu_regbase
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gelu.asc                // Ascend C样例实现
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
<tr><td align="center">x</td><td align="center">[8, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gelu_custom</td></tr>
</table>

## 样例实现

**实现方式**：

使用寄存器级别的向量计算接口，减少数据在UB和Reg之间的交互。

**关键代码**：
```cpp
__simd_vf__ inline static void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t count, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    for (uint16_t i = 0; i < loopNum; ++i) {
        // Set mask according to count; count -= oneRepeatSize after execution;
        mask = AscendC::Reg::UpdateMask<float>(count);

        // Step1: Load data from unified buffer to register;
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);

        // Step2: Compute;
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);

        // Step3: Store data from register to unified buffer;
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**样例配置**：
- 多核切分：共计8 * 8192个数据，分布到8个核上计算，单核计算8192个数据。（本例简化仅对多核进行说明，不代表实际多核切分优化逻辑）

**RegBase API优势**：
  - 寄存器级别的数据访问，减少中间数据的Load/Store开销
  - 支持Hardware Loop优化，循环可被优化为硬件级向量循环


## 理论性能分析

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

在RegBase场景下，vector指令具有双发特性，即常规计算指令并行度可以达到512 bytes/cycle，但是exp和div在双发场景下不能简单按照128 bytes/cycle进行估计。
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

- 当一个VF函数loop循环内包含的指令依赖链过长时，可能导致每cycle可以双发的指令数变少，导致性能下降。此时可以将过长的loop循环切分成多个，比如可以选择reduce结束，或者长latency指令的结束点（比如div、exp），将一个loop循环拆分成2~3个loop循环。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510;make -j;                      # 编译工程
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```
