# SetMMColumnMajor/SetMMRowMajor

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

控制Mmad/MmadWithSparse优先通过M/N的哪个方向。

## 函数原型

```cpp
__aicore__ inline void SetMMColumnMajor() // 代表CUBE将首先通过M方向，然后通过N方向生成结果
__aicore__ inline void SetMMRowMajor()   // 代表CUBE将首先通过N方向，然后通过M方向产生结果
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 调用示例

```cpp
AscendC::SetMMRowMajor();
```
