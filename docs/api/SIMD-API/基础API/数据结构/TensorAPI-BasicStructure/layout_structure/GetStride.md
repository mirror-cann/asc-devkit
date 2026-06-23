# GetStride

## 产品支持情况

| 产品     | 是否支持 |
| ----------- |:----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

GetStride用于从Layout中提取Stride部分：

- **不指定模板参数Is...**：返回完整的Stride元组。
- **指定模板参数Is...**：从Stride顶层tuple中选择对应索引的元素。指定单个索引时返回该元素；指定多个索引时返回由这些顶层元素组成的新tuple。

## 函数原型

```cpp
template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetStride(const Layout<ShapeType, StrideType>& layout)

template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetStride(Layout<ShapeType, StrideType>& layout)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 输入/输出 | 描述 |
| -------- | ----------- | ------ |
| Is... | 输入 | 可选。用于选择Stride顶层tuple中的一个或多个索引。 |
| ShapeType | 输入 | Layout的shape类型。 |
| StrideType | 输入 | Layout的stride类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| -------- | ----------- | ------ |
| layout | 输入 | Layout对象。 |

## 返回值说明

返回描述内存访问步长的Stride对象（或其子结构）。

## 约束说明

Is...必须在Stride顶层tuple的有效范围内。

## 调用示例

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(10, 20, 30), MakeStride(1, 10, 200));

// 不指定Is... → 返回完整stride元组
auto fullStride = GetStride(layout);         // (1, 10, 200)

// 指定单个索引 → 返回对应元素
auto s0 = GetStride<0>(layout);             // 1

// 指定多个索引 → 组成新tuple
auto s01 = GetStride<0, 1>(layout);         // (1, 10)
```
