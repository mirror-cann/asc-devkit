# IsAttrTensorV

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

IsAttrTensorV用于判断一个类型是否为带Engine, Layout属性的张量[Tensor](../tensor_structure/Tensor.md)类型。

## 函数原型

```cpp
template <typename T>
constexpr bool IsAttrTensorV = IsAttrTensor<Std::remove_cvref_t<T>>::value;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待判断的类型。 |

## 返回值说明

若T是张量Tensor类型，则返回true；否则返回false。

## 调用示例

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(16, 16), MakeStride(16, 1));
auto tensor = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), layout);

constexpr bool isTensor = IsAttrTensorV<decltype(tensor)>;
// isTensor = true

constexpr bool isPlainInt = IsAttrTensorV<int>;
// isPlainInt = false
```
