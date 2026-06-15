# GetReduceRepeatSumSpr(ISASI)

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

头文件路径为：`"basic_api/kernel_operator_vec_reduce_intf.h"`。

获取[ReduceSum](../ReduceSum.md)接口的计算结果，仅支持tensor前n个数据连续计算接口使用。计算结果以全局变量形式存储，可以随时调用获取。

## 函数原型

```cpp
template <typename T>
__aicore__ inline T GetReduceRepeatSumSpr()
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

## 数据类型

操作数支持的数据类型为：`half`/`float`。

## 返回值说明

`ReduceSum`接口的计算结果。

## 约束说明

仅支持配合`ReduceSum`的tensor前n个数据连续计算接口使用。

## 调用示例

具体可参考[GetReduceRepeatSumSpr样例](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation)。

```cpp
AscendC::LocalTensor<float> src;
AscendC::LocalTensor<float> work;
AscendC::LocalTensor<float> dst;
AscendC::ReduceSum(dst, src, work, 128);
float res = AscendC::GetReduceRepeatSumSpr<float>(); // 返回前128个数的和
```
