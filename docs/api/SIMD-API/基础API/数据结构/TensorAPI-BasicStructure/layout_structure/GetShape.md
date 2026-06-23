# GetShape

## 产品支持情况

| 产品     | 是否支持 |
| ----------- |:----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

`GetShape`用于从Layout中提取Shape部分：

- **不指定模板参数Is...**：返回完整的Shape元组。
- **指定模板参数Is...**：从Shape顶层tuple中选择对应索引的元素。指定单个索引时返回该元素；指定多个索引时返回由这些顶层元素组成的新tuple。
## 函数原型

```cpp
template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetShape(const Layout<ShapeType, StrideType>& layout)

template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetShape(Layout<ShapeType, StrideType>& layout)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 输入/输出 | 描述 |
| -------- | ----------- | ------ |
| Is... | 输入 | 可选。用于选择Shape顶层tuple中的一个或多个索引。 |
| ShapeType | 输入 | Layout的shape类型。 |
| StrideType | 输入 | Layout的stride类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| -------- | ----------- | ------ |
| layout | 输入 | Layout对象。 |

## 返回值说明

返回描述张量形状的Shape对象（或其子结构）。

## 约束说明

Is...必须在Shape顶层tuple的有效范围内。

## 调用示例

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(10, 20, 30));

// 不指定Is... → 返回完整shape元组
auto fullShape = GetShape(layout);           // (10, 20, 30)

// 指定单个索引 → 返回对应元素
auto s0 = GetShape<0>(layout);              // 10

// 指定多个索引 → 组成新tuple
auto s01 = GetShape<0, 1>(layout);          // (10, 20)
```
