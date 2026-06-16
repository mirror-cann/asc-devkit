# GMToL1 Load2DV2指令搬运<a id="ZH-CN_TOPIC_0000002594327301"></a>

## 产品支持情况<a id="section1550532418810"></a>

| 产品 | 是否支持 |
| ---------- | :----------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √ </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |
| <cann-filter npu-type = "x90">Kirin X90 | x </cann-filter> |
| <cann-filter npu-type = "9030">Kirin 9030 | x </cann-filter> |

## 功能说明<a id="section618mcpsimp"></a>

头文件路径为：basic_api/kernel_operator_mm_intf.h。

负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位从Global Memory搬运至L1 Buffer（TPosition为A1/B1）。

## 函数原型<a id="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParam)
```

## 参数说明<a id="section622mcpsimp"></a>

**表 1** 模板参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| T | 源操作数和目的操作数的数据类型。 |

**表 2** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。支持的物理存储位置为L1 Buffer（TPosition为A1/B1）。<br>数据分形无格式要求，一般情况下为NZ格式。NZ格式下，对应的分形大小为16 \* (32字节 / sizeof(T))。 |
| src | 输入 | 源操作数，类型为GlobalTensor。数据类型需要与dst保持一致。 |
| loadDataParam | 输入 | LoadData参数结构体，类型为LoadData2DParamsV2，具体参考[表3](#table49630346128)。 |

**表 3** LoadData2DParamsV2结构体内参数说明<a id="table49630346128"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| mStartPosition | 以M*K矩阵为例，源矩阵M轴方向的起始位置，单位为16个元素。 |
| kStartPosition | 以M*K矩阵为例，源矩阵K轴方向的起始位置，单位为32字节。 |
| mStep | 以M*K矩阵为例，源矩阵M轴方向搬运长度，单位为16个元素。取值范围：mStep∈[0, 255]。<br>**注：mStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| kStep | 以M*K矩阵为例，源矩阵K轴方向搬运长度，单位为32字节。取值范围：kStep∈[0, 255]。<br>**注：kStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 以M*K矩阵为例，源矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位为512字节。 |
| dstStride | 以M*K矩阵为例，目的矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位为512字节。 |
| ifTranspose | 是否启用转置功能，对每个分形矩阵进行转置，默认为false。<br>&nbsp;&nbsp;&bull; true：启用。<br>&nbsp;&nbsp;&bull; false：不启用。<br>注意：本通路场景下（GM->L1 Buffer）不支持转置，参数无意义，保持默认值即可。 |
| sid | 预留参数，配置为0即可。 |

LoadData2DParamsV2结构体在不启用转置时，示意图如下，参数设置值和解释说明如下：

- mStartPosition = 2，表示源矩阵M轴方向的起始位置为第32个元素（2*16个元素）。
- kStartPosition = 2，表示源矩阵K轴方向的起始位置为第32个元素（2*32字节）。
- mStep = 2，表示源矩阵M轴方向搬运长度为32个元素（2*16个元素）。
- kStep = 3，表示源矩阵K轴方向搬运长度为48个元素（3*32字节）。
- srcStride = 5，表示源矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔为1280个元素（5*512字节）。
- dstStride = 3，表示目的矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔为768个元素（3*512字节）。
- ifTranspose = false，表示不启用转置。
- sid = 0，预留参数，配置为0即可。

**图 1** LoadData2DParamsV2结构体参数示例（以half数据类型，不启用转置为例）<a id="fig13901164574218"></a>

![](../../../../../figures/LoadData2DParamsV2结构体参数示例（以half数据类型-不启用转置为例）.png)

## 数据类型<a id="section4219135304818"></a>

支持数据类型为：uint8_t、int8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、uint32_t、int32_t、float。

## 返回值说明

无

## 约束说明<a id="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。
- 本通路场景下不支持转置。

## 调用示例<a id="section6461234123118"></a>

无