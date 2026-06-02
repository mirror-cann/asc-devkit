# MakeTensor

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

MakeTensor负责将内存指针（由[MakeMemPtr](./MakeMemPtr.md)生成）与布局对象（由[MakeFrameLayout](../layout_structure/MakeFrameLayout.md)/[MakeLayout](../layout_structure/MakeLayout.md)生成）绑定，生成带有完整类型信息的Tensor对象。

MakeTensor会根据内存指针的物理位置标记，自动推导返回GlobalTensor或LocalTensor，当指针物理位置位于Global Memory（外部存储）时，返回GlobalTensor；当指针位置位于AI Core上的Local Memory（内部存储）时，返回LocalTensor。

生成的Tensor对象可直接传入Copy（数据搬运）和Mmad（矩阵计算）等接口，编译器会根据其Engine中的物理位置标记和Layout中的布局模式，自动选择底层硬件指令。

## 函数原型

```cpp
template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto MakeTensor(const Iterator& iter, const Args&... args)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| iter | 输入 | 迭代器，要求传入ViewEngine对象，用于创建Tensor。返回类型由该迭代器绑定的内存位置决定。 |
| args | 输入 | 可变参数，当前支持传入一个或两个参数。<br>&bull; 当传入一个入参时：根据传入的Layout对象创建Tensor。<br>&bull; 当传入两个入参时：根据传入的参数构建Layout对象，并基于该Layout创建Tensor。 |

## 返回值说明

- 返回GlobalTensor<TensorAttribute<Engine, Layout>>或LocalTensor<TensorAttribute<Engine, Layout>>类型的张量对象，具体由Engine绑定的内存位置决定。

## 调用示例

  ```cpp
  // 示例1：使用GM指针和Layout创建GlobalTensor
  auto gmPtr = MakeMemPtr<Location::GM, half>(gmAddr);
  auto gmLayout = MakeLayout(MakeShape(32, 32), MakeStride(32, 1));
  auto gmTensor = MakeTensor(gmPtr, gmLayout);

  // 示例2：使用Local Memory指针和Layout创建LocalTensor
  constexpr int tileNum = 4;
  __cbuf__ half dataPtr[tileNum]; // 初始化
  auto ptr = MakeMemPtr(dataPtr);
  auto layout = MakeFrameLayout<NZLayoutPtn, half>(32, 32);
  auto tensor = MakeTensor(ptr, layout);
  
  // 示例3：使用指针和形状创建张量（自动计算步幅）
  auto tensor2 = MakeTensor(ptr, MakeShape(32, 32), MakeStride(32, 32));
  ```
