# SetLoadDataPaddingValue<a name="ZH-CN_TOPIC_0000001834740625"></a>

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

用于调用[Load3D](../矩阵数据搬入至L0-Buffer/Load3D.md)接口时设置Pad填充的数值。Load3D的模板参数isSetPadding设置为true时，用户需要通过本接口设置Pad填充的数值，设置为false时，本接口设置的填充值不生效。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetLoadDataPaddingValue(const T padValue)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| padValue | 输入 | 边界值。<br>Pad填充值的数值。 |

## 数据类型

<cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。</cann-filter><br>
<cann-filter npu-type = "310p">Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half。</cann-filter><br>
<cann-filter npu-type = "910b">Atlas A2训练系列产品/Atlas A2推理系列产品，支持数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。</cann-filter><br>
<cann-filter npu-type = "A3">Atlas A3训练系列产品/Atlas A3推理系列产品，支持数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。</cann-filter><br>
<cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品，支持数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。</cann-filter><br>
<cann-filter npu-type = "x90">Kirin X90，支持数据类型：int8_t、half。</cann-filter><br>
<cann-filter npu-type = "9030">Kirin 9030，支持数据类型：half。</cann-filter>

## 返回值说明

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

参考[调用示例](SetFmatrix.md#section642mcpsimp)

展示代码示例片段：

```cpp
// Load3Dv2指令完成img2col的过程，可知img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ci * kh * kw,代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。最后，配置loadDataParams.enTranspose = true，将整个A矩阵转置并且将其中每一个分形转置
// 使用load3d接口，实现NZ2ZZ
AscendC::LoadData3DParamsV2<T> loadDataParams;
// 设置loadDataParams相关参数
...
// 设置不在Load3D接口内部设置相关属性，通过外部接口显式调用设置
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};

// 使用SetLoadDataRepeat接口，设置Load3Dv2接口的repeat参数
AscendC::SetLoadDataPaddingValue(0);

AscendC::LoadData<T, LOAD3D_CONFIG>(a2Local, a1Local, loadDataParams);
```