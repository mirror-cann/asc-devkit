# MX矩阵计算

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

MX 矩阵计算是 Tensor API 中通过 `MmadTrait::mmadType` 选择的矩阵计算模式，用于低精度 MX 数据类型的矩阵乘加。该模式仍以 L0A 为左矩阵、L0B 为右矩阵、L0C 为结果矩阵，调用入口与普通 `Mmad` 一致。

## 函数原型

- 自动使用默认矩阵计算原子对象。

    ```cpp
    template <typename T, typename U, typename S,
        Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U> && IsAttrTensorV<S>, int>, typename... Params>
    __aicore__ inline void
    Mmad(const T& dst, const U& fm, const S& filter, const Params& ...params)
    ```

- 使用 `MmadAtom` 指定 MX trait。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Mmad(const MmadAtom<T>& atomMmad, const Params& ...params)
    ```

- 构造 `MmadAtom`。

    ```cpp
    template <typename... Args>
    __aicore__ inline constexpr auto MakeMmad(const Args& ...traits)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 结果矩阵张量，存储位置为 `Location::L0C`，数据类型通常为 `float`。 |
| `fm` | 输入 | 左矩阵张量，存储位置为 `Location::L0A`，数据类型为 MX 支持的低精度类型。 |
| `filter` | 输入 | 右矩阵张量，存储位置为 `Location::L0B`，数据类型为 MX 支持的低精度类型。 |
| `params` | 输入 | 可变参数。普通 MX 场景传入 `MmadParams`，带 bias 场景传入 bias 张量和 `MmadParams`。 |
| `atomMmad` | 输入 | 使用 `MmadTrait` 指定 `MmadType::MX` 的矩阵计算原子对象。 |
| `traits` | 输入 | `MakeMmad` 的构造参数。需要通过 `.with(MmadTrait{...})` 将 `mmadType` 设置为 `MmadType::MX`。 |

## 返回值说明

`Mmad` 无返回值。`MakeMmad` 返回 `MmadAtom` 对象。

## 约束说明

- 目的张量必须位于 L0C，左矩阵必须位于 L0A，右矩阵必须位于 L0B。
- 支持的数据类型组合包括 `float = fp4x2_e2m1_t * fp4x2_e2m1_t`、`float = fp4x2_e2m1_t * fp4x2_e1m2_t`、`float = fp4x2_e1m2_t * fp4x2_e2m1_t`、`float = fp4x2_e1m2_t * fp4x2_e1m2_t`、`float = fp8_e4m3fn_t * fp8_e4m3fn_t`、`float = fp8_e4m3fn_t * fp8_e5m2_t`、`float = fp8_e5m2_t * fp8_e4m3fn_t`、`float = fp8_e5m2_t * fp8_e5m2_t`。
- 带 bias 场景中，bias 位于 BiasTable Buffer 或 L0C 时，bias 数据类型需要为 `float`。
- MX 计算通常需要配合 ScaleA 或 ScaleB 数据搬运流程准备 scale 数据，ScaleA 搬运可参考 [L1到L0A数据搬运](../data_movement/l1_to_l0a_data_movement.md)，ScaleB 搬运可参考 [L1到L0B数据搬运](../data_movement/l1_to_l0b_data_movement.md)。
- `m`、`n`、`k` 的取值和张量布局需要满足底层 MX 矩阵计算指令约束。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l0A = MakeTensor(MakeMemPtr<Location::L0A>(aAddr),
    MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<fp8_e4m3fn_t>>(m, k));
auto l0B = MakeTensor(MakeMemPtr<Location::L0B>(bAddr),
    MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<fp8_e5m2_t>>(k, n));
auto l0C = MakeTensor(MakeMemPtr<Location::L0C>(cAddr),
    MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, 16>>(m, n));

MmadParams params(m, n, k, 0, true);
MmadTrait trait(0, false, false, true, MmadType::MX);
auto atom = MakeMmad(MmadOperation{}, MmadTraitDefault{}).with(trait);

Mmad(atom, l0C, l0A, l0B, params);
```
