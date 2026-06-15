# MakeMemPtr

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

MakeMemPtr用于构造带物理存储位置标记和数据类型信息的内存指针适配器HardwareMemPtr。支持的物理位置如[表](#支持的物理位置)所示。

MakeMemPtr提供三种调用形式：

1. 指定物理位置和数据类型，根据原始地址构造指针。
2. 指定物理位置，从已有迭代器或指针适配器构造新指针。
3. 从已有迭代器或指针适配器中自动推导物理位置。

## 函数原型

    - 指定物理位置和数据类型，根据原始地址构造：

    ```cpp
    template <typename Hardware, typename DataType, typename Addr,
        EnableMakePtrByTrait<Hardware, Addr> Enable>
    __aicore__ inline constexpr auto MakeMemPtr(Addr addr)
    ```

    - 指定物理位置，从已有迭代器构造：

    ```cpp
    template <typename Hardware, typename Iterator,
        EnableMakeHardwarePtr<Hardware, Iterator> Enable>
    __aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
    ```

    - 从已有迭代器中自动推导物理位置：

    ```cpp
    template <typename Iterator, EnableMakePtrByIter<Iterator> Enable>
    __aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
    ```

## 参数说明

**表1** 支持的物理位置

| Location | 说明 | 用途 |
| :--- | :--- | :--- |
| Location::GM | Global Memory | 全局内存，张量输入输出的主要存储区域 |
| Location::L1 | L1 Buffer | 一级缓存，矩阵搬运和计算前的数据暂存区 |
| Location::L0A | L0A Buffer | 矩阵计算左输入缓存 |
| Location::L0B | L0B Buffer | 矩阵计算右输入缓存 |
| Location::L0ScaleA | L0 ScaleA Buffer | MX矩阵计算ScaleA缓存 |
| Location::L0ScaleB | L0 ScaleB Buffer | MX矩阵计算ScaleB缓存 |
| Location::L0C | L0C Buffer | 矩阵计算结果缓存 |
| Location::UB | Unified Buffer | 统一缓存，矢量计算的数据存储区 |
| Location::BIAS | BiasTable Buffer | 偏置表缓存，带偏置矩阵计算的偏置存放区 |
| Location::FIXBUF | Fixpipe Buffer | Fixpipe输出缓存，L0C到GM/UB的中转区 |

**表2** 模板参数说明

| 参数名 | 类型 | 描述 |
| :--- | :---: | :--- |
| Hardware | 输入 | 物理存储位置类型。 |
| DataType | 输入 | 数据类型。 |
| Addr | 输入 | 原始地址类型。 |
| Iterator | 输入 | 迭代器类型或已有指针适配器类型。 |

**表3** 参数说明

| 参数名 | 类型 | 描述 |
| :--- | :---: | :--- |
| addr | 输入 | 原始地址或地址偏移量。 |
| iterator | 输入 | 已有的硬件地址迭代器或指针适配器对象。 |

## 返回值说明

返回HardwareMemPtr<PtrPattern, Pointer>类型的指针适配器对象，其中：

- PtrPattern表示指定或推导得到的物理位置；
- Pointer表示对应地址空间下的指针类型，例如__gm__ float*、__cbuf__ half*等。

## 约束说明

- 指定Hardware时，必须是受支持的物理位置类型。
- Iterator需要满足对应构造路径的模板约束。
- 自动推导形式要求输入对象本身已经带有可识别的硬件位置信息。

## 调用示例

```cpp
using namespace AscendC::Te;

// 1. 从原始地址构造：指定物理位置和数据类型
auto gmPtr = MakeMemPtr<Location::GM, float>(gmBuf);
auto l1Ptr = MakeMemPtr<Location::L1, float>(l1Buf);
auto l0aPtr = MakeMemPtr<Location::L0A, half>(l0aBuf);
auto ubPtr = MakeMemPtr<Location::UB, bfloat16_t>(ubBuf);

// 2. 配合MakeTensor构造张量
auto gmA = MakeTensor(gmPtr, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));
auto l1A = MakeTensor(l1Ptr, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

// 3. 从已有指针适配器继承位置（自动推导）
auto anotherPtr = MakeMemPtr(gmPtr);

// 4. 从已有迭代器指定新位置
auto l1FromIter = MakeMemPtr<Location::L1>(someIterator);
```
