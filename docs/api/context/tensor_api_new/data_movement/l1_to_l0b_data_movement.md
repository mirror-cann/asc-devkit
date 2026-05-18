# L1到L0B数据搬运

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

该接口用于将 L1 Buffer 中的矩阵分形数据搬运到 L0B Buffer，作为矩阵计算的右矩阵输入。接口支持普通搬运、转置搬运、b8/b4 转置搬运以及 MX ScaleB 搬运。传入坐标时，可以从源张量指定坐标处搬运一个子块。

## 函数原型

- 自动根据张量位置分发。

    ```cpp
    template <typename T, typename U, Std::enable_if_t<IsAttrTensorV<T> && IsAttrTensorV<U>, int>,
        typename... Params>
    __aicore__ inline void
    Copy(const T& dst, const U& src, const Params& ...params)
    ```

- 显式指定 `CopyL12L0B`。

    ```cpp
    template <typename T, typename... Params>
    __aicore__ inline void Copy(const CopyAtom<T>& atomCopy, const Params& ...params)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `dst` | 输出 | 目的张量，存储位置为 `Location::L0B`。 |
| `src` | 输入 | 源张量，存储位置为 `Location::L1`。 |
| `params` | 输入 | 可变参数。可传入 `Coord` 坐标，指定从源张量某个分形坐标开始搬运。 |
| `atomCopy` | 输入 | 搬运原子对象，显式调用时使用 `CopyAtom<CopyTraits<CopyL12L0B, CopyL12L0BTraitDefault>>` 或 `MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{})`。 |

## 返回值说明

`Copy` 无返回值。

## 约束说明

- 目的张量必须位于 L0B Buffer，源张量必须位于 L1 Buffer。
- 普通搬运支持 `ZNLayoutPtn` 到 `ZNLayoutPtn`。
- 转置搬运支持 `NZLayoutPtn` 到 `ZNLayoutPtn`，b8/b4 数据类型会选择 b8/b4 专用转置路径。
- MX ScaleB 搬运支持 `NNLayoutPtn` 到 `NNLayoutPtn`。
- 普通数据类型支持 `half`、`int16_t`、`uint16_t`、`bfloat16_t`、`uint32_t`、`int32_t`、`float`、`uint8_t`、`int8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`。
- ScaleB 数据类型支持 `fp8_e8m0_t`。
- L0B 通常作为 `Mmad` 的右矩阵输入，后续矩阵计算要求 L0B 地址和分形排布满足硬件矩阵计算约束。

## 调用示例

```cpp
using namespace AscendC::Te;

auto l1B = MakeTensor(MakeMemPtr<Location::L1>(l1Addr),
    MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<half>>(k, n));
auto l0B = MakeTensor(MakeMemPtr<Location::L0B>(l0bAddr),
    MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<half>>(k, n));

Copy(l0B, l1B);
Copy(l0B, l1B, MakeCoord(0, 0));
```
