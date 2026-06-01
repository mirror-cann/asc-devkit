# L1 Buffer到L0ScaleB Buffer数据搬运

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件为：`#include "tensor_api/tensor.h"`

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵缩放数据搬运到L0ScaleB Buffer。右矩阵缩放数据在L0ScaleB Buffer上的首地址由右矩阵在L0B Buffer的首地址的1/16推导出来。

该通路的数据类型固定为`fp8_e8m0_t`。左矩阵和右矩阵的缩放数据的分形排布和缩放计算关系可参考下图，其中右矩阵缩放数据是`NN`的数据格式：

![左矩阵和右矩阵缩放数据缩放示意图](../figures/zh-cn_image_0000002549011155.png)

## 函数原型

- 执行L1 Buffer到L0ScaleB Buffer的数据搬运。

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
| `atomCopy` | 输入 | 搬运原子对象。L1 Buffer到L0ScaleB Buffer通路可通过`MakeCopy(CopyL12L0ScaleB{})`或`MakeCopy(CopyL12L0ScaleB{}, CopyL12L0ScaleBTraitDefault{})`构造。 |
| `dst` | 输出 | 目的张量，存储位置为`Location::L0ScaleB`，数据格式为`NN`。 |
| `src` | 输入 | 源张量，存储位置为`Location::L1`，数据格式为`NN`。 |

**表2** `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| `copyOperation` | 输入 | 搬运操作对象。L1 Buffer到L0ScaleB Buffer通路使用`CopyL12L0ScaleB{}`。 |
| `copyTrait` | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0ScaleB Buffer默认Trait使用`CopyL12L0ScaleBTraitDefault{}`。 |

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 数据类型

数据类型仅支持`fp8_e8m0_t`。

源张量和目的张量的基础数据类型需要保持一致。

## API映射关系

与built-in接口映射关系：

L1 Buffer到L0ScaleB Buffer搬运接口是在built-in接口`asc_copy_l12l0b_mx`的基础上进行抽象封装实现的，其对应的底层built-in接口为：

```cpp
__aicore__ inline void asc_copy_l12l0b_mx(uint64_t dst, __cbuf__ fp8_e8m0_t* src,
    uint16_t x_start_pos, uint16_t y_start_pos, uint8_t x_step, uint8_t y_step,
    uint16_t src_stride, uint16_t dst_stride);
```

## 约束说明

- 源张量和目的张量数据格式为`NN`，使用`MakeFrameLayout<NNLayoutPtn, _2>(...)`构造。
- 源地址和目的地址需要满足32 Byte对齐要求。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0ScaleBExample()
{
    constexpr uint32_t k = 32;
    constexpr uint32_t n = 16;
    constexpr uint32_t scaleK = k / 16;

    __cbuf__ fp8_e8m0_t l1Buf[scaleK * n];
    __cb__ fp8_e5m2_t l0BBuf[k * n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NNLayoutPtn, _2>(scaleK, n));

    // ScaleB地址由右矩阵的L0B Buffer地址按1/16地址编码换算得到。
    auto l0ScaleBPtr = MakeMemPtr<Location::L0ScaleB, fp8_e8m0_t>(reinterpret_cast<uint64_t>(l0BBuf) / 16);
    auto l0ScaleBTensor = MakeTensor(l0ScaleBPtr, MakeFrameLayout<NNLayoutPtn, _2>(scaleK, n));

    auto copyAtom = MakeCopy(CopyL12L0ScaleB{}, CopyL12L0ScaleBTraitDefault{});
    Copy(copyAtom, l0ScaleBTensor, l1Tensor);
}
```
