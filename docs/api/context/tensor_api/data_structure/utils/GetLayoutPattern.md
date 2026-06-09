# GetLayoutPattern

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

GetLayoutPattern用于从Layout类型中提取其LayoutPattern类型信息。

## 函数原型

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

若T包含Layout布局模式信息，则返回对应的LayoutPattern类型；否则返回Std::ignore_t。

## 调用示例

```cpp
using namespace AscendC::Te;

using LayoutType = decltype(MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(16, 16));
using Pattern = GetLayoutPattern<LayoutType>;
// Pattern = NZLayoutPtn

using PlainLayout = decltype(MakeLayout(MakeShape(16, 16), MakeStride(16, 1)));
using PlainPattern = GetLayoutPattern<PlainLayout>;
// PlainPattern = Std::ignore_t
```
