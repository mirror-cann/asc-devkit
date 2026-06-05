# ResetCtrlSpr(ISASI)

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 AI Core | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

重置CTRL寄存器（控制寄存器）的特定比特位。

## 函数原型

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSpr()
```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| startBit | 起始比特位索引。 |
| endBit | 终止比特位索引。 |

## 返回值说明

无

## 约束说明

仅支持CTRL[8:6]、CTRL[10:9]、CTRL[48]、CTRL[50]、CTRL[53]、CTRL[59]、CTRL[60]比特位，各比特位的说明请参考[SetCtrlSpr(ISASI)参数说明](./SetCtrlSpr(ISASI).md#参数说明)。

## 调用示例

如下示例中重置CTRL[48]比特位，使能饱和模式。

```cpp
AscendC::SetCtrlSpr<48, 48>(1);
...
AscendC::ResetCtrlSpr<48, 48>();
```
