# GetLayoutTrait

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

GetLayoutTrait用于从Layout类型中提取其LayoutTrait类型信息。

## 原型定义

```cpp
template <typename T>
using GetLayoutTrait = typename GetTraitType<Std::remove_cvref_t<T>>::type;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待提取LayoutTrait的Layout类型。 |

## 返回值说明

若T包含Layout布局特征信息，则返回对应的TraitType；否则返回Std::ignore_t。

## 调用示例

```cpp
using namespace AscendC::Te;

using LayoutType = decltype(MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(16, 16));
using Trait = GetLayoutTrait<LayoutType>;
// Trait = LayoutTraitDefault<float>

using PlainLayout = decltype(MakeLayout(MakeShape(16, 16), MakeStride(16, 1)));
using PlainTrait = GetLayoutTrait<PlainLayout>;
// PlainTrait = Std::ignore_t
```
