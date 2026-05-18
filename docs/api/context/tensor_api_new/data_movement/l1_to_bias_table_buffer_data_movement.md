# L1到BiasTable Buffer数据搬运

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

该接口用于将 L1 Buffer 中的数据搬运到 BiasTable Buffer。BiasTable Buffer 可作为矩阵计算中 bias 输入的物理位置，用于对 L0C 的初始矩阵或偏置项进行处理。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyL12BT`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::BIAS`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::L1`。 |
| `params` | 输入 | 可变参数。当前 L1 到 BiasTable Buffer 搬运通常不需要额外运行时参数。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyL12BT, CopyL12BTTraitDefault>>` 或 `MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{})`。 |

## 返回值说明

`Copy` 无返回值。

## 约束说明

- 目的张量必须位于 BiasTable Buffer，源张量必须位于 L1 Buffer。
- 支持的数据类型组合包括 `float` 目的和 `bfloat16_t` 源、`float` 目的和 `half` 源、`float` 目的和 `float` 源、`int32_t` 目的和 `int32_t` 源。
- 常用布局为 `NDExtLayoutPtn` 到 `NDExtLayoutPtn`。
- 作为 `Mmad` bias 输入时，bias 张量维度需要与矩阵计算的 N 方向匹配。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l1Bias = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(1, n));
auto bias = MakeTensor(MakeMemPtr<Location::BIAS>(biasAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(1, n));

Copy(bias, l1Bias);
```
