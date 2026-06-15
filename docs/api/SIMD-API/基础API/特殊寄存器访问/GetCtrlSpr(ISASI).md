# GetCtrlSpr(ISASI)

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

读取CTRL寄存器（控制寄存器）特定比特位上的值。

## 函数原型

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSpr()
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| startBit | 起始比特位索引。 |
| endBit | 终止比特位索引。 |

## 返回值说明

CTRL寄存器对应比特位上的值。

## 约束说明

- startBit和endBit的取值范围为[0 ,63]。
- startBit小于等于endBit。
- 各比特位的说明请参考[SetCtrlSpr(ISASI)参数说明](./SetCtrlSpr(ISASI).md#参数说明)。

## 调用示例

如下为读取CTRL[48]比特位数值的示例。

```cpp
int64_t valueCtrlAfterSet = AscendC::GetCtrlSpr<48, 48>();
```
