# 类型转换

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

类型转换将源 Tensor 的数据类型转换为目的数据类型，支持多种数据类型间的转换。通过 `Transform` 接口传入 `Inst::Cast` 操作类型和 Cast Trait 特征参数，由编译器根据源/目的数据类型和特征参数自动选择底层硬件指令实现。

Cast Trait 用于指定类型转换的附加属性，包括舍入模式（RoundMode）、饱和模式（SatMode）和索引位置（IndexPos）。

### 舍入模式（RoundMode）

| 舍入模式 | 说明 |
| :--- | :--- |
| `CastRoundMode::RD` | floor模式，向下取整） |
| `CastRoundMode::RN` | 返回最接近参数的整数，如果有2个整数同样接近，则会返回偶数的那个  |
| `CastRoundMode::RNA` | round模式，四舍五入求整 |
| `CastRoundMode::RH` | 随机舍入，算子中目前特指输出结果是hif8数据时，会用到的一种随机舍入。 |
| `CastRoundMode::RO` | 向奇数的方向舍入，既当小数点后数值不为0时，如果整数位是偶数，则进位 |
| `CastRoundMode::RU` | ceil模 式，向上取整 |
| `CastRoundMode::RZ` | truncation模式，截断取整） |

### 饱和模式（SatMode）

| 饱和模式 | 说明 |
| :--- | :--- |
| `CastSatMode::Sat` | 饱和模式，超出目标类型范围时截断到最大/最小值 |
| `CastSatMode::NoSat` | 非饱和模式，超出目标类型范围时行为未定义 |

### 索引位置（IndexPos）

| 索引位置 | 说明 |
| :--- | :--- |
| `CastIndexPos::Even` | 偶数位 |
| `CastIndexPos::Odd` | 奇数位 |
| `CastIndexPos::PartP0` | 第0部分 |
| `CastIndexPos::PartP1` | 第1部分 |
| `CastIndexPos::PartP2` | 第2部分 |
| `CastIndexPos::PartP3` | 第3部分 |

### 舍入操作

除了 `Inst::Cast` 外，还支持以下舍入操作，用于对同类型数据进行舍入：

| 操作类型 | 功能说明 | 公式 |
| :--- | :--- | :--- |
| `Inst::Rint` | 就近舍入到整数 | `dst[i] = rint(src[i])` |
| `Inst::Round` | 四舍五入到整数 | `dst[i] = round(src[i])` |
| `Inst::Floor` | 向下取整 | `dst[i] = floor(src[i])` |
| `Inst::Ceil` | 向上取整 | `dst[i] = ceil(src[i])` |
| `Inst::Trunc` | 向零取整 | `dst[i] = trunc(src[i])` |

## 函数原型

```cpp
// 使用 Inst::Cast 进行类型转换
template<typename CalcFunc, typename TraitType, typename DstTensor, typename SrcTensor>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor& src)

// 使用舍入操作对同类型数据进行舍入
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor& src)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，类型转换取 `Inst::Cast`，舍入操作取 `Inst::Rint`、`Inst::Round`、`Inst::Floor`、`Inst::Ceil`、`Inst::Trunc`。 |
| TraitType | Cast Trait 特征参数类型，用于指定舍入模式、饱和模式和索引位置。舍入操作使用默认值 `Std::ignore_t`。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |
| SrcTensor | 源张量类型，需为 Tensor API Tensor 类型，存储位置为 `Location::UB`。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的张量，存储位置为 `Location::UB`，数据类型为目的类型。 |
| src | 输入 | 源张量，存储位置为 `Location::UB`，数据类型为源类型。 |

## 数据类型

### 类型转换支持矩阵

以下为 `Inst::Cast` 支持的源类型到目的类型的转换组合。每种转换组合支持的舍入模式（R）、饱和模式（S）和索引位置（I）请参考代码中 `CastTrait` 的定义。

| 源类型 | 目的类型 | 宽度关系 | 说明 |
| :--- | :--- | :--- | :--- |
| bfloat16_t | bfloat16_t | 1:1 | 舍入操作 |
| bfloat16_t | float | 1:2 | BF16 转 float |
| bfloat16_t | half | 1:1 | BF16 转 half |
| bfloat16_t | int32_t | 1:2 | BF16 转 int32 |
| bfloat16_t | fp4x2_e1m2_t | 2:1 | BF16 转 FP4 E1M2 |
| bfloat16_t | fp4x2_e2m1_t | 2:1 | BF16 转 FP4 E2M1 |
| float | bfloat16_t | 2:1 | float 转 BF16 |
| float | float | 1:1 | 舍入操作 |
| float | half | 2:1 | float 转 half |
| float | int16_t | 2:1 | float 转 int16 |
| float | int32_t | 1:1 | float 转 int32 |
| float | int64_t | 1:2 | float 转 int64 |
| float | fp8_e4m3fn_t | 4:1 | float 转 FP8 E4M3 |
| float | fp8_e5m2_t | 4:1 | float 转 FP8 E5M2 |
| float | hifloat8_t | 4:1 | float 转 HiFloat8 |
| half | bfloat16_t | 1:1 | half 转 BF16 |
| half | float | 1:2 | half 转 float |
| half | half | 1:1 | 舍入操作 |
| half | int8_t | 2:1 | half 转 int8 |
| half | uint8_t | 2:1 | half 转 uint8 |
| half | int16_t | 1:1 | half 转 int16 |
| half | int32_t | 1:2 | half 转 int32 |
| half | hifloat8_t | 2:1 | half 转 HiFloat8 |
| half | int4x2_t | 4:1 | half 转 Int4x2 |
| int8_t | half | 1:2 | int8 转 half |
| int8_t | int16_t | 1:2 | int8 转 int16 |
| int8_t | int32_t | 1:4 | int8 转 int32 |
| int16_t | float | 1:2 | int16 转 float |
| int16_t | half | 1:1 | int16 转 half |
| int16_t | int32_t | 1:2 | int16 转 int32 |
| int16_t | uint8_t | 2:1 | int16 转 uint8 |
| int16_t | uint32_t | 1:2 | int16 转 uint32 |
| int32_t | float | 1:1 | int32 转 float |
| int32_t | int16_t | 2:1 | int32 转 int16 |
| int32_t | int64_t | 1:2 | int32 转 int64 |
| int32_t | uint8_t | 4:1 | int32 转 uint8 |
| int32_t | uint16_t | 2:1 | int32 转 uint16 |
| int64_t | float | 2:1 | int64 转 float |
| int64_t | int32_t | 2:1 | int64 转 int32 |
| uint8_t | half | 1:2 | uint8 转 half |
| uint8_t | int16_t | 1:2 | uint8 转 int16 |
| uint8_t | uint16_t | 1:2 | uint8 转 uint16 |
| uint8_t | uint32_t | 1:4 | uint8 转 uint32 |
| uint16_t | uint8_t | 2:1 | uint16 转 uint8 |
| uint16_t | uint32_t | 1:2 | uint16 转 uint32 |
| uint32_t | int16_t | 2:1 | uint32 转 int16 |
| uint32_t | uint8_t | 4:1 | uint32 转 uint8 |
| uint32_t | uint16_t | 2:1 | uint32 转 uint16 |
| fp8_e4m3fn_t | float | 1:4 | FP8 E4M3 转 float |
| fp8_e5m2_t | float | 1:4 | FP8 E5M2 转 float |
| hifloat8_t | float | 1:4 | HiFloat8 转 float |
| hifloat8_t | half | 1:2 | HiFloat8 转 half |
| int4x2_t | half | 1:2 | Int4x2 转 half |
| int4x2_t | int16_t | 1:4 | Int4x2 转 int16 |
| int4x2_t | bfloat16_t | 1:4 | Int4x2 转 BF16 |
| fp4x2_e1m2_t | bfloat16_t | 1:4 | FP4 E1M2 转 BF16 |
| fp4x2_e2m1_t | bfloat16_t | 1:4 | FP4 E2M1 转 BF16 |

### 舍入操作支持的数据类型

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| `Inst::Rint` / `Inst::Round` / `Inst::Floor` / `Inst::Ceil` / `Inst::Trunc` | float, half, bfloat16_t |

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 源 Tensor 和目的 Tensor 的数据类型可以不同，具体支持的转换组合请参考上述数据类型支持矩阵。
- 源 Tensor 和目的 Tensor 的数据量（由 Layout 的 `Size()` 确定）需匹配。对于宽度不同的类型转换，数据量按目的 Tensor 的元素数计算。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，根据源/目的类型宽度关系选择对应的搬运和存储策略（pack/unpack）。
- 使用 `Inst::Cast` 时必须通过 TraitType 指定 Cast Trait，否则将使用默认 trait，可能无法匹配到正确的底层指令。
- FP4/FP8/HiFloat8/Int4x2 等小位宽类型的转换涉及 PartP0~PartP3 索引位置，需分四次调用以完成完整转换。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

// 定义 Cast Trait
struct CastTraitFloat2HalfRDNoSatEven {
    using roundMode = CastRoundMode::RD;
    using satMode = CastSatMode::NoSat;
    using indexPos = CastIndexPos::Even;
};

__aicore__ inline void CastComputeExample(__gm__ float* gmX, __gm__ half* gmZ)
{
    __ubuf__ float xUB[TILE_LENGTH];
    __ubuf__ half zUB[TILE_LENGTH];

    auto xGm = MakeTensor(MakeMemPtr<Location::GM>(gmX), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(MakeMemPtr<Location::GM>(gmZ), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, Std::Int<TILE_LENGTH>{}));

    // 搬入数据
    auto copyGM2UB = MakeCopy(CopyGM2UB{});
    Copy(copyGM2UB, xLocal, xGm);

    // 执行类型转换: z = (half)x，向下舍入，非饱和，偶数位
    Transform<Inst::Cast, CastTraitFloat2HalfRDNoSatEven>(zLocal, xLocal);

    // 搬出数据
    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```