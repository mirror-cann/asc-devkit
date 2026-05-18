# L1到UB数据搬运

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

该接口用于将 L1 Buffer 中的数据搬运到 UB。接口根据张量布局生成搬运参数，支持 ND、DN、NZ 等布局的同布局搬运。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyL12UB`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::L1`。 |
| `params` | 输入 | 可变参数。当前 L1 到 UB 搬运通常不需要额外运行时参数。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyL12UB, CopyL12UBTraitDefault>>` 或 `MakeCopy(CopyL12UB{}, CopyL12UBTraitDefault{})`。 |

## 返回值说明

`Copy` 无返回值。

## 约束说明

- 目的张量必须位于 UB，源张量必须位于 L1 Buffer。
- 支持 `NDExtLayoutPtn` 到 `NDExtLayoutPtn`、`DNExtLayoutPtn` 到 `DNExtLayoutPtn`、`NZLayoutPtn` 到 `NZLayoutPtn`。
- 源张量和目的张量的数据类型需要保持一致。
- 搬运长度和步长由布局计算得到，调用者需要保证布局描述与实际内存空间匹配。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l1 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));
auto ub = MakeTensor(MakeMemPtr<Location::UB>(ubAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<int8_t>>(m, n));

Copy(ub, l1);
```
