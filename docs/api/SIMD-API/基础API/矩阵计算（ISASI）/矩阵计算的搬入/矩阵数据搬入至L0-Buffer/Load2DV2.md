# Load2DV2

## 产品支持情况

| 产品 | 是否支持 |
| ---------- | :----------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √ </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品 AI Core | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品 Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |
| <cann-filter npu-type = "x90">Kirin X90 | x </cann-filter> |
| <cann-filter npu-type = "9030">Kirin 9030 | x </cann-filter> |

## 功能说明

头文件路径为：basic_api/kernel_operator_mm_intf.h。

负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位进行，支持如下数据通路：

- L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

对于不同的数据类型，每个数据分形对应的矩阵如下：

- 对于b4数据类型，每个数据分形在L0A Buffer中为一个16×64的矩阵，在L0B Buffer中为一个64×16的矩阵。
- 对于b8数据类型，每个数据分形在L0A Buffer中为一个16×32的矩阵，在L0B Buffer中为一个32×16的矩阵。
- 对于b16数据类型，每个数据分形为一个16×16的矩阵。
- 对于b32数据类型，每个数据分形在L0A Buffer中为一个16×8的矩阵，在L0B Buffer中为一个8×16的矩阵。

## 函数原型

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParam)
```

## 参数说明

**表1** 模板参数说明

| 参数名称 | 含义 |
| ---------- | ------ |
| T | 源操作数和目的操作数的数据类型。 |

**表2** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](../矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](../矩阵计算输入搬运约束.md)。<br>数据类型和src的数据类型保持一致。<br>支持的物理存储位置为L0A Buffer（TPosition: A2）/L0B Buffer（TPosition: B2）。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](../矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](../矩阵计算输入搬运约束.md)。<br>数据类型和dst的数据类型保持一致。<br>支持的物理存储位置为L1 Buffer（TPosition: A1/B1）/Global Memory（TPosition: GM）。 |
| loadDataParam | 输入 | LoadData参数结构体，类型为LoadData2DParamsV2，具体参考[表3](#表3)。 |

**表3** LoadData2DParamsV2结构体内参数说明<a id="表3"></a>

| 参数名称 | 含义 |
| ---------- | ------ |
| mStartPosition | 以M×K矩阵为例，源矩阵M轴方向的起始位置，单位为16个元素。 |
| kStartPosition | 以M×K矩阵为例，源矩阵K轴方向的起始位置，单位为32字节。 |
| mStep | 以M×K矩阵为例，源矩阵M轴方向搬运长度，单位为16个元素。取值范围：mStep∈[0, 255]。<br>通过ifTranspose参数启用转置功能时，mStep除需满足取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull; 当数据类型为b4时，mStep必须是4的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b8时，mStep必须是2的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b16时，mStep必须是1的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b32时，mStep无额外约束。<br>**注：mStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| kStep | 以M×K矩阵为例，源矩阵K轴方向搬运长度，单位为32字节。取值范围：kStep∈[0, 255]。<br>通过ifTranspose参数启用转置功能时，kStep除需满足取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull; 当数据类型为b4、b8或b16时，kStep没有额外约束；<br>&nbsp;&nbsp;&bull; 当数据类型为b32时，kStep必须是2的倍数。<br>**注：kStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 以M×K矩阵为例，源矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位：512字节。 |
| dstStride | 以M×K矩阵为例，目标矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位：512字节。 |
| ifTranspose | 是否启用转置功能，对每个分形矩阵进行转置，默认为false：<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>注意：只有L1 Buffer（TPosition: A1）->L0A Buffer（TPosition: A2）和L1 Buffer（TPosition: B1）->L0B Buffer（TPosition: B2）通路才能开启转置。开启转置功能时，支持b4、b8、b16、b32数据类型。 |
| sid | 预留参数，配置为0即可。<br>注：兼容旧款产品接口传入，Ascend 950PR/Ascend 950DT产品不做处理。 |

LoadData2DParamsV2结构体在不启用转置时，示意图如下，参数设置值和解释说明如下：

- mStartPosition=2，表示源矩阵M轴方向的起始位置为第32个元素（2×16个元素）。
- kStartPosition=2，表示源矩阵K轴方向的起始位置为第32个元素（2×32字节）。
- mStep=2，表示源矩阵M轴方向搬运长度为32个元素（2×16个元素）。
- kStep=3，表示源矩阵K轴方向搬运长度为48个元素（3×32字节）。
- srcStride=5，表示源矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔为1280个元素（5×512字节）。
- dstStride=3，表示目的矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔为768个元素（3×512字节）。
- ifTranspose=false，表示不启用转置。
- sid=0，预留参数，配置为0即可。

**图1** LoadData2DParamsV2结构体参数示例（以half数据类型，不启用转置为例）

![](../../../../../figures/LoadData2DParamsV2结构体参数示例（以half数据类型-不启用转置为例）.png)

## 数据类型

支持的数据类型：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明

无

## 约束说明

无

## 调用示例

示例代码片段如下，仅展示样例中的部分代码，完整使用样例请参见[Load2DV2样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/01_matrix_compute/load_data_2dv2_l12l0)。

```cpp
// Load2DV2: Nz -> Nz
uint32_t m = 40;
uint32_t k = 70;
uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t fractalSize = fractalShape[0] * fractalShape[1];
uint16_t mStep = (m + fractalShape[0] - 1) / fractalShape[0];

AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = mStep;
loadDataParams.kStep = (k + fractalShape[1] - 1) / fractalShape[1];
loadDataParams.srcStride = mStep;
loadDataParams.dstStride = mStep;
loadDataParams.ifTranspose = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
