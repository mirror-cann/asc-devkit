# tensor_api_list

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

本文件列出 Tensor API 的公开入口。接口位于 `AscendC::Te` 命名空间，典型调用顺序为构造指针、构造布局、构造张量、调用数据搬运或矩阵计算接口。

## 函数原型

```cpp
template <typename... Ts>
__aicore__ inline constexpr Shape<Ts...> MakeShape(const Ts&... t)

template <typename... Ts>
__aicore__ inline constexpr Stride<Ts...> MakeStride(const Ts&... t)

template <typename... Ts>
__aicore__ inline constexpr Tile<Ts...>  MakeTile(const Ts&... t)

template <typename... Ts>
__aicore__ inline constexpr Coord<Ts...> MakeCoord(const Ts&... t)

template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride)

template <typename T>
__aicore__ inline constexpr auto MakeLayout(const T& shape)

template <typename T, typename Shape, typename Stride> 
__aicore__ inline constexpr auto Crd2Idx(const T& coord, const Shape& shape, const Stride& stride)

template <size_t I, typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t)

template <size_t I0, size_t I1, size_t... Is, typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t)

template <typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Cosize(const LayoutType& layout)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Rank(const LayoutType& layout)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Select(const LayoutType& layout)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Get(const LayoutType& layout)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Size(const LayoutType& layout)

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Capacity(const LayoutType& layout)

template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const Coord& coord, const Info& info)

template <typename LayoutPattern, typename TraitType,
    Std::enable_if_t<IsFrameLayoutTraitV<TraitType>, int> Enable, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)

template <typename LayoutPattern, typename IntTypeOrDataType,
    Std::enable_if_t<!IsFrameLayoutTraitV<IntTypeOrDataType>, int> Enable, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)

template <typename Hardware, typename TraitOrType, typename Arg0, typename... Args,
    EnableMakePtrByTrait<Hardware, Arg0> Enable>
__aicore__ inline constexpr auto MakeMemPtr(Arg0 arg0, Args... args)

template <typename Hardware, typename Arg0, typename... Args,
    EnableMakeHardwarePtr<Hardware, Arg0> Enable>
__aicore__ inline constexpr auto MakeMemPtr(Arg0 arg0, Args... args)

template <typename Iterator, EnableMakePtrByIter<Iterator> Enable>
__aicore__ inline constexpr auto MakeMemPtr(Iterator& iter)

template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto MakeTensor(const Iterator& iter, const Args&... args)

template <typename Tp, const Tp& traits, typename T, typename... Params>
__aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)

template <typename T, typename... Params>
__aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)

template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
    typename... Params>
__aicore__ inline void
Copy(const T& dst, const U& src, const Params& ...params)

template <typename... Args>
__aicore__ inline constexpr auto MakeCopy(const Args& ...traits)

template <typename Tp, const Tp& traits, typename T, typename... Params>
__aicore__ inline void Mmad(const MmadAtom<T>& atomMmad, const Params& ...params)

template <typename T, typename... Params>
__aicore__ inline void Mmad(const MmadAtom<T>& atomMmad, const Params& ...params)

template <typename T, typename U, typename S,
    Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U> && IsAttrTensorV<S>, int>, typename... Params>
__aicore__ inline void
Mmad(const T& dst, const U& fm, const S& filter, const Params& ...params)

template <typename... Args>
__aicore__ inline constexpr auto MakeMmad(const Args& ...traits)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `shape` | 输入 | 张量各维度大小，可由 `MakeShape` 构造。 |
| `stride` | 输入 | 张量各维度步长，可由 `MakeStride` 构造。 |
| `coord` | 输入 | 张量坐标，可由 `MakeCoord` 构造。 |
| `layout` | 输入 | 张量布局对象，包含形状、步长、排布模式和布局属性。 |
| `iter` | 输入 | 由 `MakeMemPtr` 构造的内存指针适配器。 |
| `dst` | 输出 | 目的张量。 |
| `src` | 输入 | 源张量。 |
| `atomCopy` | 输入 | 数据搬运原子对象，可由 `MakeCopy` 或 `CopyAtom` 构造。 |
| `atomMmad` | 输入 | 矩阵计算原子对象，可由 `MakeMmad` 或 `MmadAtom` 构造。 |
| `fm` | 输入 | 矩阵计算左矩阵张量。 |
| `filter` | 输入 | 矩阵计算右矩阵张量。 |
| `params` | 输入 | 可变参数，用于传入坐标、量化系数、`FixpipeParams`、`MmadParams` 等。 |
| `traits` | 输入 | 编译期特征参数，用于选择操作类型、计算模式、转置、量化、激活等行为。 |

## 返回值说明

`MakeShape`、`MakeStride`、`MakeTile`、`MakeCoord`、`MakeLayout`、`Select`、`Get`、`Slice`、`MakeFrameLayout`、`MakeMemPtr`、`MakeTensor`、`MakeCopy` 和 `MakeMmad` 返回对应构造对象。`Copy` 和 `Mmad` 无返回值。

## 约束说明

- Tensor API 的内部实现头文件位于 `impl/tensor_api`，用户代码应包含 `tensor_api/tensor.h`，不要直接包含内部实现头文件。
- `Copy` 会根据目的张量和源张量的存储位置自动选择搬运实现。不支持的存储位置组合会触发编译期报错。
- `Mmad` 要求目的张量位于 L0C，左矩阵位于 L0A，右矩阵位于 L0B，带偏置场景的偏置张量位于 BiasTable Buffer 或 L0C。
- 张量的数据类型、布局模式和物理地址对齐要求需要同时满足具体数据搬运或矩阵计算接口的约束。
