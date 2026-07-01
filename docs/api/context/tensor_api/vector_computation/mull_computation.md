# Mull计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

Mull计算对两个源 Tensor 的对应元素执行长乘法（Long Multiplication），产生双输出结果并分别写入两个目的 Tensor。通过 `Transform` 接口传入 `Inst::Mull` 操作类型来实现。

计算结果为：

- `dst0[i] = src0[i] * src1[i]` 的低位结果。
- `dst1[i] = src0[i] * src1[i]` 的高位结果。

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor0, typename DstTensor1, typename SrcTensor0, typename SrcTensor1>
__aicore__ inline void Transform(const DstTensor0& dst0, const DstTensor1& dst1, const SrcTensor0& src0, const SrcTensor1& src1)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，Mull 计算取 `Inst::Mull`。 |
| TraitType | 计算特征参数类型，Mull 计算使用默认值 `Std::ignore_t`。 |
| DstTensor0 | 第一个目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| DstTensor1 | 第二个目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor0 | 第一个源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor1 | 第二个源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst0 | 输出 | 第一个目的张量，存储位置为 `Location::UB`。存放乘法结果的低位。 |
| dst1 | 输出 | 第二个目的张量，存储位置为 `Location::UB`。存放乘法结果的高位。 |
| src0 | 输入 | 第一个源张量，存储位置为 `Location::UB`。 |
| src1 | 输入 | 第二个源张量，存储位置为 `Location::UB`。 |

## 数据类型

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::Mull` | half, float |

源张量和目的张量的数据类型需要保持一致。

## 返回值说明

无返回值。

## 约束说明

- 所有源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 两个源 Tensor 和两个目的 Tensor 的数据类型需一致。
- 两个源 Tensor 和两个目的 Tensor 的数据量（由 Layout 的 `Size()` 确定）需匹配。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，末尾不足一个寄存器的部分通过 mask 机制处理。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void MullComputeExample(__gm__ half* gmX, __gm__ half* gmY,
                                           __gm__ half* gmZ0, __gm__ half* gmZ1)
{
    __ubuf__ half xUB[TILE_LENGTH];
    __ubuf__ half yUB[TILE_LENGTH];
    __ubuf__ half z0UB[TILE_LENGTH];
    __ubuf__ half z1UB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yGm = MakeTensor(MakeMemPtr<Location::GM>(gmY), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto z0Gm = MakeTensor(MakeMemPtr<Location::GM>(gmZ0), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto z1Gm = MakeTensor(MakeMemPtr<Location::GM>(gmZ1), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yLocal = MakeTensor(MakeMemPtr(yUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto z0Local = MakeTensor(MakeMemPtr(z0UB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto z1Local = MakeTensor(MakeMemPtr(z1UB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);
    Copy(copyGM2UB, yLocal, yGm);

    Transform<Inst::Mull>(z0Local, z1Local, xLocal, yLocal);

    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, z0Gm, z0Local);
    Copy(copyUB2GM, z1Gm, z1Local);
}
```
