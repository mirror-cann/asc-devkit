# GetLayoutPattern

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

GetLayoutPattern用于从Layout类型中提取其LayoutPattern类型信息。

## 原型定义

```cpp
template <typename T>
using GetLayoutPattern = typename GetPattern<Std::remove_cvref_t<T>>::type;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待提取LayoutPattern的Layout类型。 |

## 返回值说明

若T包含Layout布局模式信息，则返回对应的LayoutPattern类型。

若T为MakeLayout构造的Layout，GetLayoutPattern会根据Shape和Stride的类型结构自动推导可识别的LayoutPattern；无法根据类型唯一识别时返回Std::ignore_t。

## 调用示例

```cpp
using namespace AscendC::Te;

using LayoutType = decltype(MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(16, 16));
using Pattern = GetLayoutPattern<LayoutType>;
// Pattern = NZLayoutPtn

using InferredLayout = decltype(MakeLayout(MakeShape(AscendC::Te::_16{}, AscendC::Te::_16{})));
using InferredPattern = GetLayoutPattern<InferredLayout>;
// InferredPattern = NDLayoutPtn

using PlainLayout = decltype(MakeLayout(MakeShape(16, 16), MakeStride(16, 1)));
using PlainPattern = GetLayoutPattern<PlainLayout>;
// PlainPattern = Std::ignore_t，运行时数值未编码到类型中时无法自动推导
```
