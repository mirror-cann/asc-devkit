# 矩阵计算流程

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 头文件/库文件

头文件为：

```cpp
#include "tensor_api/tensor.h"
```

## 功能说明

矩阵计算流程由数据搬运和矩阵乘加两部分组成。通常先将 Global Memory 中的输入矩阵搬运到 L1，再将左矩阵搬运到 L0A、右矩阵搬运到 L0B，然后调用 `Mmad` 将结果写入 L0C，最后将 L0C 结果搬运到 GM 或 UB。

典型流程如下。

1. 使用 `MakeMemPtr` 标识 GM、L1、L0A、L0B、L0C 等存储位置。
2. 使用 `MakeFrameLayout` 构造 ND、NZ、ZN 等布局。
3. 使用 `MakeTensor` 构造张量。
4. 使用 `Copy` 完成 GM 到 L1、L1 到 L0A、L1 到 L0B 的数据搬运。
5. 使用 `Mmad` 完成矩阵乘加计算。
6. 使用 `Copy` 完成 L0C 到 GM 或 L0C 到 UB 的结果搬运。

## 函数原型

```cpp
template <typename T, typename U, typename S,
    Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U> && IsAttrTensorV<S>, int>, typename... Params>
__aicore__ inline void
Mmad(const T& dst, const U& fm, const S& filter, const Params& ...params)
```

```cpp
template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
    typename... Params>
__aicore__ inline void
Copy(const T& dst, const U& src, const Params& ...params)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | `Mmad` 的目的张量位于 L0C。`Copy` 的目的张量由搬运方向决定。 |
| `fm` | 输入 | 左矩阵张量，存储位置为 L0A。 |
| `filter` | 输入 | 右矩阵张量，存储位置为 L0B。 |
| `src` | 输入 | `Copy` 的源张量。 |
| `params` | 输入 | `MmadParams`、`FixpipeParams`、坐标、量化参数等可变参数。 |

## 返回值说明

`Copy` 和 `Mmad` 无返回值。

## 约束说明

- 矩阵计算的左矩阵必须位于 L0A，右矩阵必须位于 L0B，结果矩阵必须位于 L0C。
- 普通矩阵计算中，左矩阵常用 `NZLayoutPtn`，右矩阵常用 `ZNLayoutPtn`，结果矩阵常用 `NZLayoutPtn`。
- `MmadParams` 中的 `m`、`n`、`k` 需要与参与计算的矩阵尺寸一致。
- 如果需要 bias 输入，bias 张量需要位于 BiasTable Buffer 或 L0C。
- 计算结果搬出时，应根据目标数据类型选择 L0C 到 GM 或 L0C 到 UB 搬运，并传入符合要求的 `FixpipeParams`。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l0A = MakeTensor(MakeMemPtr<Location::L0A>(aAddr),
    MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<half>>(m, k));
auto l0B = MakeTensor(MakeMemPtr<Location::L0B>(bAddr),
    MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<half>>(k, n));
auto l0C = MakeTensor(MakeMemPtr<Location::L0C>(cAddr),
    MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, 16>>(m, n));

MmadParams params(m, n, k, 0, true);
Mmad(l0C, l0A, l0B, params);
```
