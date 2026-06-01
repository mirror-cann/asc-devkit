# L1 Buffer到L0A Buffer数据搬运

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件为：`#include "tensor_api/tensor.h"`

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的左矩阵数据搬运到L0A Buffer。

该通路以512 Byte数据分形为基本搬运单位。不同数据类型对应的分形矩阵形态如下：

| 数据位宽 | 分形矩阵形态 |
| :--- | :--- |
| b4 | 16 * 64 |
| b8 | 16 * 32 |
| b16 | 16 * 16 |
| b32 | 16 * 8 |

接口支持非转置搬运和转置搬运。源张量和目的张量的Layout需要匹配当前通路支持的格式组合。

## 函数原型

- 执行L1 Buffer到L0A Buffer的数据搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src);
    ```

- 构造默认搬运原子对象。

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation);
    ```

- 构造指定Trait的搬运原子对象。

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait);
    ```

## 参数说明

**表1** `Copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `atomCopy` | 输入 | 搬运原子对象。L1 Buffer到L0A Buffer通路可通过`MakeCopy(CopyL12L0A{})`或`MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{})`构造。 |
| `dst` | 输出 | 目的张量，存储位置为`Location::L0A`。 |
| `src` | 输入 | 源张量，存储位置为`Location::L1`。 |

**表2** `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `copyOperation` | 输入 | 搬运操作对象。L1 Buffer到L0A Buffer通路使用`CopyL12L0A{}`。 |
| `copyTrait` | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0A Buffer默认Trait使用`CopyL12L0ATraitDefault{}`。 |

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 数据类型

支持的数据类型包括：

`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`float`。

源张量和目的张量的数据类型需要保持一致。

## API映射关系

与built-in接口映射关系：

非转置搬运接口是在built-in接口`asc_copy_l12l0a`的基础上进行抽象封装实现的，其对应的底层built-in接口为：

```cpp
__aicore__ inline void asc_copy_l12l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ float8_e4m3_t* dst, __cbuf__ float8_e4m3_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ float8_e5m2_t* dst, __cbuf__ float8_e5m2_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ half* dst, __cbuf__ half* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ float* dst, __cbuf__ float* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ int16_t* dst, __cbuf__ int16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ uint16_t* dst, __cbuf__ uint16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
```

转置搬运接口是在built-in接口`asc_copy_l12l0a_transpose`的基础上进行抽象封装实现的，其对应的底层built-in接口为：

```cpp
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ float8_e4m3_t* dst, __cbuf__ float8_e4m3_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ float8_e5m2_t* dst, __cbuf__ float8_e5m2_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ half* dst, __cbuf__ half* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ float* dst, __cbuf__ float* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int16_t* dst, __cbuf__ int16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint16_t* dst, __cbuf__ uint16_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
__aicore__ inline void asc_copy_l12l0a_transpose(__ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src,
    uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);
```

## 约束说明

- 目的地址位于L0A Buffer时，起始地址需要满足512 Byte对齐要求；
- 源地址位于L1 Buffer时，起始地址需要满足32 Byte对齐要求。
- 特殊数据类型约束：L1 Buffer到L0A Buffer通路使能转置时，即`ZN2NZ`，支持的数据类型约束如下：
  - b32数据类型要求源矩阵Shape在K轴方向16对齐。
  - b8数据类型要求源矩阵Shape在M轴方向32对齐。
  - b4数据类型要求源矩阵Shape在M轴方向64对齐。

## 关键特性说明

### 非转置搬运

当源张量和目的张量数据格式为`NZ`时，接口按非转置方式从L1 Buffer搬运到L0A Buffer。

![非转置搬运示意图](../figures/notranspose.png)

### 转置搬运

当源张量数据格式为`ZN`、目的张量数据格式为`NZ`时，接口按转置方式搬运。转置搬运会对源张量中每个分形矩阵进行转置。

不同数据位宽的转置搬运示意如下。

#### b4转置搬运

![b4转置搬运示意图](../figures/transpose_b4.png)

#### b8转置搬运

![b8转置搬运示意图](../figures/transpose_b8.png)

#### b16转置搬运

![b16转置搬运示意图](../figures/transpose_b16.png)

#### b32转置搬运

![b32转置搬运示意图](../figures/transpose_b32.png)

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0AExample()
{
    using AType = half;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ AType l1Buf[m * k];
    __ca__ AType l0aBuf[m * k];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto l0aTensor = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto copyAtom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyAtom, l0aTensor, l1Tensor);
}
```
