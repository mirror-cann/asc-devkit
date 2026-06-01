# L1 Buffer到Fixpipe Buffer数据搬运

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件为：`#include "tensor_api/tensor.h"`。

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的量化数据搬运到Fixpipe Buffer。量化数据可用于L0C输出到GM/UB时做随路量化。

## 函数原型

- 执行L1 Buffer到Fixpipe Buffer的数据搬运。

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
| atomCopy | 输入 | 搬运原子对象。L1 Buffer到Fixpipe Buffer通路可通过`MakeCopy(CopyL12FB{})`或`MakeCopy(CopyL12FB{}, CopyL12FBTraitDefault{})`构造。 |
| dst | 输出 | 目的张量，存储位置为`Location::FIXBUF`，数据格式为`ND`。 |
| src | 输入 | 源张量，存储位置为`Location::L1`，数据格式为`ND`。 |

**表2** `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象。L1 Buffer到Fixpipe Buffer通路使用`CopyL12FB{}`。 |
| copyTrait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到Fixpipe Buffer通路默认Trait使用`CopyL12FBTraitDefault{}`。 |

## 数据类型

源张量和目的张量的数据类型要求是`uint64_t`。

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 源Tensor内存地址和大小需要满足32Byte对齐要求。
- 目的Tensor内存地址和大小需要满足128Byte对齐要求。
- 由于Fixpipe Buffer大小为4KB，因此每次搬运最多4KB数据。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToFixpipeExample()
{
    using DataType = uint64_t;
    constexpr uint32_t n = 16;

    __cbuf__ DataType l1Buf[n];
    __fbuf__ DataType fixbuf[n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto fixbufTensor = MakeTensor(MakeMemPtr(fixbuf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto copyAtom = MakeCopy(CopyL12FB{}, CopyL12FBTraitDefault{});
    Copy(copyAtom, fixbufTensor, l1Tensor);
}
```
