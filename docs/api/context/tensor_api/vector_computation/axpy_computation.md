# Axpy计算

## 产品支持情况

| 产品 | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

Axpy计算执行 dst = dst + src * scalar运算，即目的张量的每个元素与源张量对应元素乘以标量后累加。通过Transform接口传入Inst::Axpy操作类型来实现。

与标量二元计算中的Inst::MulScalar不同，Axpy是一个原地累加操作，目的张量既作为输入也作为输出。

## 函数原型

```cpp
template<typename CalcFunc, typename TraitType = Std::ignore_t, typename DstTensor, typename SrcTensor, typename ScalarType>
__aicore__ inline void Transform(const DstTensor& dst, const SrcTensor& src, const ScalarType& value)
```

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| CalcFunc | 计算操作类型，Axpy 计算取Inst::Axpy。 |
| TraitType | 计算特征参数类型，Axpy计算使用默认值Std::ignore_t。 |
| DstTensor | 目的张量类型，需为 Tensor API Tensor 类型，存储位置为Location::UB。同时作为输入和输出。 |
| SrcTensor | 源张量类型，需为 Tensor API Tensor 类型，存储位置为Location::UB。 |
| ScalarType | 标量值类型。 |

**函数参数说明**

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输入/输出 | 目的张量，存储位置为Location::UB。既作为累加输入，也作为结果输出。 |
| src | 输入 | 源张量，存储位置为Location::UB。 |
| value | 输入 | 标量值，作为 src 的乘数。 |

## 数据类型

| 操作类型 | 支持的数据类型 |
| :--- | :--- |
| Inst::Axpy | half, float |

源张量、目的张量和标量值的数据类型需要保持一致。支持的组合为 `half/half/half` 和 `float/float/float`，不支持 `dst`、`src`、`value` 混合数据类型计算。

## 返回值说明

无返回值。

## 约束说明

- 源 Tensor、目的 Tensor 和标量值的数据类型需一致。
- `Inst::Axpy` 底层调用同类型 `asc_axpy` 指令，不支持 `dst=float`、`src=half` 等混合类型组合；如需 half 输入输出 float 结果，可先使用 `Inst::Cast` 将源 Tensor 转换为 float，再执行 float 类型计算。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t TILE_LENGTH = 2048;

__aicore__ inline void AxpyComputeExample(__gm__ half* gmX, __gm__ half* gmZ)
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
    Copy(copyGM2UB, zLocal, zGm);

    // 执行 Axpy 计算: z = z + x * 2.0
    Transform<Inst::Axpy>(zLocal, xLocal, half(2.0));

    // 搬出数据
    auto copyUB2GM = MakeCopy(CopyUB2GM{});
    Copy(copyUB2GM, zGm, zLocal);
}
```
