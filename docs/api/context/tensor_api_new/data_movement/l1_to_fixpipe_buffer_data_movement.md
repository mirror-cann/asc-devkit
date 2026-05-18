# L1到Fixpipe Buffer数据搬运

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

该接口用于将 L1 Buffer 中的数据搬运到 Fixpipe Buffer。Fixpipe Buffer 可用于 L0C 输出搬运中的量化参数或中间参数准备。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyL12FB`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::FIXBUF`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::L1`。 |
| `params` | 输入 | 可变参数。当前 L1 到 Fixpipe Buffer 搬运通常不需要额外运行时参数。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyL12FB, CopyL12FBTraitDefault>>` 或 `MakeCopy(CopyL12FB{}, CopyL12FBTraitDefault{})`。 |

## 返回值说明

`Copy` 无返回值。

## 约束说明

- 目的张量必须位于 Fixpipe Buffer，源张量必须位于 L1 Buffer。
- 支持的数据类型组合为 `uint64_t` 目的和 `uint64_t` 源。
- 常用布局为 `NDExtLayoutPtn` 到 `NDExtLayoutPtn`。
- 该接口通常与 L0C 到 GM 或 L0C 到 UB 的量化输出流程配合使用。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l1Param = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<uint64_t>>(1, n));
auto fixbuf = MakeTensor(MakeMemPtr<Location::FIXBUF>(fixbufAddr),
    MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<uint64_t>>(1, n));

Copy(fixbuf, l1Param);
```
