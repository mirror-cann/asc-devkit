# 向量计算总体说明

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

Tensor API 向量计算提供基于 Unified Buffer 的向量计算能力，通过统一的 `Transform` 接口，将向量计算操作类型（如 Add、Sub、Cast 等）与 Tensor 对象结合，由编译器根据操作类型和数据类型自动选择底层硬件指令实现。

向量计算接口通过 `Transform` 模板函数统一入口，按操作类型分为以下几类：

- **[一元计算](unary_computation.md)**：对单个源 Tensor 的每个元素执行逐元素运算，如 Abs、Exp、Sqrt、Ln、Not 等。
- **[二元计算](binary_computation.md)**：对两个源 Tensor 的对应元素执行逐元素运算，如 Add、Sub、Mul、Div、Max、Min 等。
- **[标量二元计算](binary_scalar_computation.md)**：对源 Tensor 的每个元素与标量执行运算，如 AddScalar、MulScalar、MaxScalar 等。
- **[类型转换](cast_computation.md)**：将源 Tensor 的数据类型转换为目的数据类型，支持多种数据类型间的转换和多种舍入模式。
- **[双输出计算](dual_computation.md)**：对两个源 Tensor 执行运算并产生两个输出 Tensor，如 Mull（长乘法）。

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename... Args>
__aicore__ inline void Transform(const Args&... args)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，取值为 `Inst` 命名空间下的操作类，如 `Inst::Add`、`Inst::Cast` 等。 |
| TraitType | 计算特征参数类型，用于指定计算的附加属性（如 Cast 的舍入模式、饱和模式等）。默认为 `Std::ignore_t`，表示使用默认特征。 |
| Args | 变长参数包，根据操作类型不同，传入对应数量的 Tensor 对象和标量值。 |

**函数参数说明**

不同操作类型对应的参数如下：

| 操作类型 | 参数说明 |
| :--- | :--- |
| 一元计算 | `Transform<Inst::XXX>(dst, src)` |
| 二元计算 | `Transform<Inst::XXX>(dst, src0, src1)` |
| 标量二元计算 | `Transform<Inst::XXX>(dst, src, value)` |
| 类型转换 | `Transform<Inst::Cast, CastTrait>(dst, src)` |
| 双输出计算 | `Transform<Inst::XXX>(dst0, dst1, src0, src1)` |

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor 和目的 Tensor 均需位于 Unified Buffer（UB），存储位置为 `Location::UB`。
- 一元计算、二元计算、标量二元计算、Axpy 计算和双输出计算中，源 Tensor 和目的 Tensor 的数据类型需一致。
- 类型转换中，源 Tensor 和目的 Tensor 的数据类型可以不同，具体支持的数据类型转换组合请参考[类型转换](cast_computation.md)。
- Tensor 的数据量由其 Layout 的 `Size()` 方法确定，所有参与计算的 Tensor 数据量需匹配。
- 计算过程中按 Vector 寄存器宽度自动分 repeat 执行，末尾不足一个寄存器的部分通过 mask 机制处理。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void VectorComputeExample(__gm__ half* gmX, __gm__ half* gmY, __gm__ half* gmZ)
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

    // 执行二元计算: z = x + y
    Transform<Inst::Add>(zLocal, xLocal, yLocal);

    // 搬出数据
    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```