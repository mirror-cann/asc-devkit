# GetRuntimeUBSize

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

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取运行时UB空间的大小，单位为Byte。开发者根据UB的大小来计算循环次数等参数值。

## 函数原型

```cpp
__aicore__ inline uint32_t GetRuntimeUBSize()
```

## 参数说明

无

## 返回值说明

运行时UB空间的大小，单位为Byte。

<cann-filter npu-type="950">Ascend 950PR/Ascend 950DT架构下，SIMD与SIMT混合场景中，获取到的UB大小上限为216KB，非SIMD与SIMT混合场景返回值为固定值248KB。</cann-filter>

## 约束说明

无

## 调用示例

本调用示例通过`GetRuntimeUBSize`获取的UB空间大小，来计算tileNum的值。完整的算子样例参考：[get\_ub\_size样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/02_features/03_basic_api/04_resource_management/get_ub_size)。

```cpp
// totalLength为待处理的总数据长度 (元素个数)
this->totalLength = totalLength;
// GetRuntimeUBSize() / sizeof(half) -> 计算UB能容纳多少个half类型元素
// 除2 -> 预留50%的UB空间
if (totalLength > AscendC::GetRuntimeUBSize() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetRuntimeUBSize() / sizeof(half) / 2;
} else { // 防止分片大小超过实际数据总量
    this->tileLength = this->totalLength;
}
// 需要迭代的分片数量
this->tileNum = this->totalLength / this->tileLength;
```
