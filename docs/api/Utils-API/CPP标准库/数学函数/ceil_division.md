# ceil_division

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x |
| Atlas 200I/500 A2 推理产品 | x |
| Atlas 推理系列产品AI Core | x |
| Atlas 推理系列产品Vector Core | x |

## 功能说明

需要包含的头文件为：#include "ceil_division.h"

ceil_division用于计算两个数相除后的向上取整结果。其计算公式为：

```cpp
(num1 + num2 - 1) / num2
```

## 函数原型

```cpp
template <typename T, typename U>
ASCENDC_HOST_AICORE inline constexpr auto ceil_division(const T& num1, const U& num2)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 被除数类型。 |
| U | 输入 | 除数类型。 |

**表2** 参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| num1 | 输入 | 被除数。支持普通整型参数，也支持Int类型的编译期常量参数。 |
| num2 | 输入 | 除数。支持普通整型参数，也支持Int类型的编译期常量参数。|

## 返回值说明

返回num1 / num2的向上取整结果，返回类型由表达式自动推导。

## 约束说明

num2不能为0。

## 调用示例

```cpp
using namespace AscendC::Std;

auto v0 = ceil_division(33, 16);
// v0 = 3

auto v1 = ceil_division(Int<33>{}, Int<16>{});
// v1的类型为Int<3>

auto v2 = ceil_division(Int<32>{}, Int<16>{});
// v2的类型为Int<2>
```
