# 一元计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

一元计算对单个源 Tensor 的每个元素执行逐元素运算，将结果写入目的 Tensor。通过 `Transform` 接口传入一元操作类型，由编译器自动选择底层硬件指令实现。

支持的一元计算操作如下：

| 操作类型 | 功能说明 | 公式 |
| :--- | :--- | :--- |
| `Inst::Abs` | 绝对值 | `dst[i] = |src[i]|` |
| `Inst::Exp` | 指数运算 | `dst[i] = e^(src[i])` |
| `Inst::Ln` | 自然对数 | `dst[i] = ln(src[i])` |
| `Inst::Sqrt` | 平方根 | `dst[i] = √(src[i])` |
| `Inst::Not` | 按位取反 | `dst[i] = ~src[i]` |

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor& src)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 一元计算操作类型，取值为 `Inst::Abs`、`Inst::Exp`、`Inst::Ln`、`Inst::Sqrt`、`Inst::Not`。 |
| TraitType | 计算特征参数类型，一元计算使用默认值 `Std::ignore_t`。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor | 源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| src | 输入 | 源张量，存储位置为 `Location::UB`。 |

## 数据类型

源张量和目的张量的数据类型需要保持一致。

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::Abs` | int8_t, int16_t, half, float, int32_t |
| `Inst::Exp` | half, float |
| `Inst::Ln` | half, float |
| `Inst::Sqrt` | half, float |
| `Inst::Not` | uint8_t, int8_t, uint16_t, int16_t, half, int32_t, uint32_t, float |

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

__aicore__ inline void UnaryComputeExample(__gm__ half* gmX, __gm__ half* gmZ)
{
    __ubuf__ half xUB[TILE_LENGTH];
    __ubuf__ half zUB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(MakeMemPtr<Location::GM>(gmZ), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    // 搬入数据
    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);

    // 执行一元计算: z = sqrt(x)
    Transform<Inst::Sqrt>(zLocal, xLocal);

    // 搬出数据
    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```