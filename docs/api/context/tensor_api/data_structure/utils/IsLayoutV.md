# IsLayoutV

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

IsLayoutV用于判断一个类型是否为[Layout](../layout_structure/Layout.md)类型。

## 原型定义

```cpp
template <typename T>
constexpr bool IsLayoutV = IsLayout<Std::remove_cvref_t<T>>::value;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待判断的类型。 |

## 返回值说明

- 若T是Layout类型，则返回true；
- 否则返回false。

## 调用示例

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(16, 16), MakeStride(16, 1));

constexpr bool isLayout = IsLayoutV<decltype(layout)>;
// isLayout = true

constexpr bool isLayoutRef = IsLayoutV<const decltype(layout)&>;
// isLayoutRef = true

constexpr bool isTuple = IsLayoutV<Std::tuple<int, int>>;
// isTuple = false
```
