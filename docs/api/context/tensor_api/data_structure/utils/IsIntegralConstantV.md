# IsIntegralConstantV

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

IsIntegralConstantV用于判断一个类型是否为Std::Int<Value>形式的整型编译期常量类型。

## 原型定义

```cpp
template <typename T>
constexpr bool IsIntegralConstantV = IsIntegralConstant<T>::value;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待判断的类型。 |

## 返回值说明

若T是Std::Int<Value>类型，则返回true；否则返回`false`。

## 调用示例

```cpp
using namespace AscendC::Te;

constexpr bool isInt16 = IsIntegralConstantV<Std::Int<16>>;
// isInt16 = true

constexpr bool isAlias16 = IsIntegralConstantV<_16>;
// isAlias16 = true

constexpr bool isSizeT = IsIntegralConstantV<size_t>;
// isSizeT = false
```
