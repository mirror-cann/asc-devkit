# L1到BiasTable Buffer数据搬运

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件为：`#include "tensor_api/tensor.h"`

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的bias数据搬运到BiasTable Buffer，作为Mmad计算中的bias输入。

## 函数原型

- 执行L1 Buffer到BiasTable Buffer的数据搬运。

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

**表1** Copy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `atomCopy` | 输入 | 搬运原子对象，L1 Buffer到BiasTable Buffer通路可通过`MakeCopy(CopyL12BT{})`或`MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{})`构造。 |
| `dst` | 输出 | 目的张量，存储位置为`Location::BT`，数据格式为ND。 |
| `src` | 输入 | 源张量，存储位置为`Location::L1`，数据格式为ND。 |

**表2** MakeCopy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `copyOperation` | 输入 | 搬运操作对象，L1 Buffer到BiasTable Buffer通路使用`CopyL12BT{}`。 |
| `copyTrait` | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到BiasTable Buffer通路默认Trait使用`CopyL12BTTraitDefault{}`。 |

## 数据类型

支持的目的张量和源张量的数据类型组合如下：
| 目的张量数据类型 | 源张量数据类型 |
| :--- | :--- |
| `float` | `bfloat16_t` |
| `float` | `half` |
| `float` | `float` |
| `int32_t` | `int32_t` |

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## API映射关系

与built-in接口映射关系：

L1 Buffer到BiasTable Buffer搬运接口是在built-in接口`asc_copy_l12bt`的基础上进行抽象封装实现的，其对应的底层built-in接口为：

```cpp
__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ bfloat16_t* src,
    uint16_t conv_control,
    uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap);
__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ half* src,
    uint16_t conv_control,
    uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap);
__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ float* src,
    uint16_t conv_control,
    uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap);
__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ int32_t* src,
    uint16_t conv_control,
    uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap);
```

## 约束说明

- 源Tensor内存地址和大小需要满足32Byte对齐要求
- 目的Tensor内存地址和大小需要满足64Byte对齐要求。
- 由于BiasTable Buffer大小为4KB，因此每次搬运最多4KB数据

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToBTExample()
{
    using SrcType = half;
    using DstType = float;
    constexpr uint32_t n = 16;

    __cbuf__ SrcType l1Buf[n];
    __biasbuf__ DstType btBuf[n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto btTensor = MakeTensor(MakeMemPtr(btBuf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto copyAtom = MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{});
    Copy(copyAtom, btTensor, l1Tensor);
}
```
