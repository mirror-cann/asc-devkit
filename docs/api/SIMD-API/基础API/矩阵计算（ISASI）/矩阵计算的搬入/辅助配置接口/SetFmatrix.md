# SetFmatrix<a name="ZH-CN_TOPIC_0000001834660673"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
| ------ | :------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √ </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | √ </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | √ </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |
| <cann-filter npu-type = "x90">Kirin X90 | √ </cann-filter> |
| <cann-filter npu-type = "9030">Kirin 9030 | √ </cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：basic_api/kernel_operator_mm_intf.h。

用于调用[Load3D](../矩阵数据搬入至L0-Buffer/Load3D.md)时设置FeatureMap的属性描述。Load3D的模板参数isSetFMatrix设置为false时，表示Load3D传入的FeatureMap的属性（包括l1H、l1W、padList，参数介绍参考[表3 LoadData3DParamsV1结构体内参数说明](../矩阵数据搬入至L0-Buffer/Load3D.md#zh-cn_topic_0000002512171652_table679014222918)、[表4 LoadData3DParamsV2结构体内参数说明](../矩阵数据搬入至L0-Buffer/Load3D.md#zh-cn_topic_0000002512171652_table193501032193419)）将不生效，开发者需要通过该接口进行设置。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFmatrix(uint16_t l1H, uint16_t l1W, const uint8_t padList[4], const FmatrixMode& fmatrixMode)
```

## 参数说明<a name="section622mcpsimp"></a>

<a name="table8955841508"></a>
**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --------- | ---------- | ------ |
| l1H | 输入 | 源操作数height，取值范围：l1H∈[1, 32767]。 |
| l1W | 输入 | 源操作数width，取值范围：l1W∈[1, 32767]。 |
| padList | 输入 | padding列表 [padding\_left, padding\_right, padding\_top, padding\_bottom]，每个元素取值范围：[0,255]。默认为{0, 0, 0, 0}。 |
| fmatrixMode | 输入 | 用于控制LoadData指令从left还是right寄存器获取信息。FmatrixMode类型，定义如下。当前只支持FMATRIX\_LEFT，左右矩阵均使用该配置。<br><pre>enum class FmatrixMode : uint8_t {<br>    FMATRIX_LEFT = 0,<br>    FMATRIX_RIGHT = 1,<br>};</pre> |

## 返回值说明

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口需要配合[Load3D](../矩阵数据搬入至L0-Buffer/Load3D.md)接口一起使用，需要在[Load3D](../矩阵数据搬入至L0-Buffer/Load3D.md)接口之前调用，其中fmatrixMode参数需要和Load3D接口参数中的fMatrixCtrl值保持一致。
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

完整调用示例参考[SetLoadDataBoundary样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary)。

```cpp
uint16_t channelSize = 32;
uint16_t H = 4, W = 4;
uint8_t Kh = 2, Kw = 2;
uint16_t Cout = 16;
uint16_t C0, C1;
uint8_t dilationH = 2, dilationW = 2;

uint8_t padList[PAD_SIZE] = {0, 0, 0, 0};
AscendC::SetFmatrix(H, W, padList, FmatrixMode::FMATRIX_LEFT); // 使能FM内存排布模式，从left寄存器获取信息
AscendC::SetLoadDataPaddingValue(0);
AscendC::SetLoadDataRepeat({0, 1, 0});
AscendC::SetLoadDataBoundary((uint32_t)0);
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false,false};
AscendC::LoadData<fmap_T, LOAD3D_CONFIG>(featureMapA2, featureMapA1,
    { padList, H, W, channelSize, k, howoRound, 0, 0, 1, 1, Kw, Kh, dilationW, dilationH, false, false, 0 });
AscendC::LoadData(weightB2, weightB1, { 0, weRepeat, 1, 0, 0, false, 0 });
```
