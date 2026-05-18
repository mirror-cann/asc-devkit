# L0C到UB数据搬运

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

该接口用于将 L0C Buffer 中的矩阵计算结果搬运到 UB。接口支持不量化输出、float 到 half 或 bfloat16_t 的随路转换输出，以及配合标量或张量量化参数的输出流程。可通过 `CopyL0C2UBTrait` 配置舍入模式、Relu 激活、channel split 和双目的模式。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyL0C2UB`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::L0C`。 |
| `params` | 输入 | 可变参数。常用形式为 `FixpipeParams`，或 `quant` 与 `FixpipeParams`。`quant` 可以是 `uint64_t` 标量，也可以是 Tensor API 张量。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>` 或 `MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitDefault{})`。 |

## 返回值说明

`Copy` 无返回值。

## 约束说明

- 目的张量必须位于 UB，源张量必须位于 L0C Buffer。
- 不量化场景支持 `float` 到 `float`、`int32_t` 到 `int32_t`。
- 直接转换场景支持 `float` 到 `half` 和 `float` 到 `bfloat16_t`。
- `CopyL0C2UBTrait` 包含 `roundMode`、`enableRelu`、`enableChannelSplit`、`dualDstCtl`。默认值分别为 `RoundMode::DEFAULT`、`false`、`false`、`DUAL_DST_DISABLE`。
- 目的布局为 `NZLayoutPtn` 时，不支持 `dualDstCtl` 设置为 `DUAL_DST_SPLIT_N`。
- `FixpipeParams` 包含 `unitFlag`，默认值为 `0`。
- 支持的布局需要满足 L0C 输出布局到 UB 输出布局的检查要求，例如 NZ 到 ND、NZ 到 DN、NZ 到 NZ 等输出形态。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l0c = MakeTensor(MakeMemPtr<Location::L0C>(l0cAddr),
    MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, 16>>(m, n));
auto ub = MakeTensor(MakeMemPtr<Location::UB>(ubAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

FixpipeParams params;
Copy(ub, l0c, params);

auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitDefault{});
Copy(atom, ub, l0c, params);
```
