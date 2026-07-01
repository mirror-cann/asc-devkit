# MinScalar计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

MinScalar计算对源 Tensor 的每个元素与标量值执行取最小值运算，将结果写入目的 Tensor。通过 `Transform` 接口传入 `Inst::MinScalar` 操作类型来实现。

计算公式为：`dst[i] = min(src[i], value)`。

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor, typename ScalarType>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor& src, const ScalarType& value)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，MinScalar 计算取 `Inst::MinScalar`。 |
| TraitType | 计算特征参数类型，MinScalar 计算使用默认值 `Std::ignore_t`。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor | 源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| ScalarType | 标量值类型。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| src | 输入 | 源张量，存储位置为 `Location::UB`。 |
| value | 输入 | 用于比较的标量值。 |

## 数据类型

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::MinScalar` | half, float |

源张量和目的张量的数据类型需要保持一致。标量值类型可以与源/目的张量的数据类型不同，支持混合数据类型计算。

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 源 Tensor 和目的 Tensor 的数据类型需一致。
- 源 Tensor 和目的 Tensor 的数据量（由 Layout 的 `Size()` 确定）需匹配。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，末尾不足一个寄存器的部分通过 mask 机制处理。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void MinScalarComputeExample(__gm__ half* gmX, __gm__ half* gmZ)
{
    __ubuf__ half xUB[TILE_LENGTH];
    __ubuf__ half zUB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(MakeMemPtr<Location::GM>(gmZ), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);

    Transform<Inst::MinScalar>(zLocal, xLocal, half(2.0));

    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```
