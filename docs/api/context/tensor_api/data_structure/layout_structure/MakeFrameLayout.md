# MakeFrameLayout

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

MakeFrameLayout用于根据标准分形排布模式构造带类型信息的Layout对象。该接口接收矩阵的行数、列数，结合指定的LayoutPattern（排布模式）和LayoutTrait（布局特征参数）计算Shape和Stride，构造Layout对象。

LayoutPattern决定数据在内存中的分形排列方式，不同存储位置间的搬运和计算操作要求源/目的张量满足特定的LayoutPattern组合。

## 函数原型

```cpp
template <typename LayoutPattern, typename TraitType, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
```

### LayoutTrait的指定方式

LayoutTrait用于指定数据类型和C0维度基数。MakeFrameLayout支持以下四种传递方式：

1. **LayoutTraitDefault<T>**：指定数据类型T，C0自动计算。
   ```cpp
   MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n)
   ```

2. **_C0**：直接指定C0维度的元素个数，数据类型为[Int](../../../Utils-API/通用工具/Int.md)。_C0是Std::Int<C0>的简写形式。
   ```cpp
   MakeFrameLayout<NZLayoutPtn, _16>(m, n)
   ```

3. **不传入Trait**：在分形构造和Trait无关，或Trait值固定的场景下，不传入Trait，内部根据LayoutPattern查找对应的默认Trait。这样的分形有NDLayoutPtn、DNLayoutPtn、DNExtLayoutPtn、NDExtLayoutPtn、ScaleANDLayoutPtn、ScaleADNLayoutPtn、ScaleBNDLayoutPtn、ScaleBDNLayoutPtn。
   ```cpp
   MakeFrameLayout<NDLayoutPtn>(m, n)
   ```

4. **自定义Trait传入**：传入一个自定义的Trait类型。适用于需要显式描述数据类型、C0大小或其他布局特征，且现有默认Trait、_C0或固定默认Trait都无法满足需求的场景。自定义Trait需要满足底层布局推导所要求的类型约定。
   ```cpp
   struct MyLayoutTrait {
       using DataType = float;
       static constexpr auto C0_SIZE = Std::Int<16>{};
   };

   MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n)
   ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| LayoutPattern | 输入 | 布局模式模板参数。支持的LayoutPattern及数据排布格式见[Layout和层次化表述法](Layout和层次化表述法.md)。 |
| TraitType | 输入 | 布局特征类型，默认内部根据LayoutPattern自动推导。支持上述四种传递方式。 |
| Args | 输入 | 构造参数。常见为矩阵的行数m和列数n。各LayoutPattern的构造参数数量可能不同。 |

## 返回值说明

返回Layout<ShapeType, StrideType, Std::tuple<LayoutPattern, TraitType>>类型的对象，其中ShapeType和StrideType由LayoutPattern和构造参数在编译期推导。

## 约束说明

- LayoutPattern必须是已支持的排布模式之一，不支持的模式会触发编译期报错。
- 构造参数的个数和含义由LayoutPattern决定。常见二维场景传入(m, n)即可。
- TraitType必须为整型常量、TraitType类型或具体数据类型，不支持的类型会触发编译期报错。

## 调用示例

```cpp
using namespace AscendC::Te;

// 构造GM端ND布局张量
auto layoutGM = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n);
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM>(gmAddr), layoutGM);

// 构造L1端NZ布局张量（L0A/L0B搬运的标准格式）
auto layoutL1 = MakeFrameLayout<NZLayoutPtn, float>(m, n);
auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutL1);

// 使用默认Trait构造NZ布局
auto layoutNZ = MakeFrameLayout<NZLayoutPtn>(m, n);
auto l1TensorNZ = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutNZ);

// 指定C0元素数构造
auto layoutC0 = MakeFrameLayout<NZLayoutPtn, _16>(m, n);
auto l1TensorC0 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutC0);

// 自定义Trait构造
struct MyLayoutTrait {
    using DataType = float;
    static constexpr auto C0_SIZE = Std::Int<16>{};
};
auto layoutCustom = MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n);
```
