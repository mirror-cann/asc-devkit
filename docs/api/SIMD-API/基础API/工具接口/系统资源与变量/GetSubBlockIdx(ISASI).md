# GetSubBlockIdx(ISASI)

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 AI Core | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|
| <cann-filter npu-type="x90">Kirin X90 | √ </cann-filter>|
| <cann-filter npu-type="9030">Kirin 9030 | √ </cann-filter>|

## 功能说明

头文件路径为： `"basic_api/kernel_operator_sys_var_intf.h"`。

在[分离模式架构](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/编程指南/高级编程/硬件实现/基本架构.md)下，获取逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的逻辑索引。

## 函数原型

```cpp
__aicore__ inline int64_t GetSubBlockIdx()
```

## 参数说明

无

## 返回值说明

返回逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的逻辑索引。

SubBlockIdx的取值范围为\[0, SubBlockNum\)，SubBlockNum可以通过[GetSubBlockNum](GetSubBlockNum(ISASI).md)获取。

## 约束说明

无

## 调用示例

```cpp
int64_t subBlockID = AscendC::GetSubBlockIdx();
```
