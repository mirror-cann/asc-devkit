# SetHF32TransMode

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √ </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | √ </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |

## 功能说明

设置HF32模式取整的具体方式，需要先使用[SetHF32Mode](SetHF32Mode.md)开启HF32取整模式。

## 函数原型

```cpp
__aicore__ inline void SetHF32TransMode(HF32TransMode mode)
```

## 参数说明

**表 1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mode | 输入 | Mmad HF32取整模式控制入参，HF32TransMode类型。支持如下两种取值：<br>NEAREST_ZERO：则FP32将以向零靠近的方式四舍五入为HF32。NEAREST_EVEN：则FP32将以最接近偶数的方式四舍五入为HF32。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

设置HF32模式取整的具体方式，需要先使用[SetHF32Mode](SetHF32Mode.md)开启HF32取整模式。

```cpp
AscendC::SetHF32TransMode(HF32TransMode::NEAREST_ZERO);
```
