# MakeLayout

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | 支持 |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

MakeLayout用于根据Shape和Stride构造Layout对象，描述数据张量在逻辑维度上的形状以及各维对应的步长信息。

MakeLayout支持以下两种构造方式：

- 传入shape和stride，直接按用户指定的布局信息构造Layout。
- 仅传入shape，接口会根据shape自动推导stride并构造紧凑布局。当shape为一维或多维普通元组时，按连续行优先存储方式推导步长。

## 函数原型

```cpp
template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride)

template <typename ShapeType>
__aicore__ inline constexpr auto MakeLayout(const ShapeType& shape)
```

## 参数说明

**表1** 模板参数说明

| 参数名称 | 类型 | 描述 |
| :------- | :--- | :--- |
| T | 输入 | shape的类型，要求为元组（tuple）类型。 |
| U | 输入 | stride的类型，要求为元组（tuple）类型。 |
| ShapeType | 输入 | 单参数重载中shape的类型，要求为元组（tuple）类型。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 描述 |
| :------- | :-------- | :--- |
| shape | 输入 | 描述逻辑形状的元组对象，可由[MakeShape](./MakeShape.md)构造。 |
| stride | 输入 | 描述各维步长的元组对象，可由[MakeStride](./MakeStride.md)构造。 |

## 返回值说明

- 返回Layout<Shape, Stride>类型对象。

## 约束说明

构造的Layout类型不含有LayoutPattern和LayoutTrait信息，需要通过[MakePatternLayout](../layout_structure/MakePatternLayout.md)造带有完整类型信息的Layout对象。

## 调用示例

```cpp
using namespace AscendC::Te;

// 示例1：显式指定Shape和Stride
auto layout1 = MakeLayout(MakeShape(8, 16), MakeStride(16, 1));

// 示例2：仅指定Shape，自动推导连续布局的Stride
auto layout2 = MakeLayout(MakeShape(8, 16, 32));
// 推导得到 stride = (16 * 32, 32, 1) = (512, 32, 1)

// 示例3：嵌套Shape自动推导嵌套Stride
auto layout3 = MakeLayout(MakeShape(MakeShape(2, 4), MakeShape(8, 16)));
// 推导得到stride为嵌套结构，用于描述FrameLayout
```
