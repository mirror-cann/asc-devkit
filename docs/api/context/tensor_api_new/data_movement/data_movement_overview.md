# 数据搬运导览

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 头文件/库文件

头文件为：

```cpp
#include "tensor_api/tensor.h"
```

Tensor API 为头文件模板接口，不需要链接额外库文件。

## 功能说明

数据搬运接口使用统一的 `Copy` 入口完成不同存储位置之间的数据搬运。调用者需要先使用 `MakeMemPtr` 标识存储位置，再使用 `MakeFrameLayout` 或 `MakeLayout` 描述张量排布，最后使用 `MakeTensor` 构造张量。`Copy` 根据目的张量和源张量的存储位置、布局模式和数据类型在编译期选择底层搬运实现。

支持的主要搬运方向如下。

| 目的位置 | 源位置 | 操作类型 |
| :--- | :--- | :--- |
| L1 | GM | `CopyGM2L1` |
| UB | GM | `CopyGM2UB` |
| GM | UB | `CopyUB2GM` |
| L1 | UB | `CopyUB2L1` |
| UB | L1 | `CopyL12UB` |
| L0A | L1 | `CopyL12L0A` |
| L0B | L1 | `CopyL12L0B` |
| BiasTable Buffer | L1 | `CopyL12BT` |
| Fixpipe Buffer | L1 | `CopyL12FB` |
| GM | L0C | `CopyL0C2GM` |
| UB | L0C | `CopyL0C2UB` |

## 函数原型

- 直接使用张量位置进行分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 使用 `CopyAtom` 指定搬运操作。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

- 使用显式 trait 指定搬运操作。

    ```cpp
    template <typename Tp, const Tp& traits, typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

- 构造 `CopyAtom`。

    ```cpp
    template <typename... Args>
    __aicore__ inline constexpr auto MakeCopy(const Args& ...traits)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量。张量的内存位置由 `MakeMemPtr<Location::...>` 指定。 |
| `src` | 输入 | 源张量。张量的内存位置由 `MakeMemPtr<Location::...>` 指定。 |
| `params` | 输入 | 可变参数。不同搬运方向可传入坐标、量化系数、`FixpipeParams` 等。 |
| `atomCopy` | 输入 | 搬运原子对象，用于显式指定搬运操作和 trait。 |
| `traits` | 输入 | 搬运操作和搬运特征，例如 `CopyGM2L1`、`CopyL12L0A`、`CopyL0C2GM`、`CopyL0C2UB` 及其默认 trait。 |

## 返回值说明

`Copy` 无返回值。`MakeCopy` 返回 `CopyAtom<CopyTraits<Args...>>` 类型对象。

## 约束说明

- `Copy` 的目的张量和源张量必须是 Tensor API 构造的属性张量。
- 自动分发只支持文档列出的存储位置组合，不支持的组合会触发编译期报错。
- 各搬运方向的数据类型、布局模式、地址对齐和参数约束以对应子文档为准。
- 显式使用 `CopyAtom` 时，操作类型必须与目的张量、源张量的存储位置保持一致。

## 调用示例

```cpp
using namespace AscendC::Te;

auto gm = MakeTensor(MakeMemPtr<Location::GM>(gmAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));
auto l1 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

Copy(l1, gm);

auto atom = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
Copy(atom, l1, gm);
```
