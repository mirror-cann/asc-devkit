# Load2DMX<a id="ZH-CN_TOPIC_0000002517258774"></a>

## 产品支持情况<a id="section1550532418810"></a>

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

## 功能说明<a id="section618mcpsimp"></a>

Load2DMX接口支持如下数据通路的搬运：

L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer

## 函数原型<a id="section620mcpsimp"></a>

- Load2DMX接口

    ```cpp
    template <typename T, typename U = T>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

- Load2Dv2MX接口，支持源操作数和目的操作数数据类型不一致。

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

## 参数说明<a id="section622mcpsimp"></a>

**表 1** 模板参数说明<a name="table07381635103112"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| T | 源操作数和目的操作数的数据类型。 |
| U | 针对Load2DMX接口，U用来表示dst的数据类型，当src为fp8_e5m2_t、fp8_e4m3fn_t时，U需为T对应的MX数据类型，即AscendC::mx_fp8_e5m2_t和AscendC::mx_fp8_e4m3_t，否则编译失败。除此之外的数据类型要求T和U一致。 |

**表 2** 通用参数说明<a name="table18368155193919"></a>

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在TPosition决定，具体约束如下：<br>&nbsp;&nbsp;&bull; A2：ZZ格式/NZ格式；对应的分形大小为16 \* (32字节 / sizeof(T))。<br>&nbsp;&nbsp;&bull; B2：ZN格式；对应的分形大小为 (32字节 / sizeof(T))  \* 16。<br>&nbsp;&nbsp;&bull; A1/B1：无格式要求，一般情况下为NZ格式。NZ格式下，对应的分形大小为16 * (32字节 / sizeof(T))。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>数据类型需要与dst保持一致。 |
| srcMx | 输入 | 源操作数，类型为LocalTensor，仅支持fp8_e8m0_t类型。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadData2DParamsV2，具体参考表[Load2DV2](Load2DV2.md)中的LoadData2DParamsV2结构体内参数说明。 |
| loadMxDataParams | 输入 | LoadData参数结构体，类型为LoadData2DMxParams，具体参考[表3](#table15901153712305)。<br>上述结构体参数定义请参考\$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_mm.h，\$\{INSTALL\_DIR\}请替换为CANN软件安装后文件存储路径。 |

**表 3** LoadData2DMxParams结构体参数说明<a name="table15901153712305"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| xStartPosition | 源矩阵X轴方向的起始位置，即M维度方向，单位为1个分形（1个单位代表一个32字节的分形）。 |
| yStartPosition | 源矩阵Y轴方向的起始位置，即K维度方向，单位为32字节。 |
| xStep | 源矩阵X轴方向搬运长度，即M维度方向，单位为1个分形（1个单位代表一个32字节的分形）。取值范围：xStep∈[0, 255]。 |
| yStep | 源矩阵Y轴方向搬运长度，即K维度方向，单位为32字节。取值范围：yStep∈[0, 255]。 |
| srcStride | 源矩阵X方向前一个分形起始地址与后一个分形起始地址的间隔，单位为32字节。 |
| dstStride | 目标矩阵X方向前一个分形起始地址与后一个分形起始地址的间隔，单位为32字节。 |

下面通过一个具体的示例来解释LoadData2DMxParams结构体参数。假设A矩阵shape为（M，K），则ScaleA矩阵shape为（M，K/32），ScaleA数据类型为fp8_e8m0_t，ScaleA矩阵分形排布见[图1](#fig138710913432)。

**图 1** ScaleA在L0A Buffer的分形排布<a id="fig138710913432"></a>

![](../../../../../figures/ScaleA在L0A的分形排布.png "ScaleA在L0A Buffer的分形排布")

下图为ScaleA从L1 Buffer搬运至L0A Buffer过程中的配置参数示意。每一行为32字节，对应着[图1](#fig138710913432)中的一个分形。xStep为M维度分形的个数，如图中的xStep = M / 16 = 3，yStep为K维度32字节的个数，如图中的yStep = K / 32 / 2 = 21，srcStride和dstStride同理，表示在K维度上32字节的个数。

![](../../../../../figures/Nd2Nz转换示意图.png)

## 数据类型

支持数据类型为：fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e5m2_t、fp8_e4m3fn_t。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。

## 调用示例

无