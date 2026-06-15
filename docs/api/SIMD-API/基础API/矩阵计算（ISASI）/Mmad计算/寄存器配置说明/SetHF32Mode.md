# SetHF32Mode

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

用于设置Mmad计算是否开启HF32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32。

## 函数原型

```cpp
__aicore__ inline void SetHF32Mode(HF32Mode mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mode | 输入 | Mmad HF32模式控制入参，HF32Mode枚举类型。支持如下两种取值：<br>ENABLE：L0A Buffer/L0B Buffer中的FP32数据将在矩阵乘法之前被舍入为HF32。DISABLE：将执行常规的FP32矩阵乘法。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
AscendC::SetHF32Mode(HF32Mode::ENABLE); // 控制mmad计算时是否使用HF32精度进行计算。
```
