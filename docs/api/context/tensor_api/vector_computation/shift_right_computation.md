# ShiftRight计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

ShiftRight计算对两个源 Tensor 的对应元素执行逐元素逻辑右移，将结果写入目的 Tensor。通过 `Transform` 接口传入 `Inst::ShiftRight` 操作类型来实现。

计算公式为：`dst[i] = src0[i] >> src1[i]`。

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor0, typename SrcTensor1>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor0& src0, const SrcTensor1& src1)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，ShiftRight 计算取 `Inst::ShiftRight`。 |
| TraitType | 计算特征参数类型，ShiftRight 计算使用默认值 `Std::ignore_t`。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor0 | 被移位张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor1 | 移位位数张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| src0 | 输入 | 被移位张量，存储位置为 `Location::UB`。 |
| src1 | 输入 | 移位位数张量，存储位置为 `Location::UB`。 |

## 数据类型

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::ShiftRight` | uint8_t, int8_t, uint16_t, int16_t, half, int32_t, uint32_t, float |

源张量和目的张量的数据类型需要保持一致。

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 两个源 Tensor 和目的 Tensor 的数据类型需一致。
- 两个源 Tensor 和目的 Tensor 的数据量（由 Layout 的 `Size()` 确定）需匹配。
- 对 half、float 类型执行移位运算时，按底层数据位模式进行处理。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，末尾不足一个寄存器的部分通过 mask 机制处理。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void ShiftRightComputeExample(__gm__ uint16_t* gmX, __gm__ uint16_t* gmY, __gm__ uint16_t* gmZ)
{
    __ubuf__ uint16_t xUB[TILE_LENGTH];
    __ubuf__ uint16_t yUB[TILE_LENGTH];
    __ubuf__ uint16_t zUB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yGm = MakeTensor(MakeMemPtr<Location::GM>(gmY), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(MakeMemPtr<Location::GM>(gmZ), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yLocal = MakeTensor(MakeMemPtr(yUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);
    Copy(copyGM2UB, yLocal, yGm);

    Transform<Inst::ShiftRight>(zLocal, xLocal, yLocal);

    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```
