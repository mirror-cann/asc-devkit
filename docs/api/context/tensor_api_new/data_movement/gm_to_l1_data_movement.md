# GM到L1数据搬运

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

该接口用于将 Global Memory 中的数据搬运到 L1 Buffer。接口通过张量的存储位置和布局模式选择连续搬运、ND 到 NZ 转换、DN 到 NZ 转换、NZ 到 NZ 搬运、ZN 到 ZN 搬运以及 MX Scale 布局搬运等实现。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyGM2L1`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

- 构造 `CopyGM2L1` 原子对象。

    ```cpp
    template <typename... Args>
    __aicore__ inline constexpr auto MakeCopy(const Args& ...traits)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::L1`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::GM`。 |
| `params` | 输入 | 可变参数。当前 GM 到 L1 搬运通常不需要额外运行时参数。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyGM2L1, CopyGM2L1TraitDefault>>` 或 `MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{})`。 |
| `traits` | 输入 | `MakeCopy` 的构造参数，GM 到 L1 搬运使用 `CopyGM2L1` 和 `CopyGM2L1TraitDefault`。 |

## 返回值说明

`Copy` 无返回值。`MakeCopy` 返回 `CopyAtom` 对象。

## 约束说明

- 目的张量必须位于 L1 Buffer，源张量必须位于 Global Memory。
- 支持的布局组合包括 `NDExtLayoutPtn` 到 `NDExtLayoutPtn`、`NDExtLayoutPtn` 到 `NZLayoutPtn`、`NDExtLayoutPtn` 到 `ZNLayoutPtn`、`DNExtLayoutPtn` 到 `NZLayoutPtn`、`DNExtLayoutPtn` 到 `ZNLayoutPtn`、`NZLayoutPtn` 到 `NZLayoutPtn`、`ZNLayoutPtn` 到 `ZNLayoutPtn`、`ScaleANDLayoutPtn` 到 `ZZLayoutPtn`、`ScaleADNLayoutPtn` 到 `ZZLayoutPtn`、`ZZLayoutPtn` 到 `ZZLayoutPtn`、`ScaleBNDLayoutPtn` 到 `NNLayoutPtn`、`ScaleBDNLayoutPtn` 到 `NNLayoutPtn`、`NNLayoutPtn` 到 `NNLayoutPtn`。
- 普通数据类型支持 `half`、`bfloat16_t`、`float`、`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`hifloat8_t`。部分 ND 对齐搬运还支持 `int64_t`、`uint64_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`。
- 源张量和目的张量的数据类型需要保持一致。
- 对齐要求由所选布局和底层搬运指令决定，使用时应保证 GM 地址和 L1 地址满足硬件搬运对齐要求。

## 调用示例

```cpp
using namespace AscendC::Te;

auto gmA = MakeTensor(MakeMemPtr<Location::GM>(gmAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));
auto l1A = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

Copy(l1A, gmA);

auto atom = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
Copy(atom, l1A, gmA);
```
