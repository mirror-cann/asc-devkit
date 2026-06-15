# SetCtrlSpr(ISASI)

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品AI Core | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

对CTRL寄存器（控制寄存器）的特定比特位进行设置。

## 函数原型

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void SetCtrlSpr(int64_t value)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| startBit | 起始比特位索引。 |
| endBit | 终止比特位索引。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| value | 输入 | 起止比特位上新设置的值。 |

<cann-filter npu-type="950">

**表3** Ascend 950PR/Ascend 950DT常用CTRL寄存器比特位说明<a id="tab-950"></a>

| CTRL寄存器比特位 | 功能 | 默认值 | 配合使用的API |
| --- | --- | --- | --- |
| CTRL[8:6] | 用于控制数据从L0C Buffer/Unified Buffer/L1 Buffer搬运至Global Memory时原子操作的启用及数据类型选择。<br>&bull; 3'b000：不开启原子操作；<br>&bull; 3'b001：开启原子操作，数据类型为float；<br>&bull; 3'b010：开启原子操作，数据类型为half；<br>&bull; 3'b011：开启原子操作，数据类型为int16_t；<br>&bull; 3'b100：开启原子操作，数据类型为int32_t；<br>&bull; 3'b101：开启原子操作，数据类型为int8_t；<br>&bull; 3'b110：开启原子操作，数据类型为bfloat16_t。 | 3'b000 | 不涉及 |
| CTRL[10:9] | 用于控制原子操作的类型，仅在CTRL[8:6]开启原子操作时生效。<br>&bull; 2'b00：选择ADD操作；<br>&bull; 2'b01：选择MAX操作；<br>&bull; 2'b10：选择MIN操作。 | 2'b00 | 不涉及 |
| CTRL[45] | 用于控制左右矩阵数据做Mmad计算时的处理方式。<br>&bull; 1'b0：按照原数据类型进行处理；<br>&bull; 1'b1：左右矩阵数据均为fp8_e4m3fn_t时，数据视为hifloat8_t进行矩阵乘法计算。其他场景按照原数据类型进行处理。 | 1'b0 | 不涉及 |
| CTRL[48] | 用于控制浮点数计算和浮点数精度转换时的饱和模式，仅在CTRL[60]开启时生效。<br>&bull; 1'b0：饱和模式，inf输出会被饱和为±MAX，NaN输出会被饱和为0；<br>&bull; 1'b1：非饱和模式，inf/NaN保持原输出。<br><br>该控制位仅支持如下数据类型：<br>&bull;浮点数计算时支持half数据类型；<br>&bull;浮点数精度转换时支持如下数据类型：hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t。 | 1'b0 | 配合使用的API：<br>&bull;矢量计算API<br>&bull;原子操作API<br>&bull;精度转换指令<br><br>使用约束：<br>&bull;需要满足数据类型限制。<br>&bull;执行原子操作过程中，如果需要重新配置该控制位，需要调用[DataCacheCleanAndInvalid](../缓存控制/DataCacheCleanAndInvalid.md)先清除当前Cache Line状态并将当前数据写出，防止饱和模式变更影响当前数据。具体调用示例可参考[原子操作中，half类型配置全局非饱和模式示例。](#example2) |
| CTRL[50] | 用于控制浮点数精度转换时的NaN饱和模式，在CTRL[48]设置为饱和模式时生效。<br>&bull; 1'b0：NaN输出会被转换为0.0；<br>&bull; 1'b1：NaN输出会保持NaN。<br><br>该控制位仅支持如下数据类型：<br>fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t。 | 1'b0 | 精度转换指令（需要满足数据类型限制）。 |
| CTRL[53] | 用于控制整数计算指令的饱和模式。<br>&bull; 1'b0：截断模式，溢出值按目标数据类型位数截断，保留低位，舍弃高位；<br>&bull; 1'b1：饱和模式，溢出值饱和到±MAX。 | 1'b0 | 矢量计算API（输入输出数据类型为整数）。 |
| CTRL[59] | 用于控制浮点数转整数或整数转整数时的精度转换饱和模式，仅在CTRL[60]开启时生效。<br>&bull; 1'b0：饱和模式：溢出值饱和到±MAX；<br>&bull; 1'b1：截断模式：溢出值按目标数据类型位数截断，保留低位，舍弃高位。 | 1'b0 | 精度转换指令。 |
| CTRL[60] | 用于控制饱和模式的全局生效方式。<br>&bull; 1'b0：单指令设置饱和；<br>&bull; 1'b1：全局设置饱和。 | 1'b1 | 该控制位可与Reg矢量计算API [Cast](../Reg矢量计算/类型转换/Cast-45.md)配合使用，或与CTRL[48]、CTRL[59]配合使用，具体配置信息参考[表6](#table231122118201)。 |

</cann-filter>

<cann-filter npu-type="A3">

**表4** Atlas A3 训练系列产品/Atlas A3 推理系列产品常用CTRL寄存器比特位说明

| CTRL寄存器比特位 | 功能 | 默认值 | 配合使用的API |
| --- | --- | --- | --- |
| CTRL[48] | 用于控制浮点数计算和浮点数精度转换时的饱和模式。<br>&bull; 1'b0：饱和模式，inf输出会被饱和为±MAX， NaN输出会被饱和为0；<br>&bull; 1'b1：非饱和模式，inf/NaN保持原输出。<br><br>该控制位仅支持如下数据类型：<br>&bull;浮点数计算时支持half、bfloat16_t数据类型；<br>&bull;浮点数精度转换时支持如下数据类型：half、bfloat16_t。 | 1'b0 | 不涉及 |

**注：针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，推荐使用[SetSaturationFlag](./SetSaturationFlag(ISASI).md)和[GetSaturationFlag](./GetSaturationFlag(ISASI).md)来设置和获取CTRL[48]以控制饱和模式。**

</cann-filter>

<cann-filter npu-type="910b">

**表5** Atlas A2 训练系列产品/Atlas A2 推理系列产品常用CTRL寄存器比特位说明

| CTRL寄存器比特位 | 功能 | 默认值 | 配合使用的API |
| --- | --- | --- | --- |
| CTRL[48] | 用于控制浮点数计算和浮点数精度转换时的饱和模式。<br>&bull; 1'b0：饱和模式，inf输出会被饱和为±MAX， NaN输出会被饱和为0；<br>&bull; 1'b1：非饱和模式，inf/NaN保持原输出。<br><br>该控制位仅支持如下数据类型：<br>&bull;浮点数计算时支持half、bfloat16_t数据类型；<br>&bull;浮点数精度转换时支持如下数据类型：half、bfloat16_t。 | 1'b0 | 不涉及 |

**注：针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，推荐使用[SetSaturationFlag](./SetSaturationFlag(ISASI).md)和[GetSaturationFlag](./GetSaturationFlag(ISASI).md)来设置和获取CTRL[48]以控制饱和模式。**

</cann-filter>

<cann-filter npu-type="950">

**表6** Ascend 950PR/Ascend 950DT饱和模式全局或单指令生效配置表<a id="table231122118201"></a>

| 全局开启位 | 控制位 | 功能描述 |
| --- | --- | --- |
| CTRL[60] = 1'b0 | Reg矢量计算Cast API的trait模板参数中satMode设置为SatMode::NO_SAT。 | 单指令非饱和模式。 |
| CTRL[60] = 1'b0 | Reg矢量计算Cast API的trait模板参数中satMode设置为SatMode::SAT。 | 单指令饱和模式。 |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b1 | 全局非饱和模式（浮点数计算和浮点数精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b0 | 全局饱和模式（浮点数计算和浮点数精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b1 | 全局非饱和模式（浮点数转整数或整数转整数时的精度转换）。 |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b0 | 全局饱和模式（浮点数转整数或整数转整数时的精度转换）。 |

</cann-filter>

## 返回值说明

无

## 约束说明

- <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT仅支持CTRL[8:6]、CTRL[10:9]、CTRL[45]、CTRL[48]、CTRL[50]、CTRL[53]、CTRL[59]、CTRL[60]比特位。</cann-filter>
- <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品仅支持CTRL[48]比特位。</cann-filter>
- <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品仅支持CTRL[48]比特位。</cann-filter>

<cann-filter npu-type="950">

- Ascend 950PR/Ascend 950DT对于CTRL[8:6]和CTRL[10:9]的设置，已封装原子操作API，建议通过这些原子操作API进行配置。
    - [SetAtomicType](../原子操作/SetAtomicType.md)
    - [DisableDmaAtomic](../原子操作/DisableDmaAtomic.md)
    - [SetAtomicAdd](../原子操作/SetAtomicAdd.md)
    - [SetAtomicMax](../原子操作/SetAtomicMax(ISASI).md)
    - [SetAtomicMin](../原子操作/SetAtomicMin(ISASI).md)

</cann-filter>

## 调用示例

- 如下示例中开启非饱和模式，数据类型为half。

    ```cpp
    AscendC::SetCtrlSpr<48, 48>(1);
    ```

- <a name="example2"></a>原子操作中，half类型配置全局非饱和模式示例。

    ```cpp
    AscendC::SetCtrlSpr<6, 8>(2);
    AscendC::SetAtomicAdd<half>();
    AscendC::DataCacheCleanAndInvalid<half, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_ATOMIC>(dstTensor);
    AscendC::SetCtrlSpr<48, 48>(1);
    ...
    ```
