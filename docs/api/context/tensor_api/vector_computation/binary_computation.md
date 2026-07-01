# 二元计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

二元计算对两个源 Tensor 的对应元素执行逐元素运算，将结果写入目的 Tensor。通过Transform接口传入二元操作类型，由编译器自动选择底层硬件指令实现。

支持的二元计算操作如下：

| 操作类型 | 功能说明 | 公式 |
| :--- | :--- | :--- |
| [`Inst::Add`](add_computation.md) | 逐元素加法 | `dst[i] = src0[i] + src1[i]` |
| [`Inst::Sub`](sub_computation.md) | 逐元素减法 | `dst[i] = src0[i] - src1[i]` |
| [`Inst::Mul`](mul_computation.md) | 逐元素乘法 | `dst[i] = src0[i] * src1[i]` |
| [`Inst::Div`](div_computation.md) | 逐元素除法 | `dst[i] = src0[i] / src1[i]` |
| [`Inst::Madd`](madd_computation.md) | 逐元素乘加 | `dst[i] = src0[i] * src1[i] + dst[i]` |
| [`Inst::Min`](min_computation.md) | 逐元素取最小值 | `dst[i] = min(src0[i], src1[i])` |
| [`Inst::Max`](max_computation.md) | 逐元素取最大值 | `dst[i] = max(src0[i], src1[i])` |
| [`Inst::AbsSub`](abs_sub_computation.md) | 逐元素绝对差值 | `dst[i] = |src0[i] - src1[i]|` |
| [`Inst::And`](and_computation.md) | 按位与 | `dst[i] = src0[i] & src1[i]` |
| [`Inst::Or`](or_computation.md) | 按位或 | `dst[i] = src0[i] \| src1[i]` |
| [`Inst::Xor`](xor_computation.md) | 按位异或 | `dst[i] = src0[i] ^ src1[i]` |
| [`Inst::Select`](select_computation.md) | 按位选择 | 根据掩码选择 src0 或 src1 的位 |
| [`Inst::ShiftLeft`](shift_left_computation.md) | 逻辑左移 | `dst[i] = src0[i] << src1[i]` |
| [`Inst::ShiftRight`](shift_right_computation.md) | 逻辑右移 | `dst[i] = src0[i] >> src1[i]` |
| [`Inst::ExpSubEven`](exp_sub_even_computation.md) | 偶数位指数减法 | `dst[2i] = e^(-src0[2i] + src1[2i])` |
| [`Inst::ExpSubOdd`](exp_sub_odd_computation.md) | 奇数位指数减法 | `dst[2i+1] = e^(-src0[2i+1] + src1[2i+1])` |
| [`Inst::Prelu`](prelu_computation.md) | PReLU 激活 | `dst[i] = src0[i] > 0 ? src0[i] : src0[i] * src1[i]` |

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor0, typename SrcTensor1>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor0& src0, const SrcTensor1& src1)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 二元计算操作类型，取值为 `Inst::Add`、`Inst::Sub`、`Inst::Mul` 等。 |
| TraitType | 计算特征参数类型，二元计算使用默认值 `Std::ignore_t`。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor0 | 第一个源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor1 | 第二个源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的张量，存储位置为 `Location::UB`。 |
| src0 | 输入 | 第一个源张量，存储位置为 `Location::UB`。 |
| src1 | 输入 | 第二个源张量，存储位置为 `Location::UB`。 |

## 数据类型

源张量和目的张量的数据类型需要保持一致。

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::Add` / `Inst::Sub` / `Inst::Mul` / `Inst::Div` / `Inst::Madd` | half, float |
| `Inst::Min` / `Inst::Max` / `Inst::AbsSub` | half, float |
| `Inst::And` / `Inst::Or` / `Inst::Xor` / `Inst::Select` | uint8_t, int8_t, uint16_t, int16_t, half, int32_t, uint32_t, float |
| `Inst::ShiftLeft` / `Inst::ShiftRight` | uint8_t, int8_t, uint16_t, int16_t, half, int32_t, uint32_t, float |
| `Inst::ExpSubEven` / `Inst::ExpSubOdd` | half, float |
| `Inst::Prelu` | half, float |

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 两个源 Tensor 和目的 Tensor 的数据类型需一致。
- 两个源 Tensor 和目的 Tensor 的数据量（由 Layout 的 `Size()` 确定）需匹配。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，末尾不足一个寄存器的部分通过 mask 机制处理。
- `Inst::Madd` 为乘加运算，结果会累加到目的 Tensor 原有值上。
- `Inst::ExpSubEven` 和 `Inst::ExpSubOdd` 分别处理偶数位和奇数位元素，通常配合使用以实现完整的指数减法运算。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void BinaryComputeExample(__gm__ half* gmX, __gm__ half* gmY, __gm__ half* gmZ)
{
    __ubuf__ half xUB[TILE_LENGTH];
    __ubuf__ half yUB[TILE_LENGTH];
    __ubuf__ half zUB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yGm = MakeTensor(MakeMemPtr<Location::GM>(gmY), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(MakeMemPtr<Location::GM>(gmZ), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto yLocal = MakeTensor(MakeMemPtr(yUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    // 搬入数据
    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);
    Copy(copyGM2UB, yLocal, yGm);

    // 执行二元计算: z = max(x, y)
    Transform<Inst::Max>(zLocal, xLocal, yLocal);

    // 搬出数据
    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```
