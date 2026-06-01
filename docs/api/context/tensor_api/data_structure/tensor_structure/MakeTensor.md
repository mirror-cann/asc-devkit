# MakeTensor

## 产品支持情况

| 产品 | 是否支持 |
| :--- | :------: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

需要包含的头文件为：#include "tensor_api/tensor.h"。

MakeTensor负责将内存指针适配器（由[MakeMemPtr](./MakeMemPtr.md)生成）与布局对象（由[MakeFrameLayout](../layout_structure/MakeFrameLayout.md)/[MakeLayout](../layout_structure/MakeFrameLayout.md)生成）绑定，生成带有完整类型信息的LocalTensor对象。

`MakeTensor`内部会根据第一个参数的类型自动判断：

- 第一个参数是否是一个已具备解引用能力的迭代器。若是，则作为Engine（内存引擎）使用。
- 后续参数会被处理为Layout。

生成的LocalTensor对象可直接传入Copy（数据搬运）和Mmad（矩阵计算）接口，编译器会根据其Engine中的物理位置标记和Layout中的布局模式，自动选择底层硬件指令。

## 函数原型

```cpp
template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto MakeTensor(const Iterator& iter, const Args&... args)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| iter | 输入 | 迭代器，要求传入ViewEngine对象，用于创建LocalTensor。 |
| args | 输入 | 可变参数，当前支持传入一个或两个参数。<br>&bull; 当传入一个入参时：根据传入的Layout对象创建LocalTensor。<br>&bull; 当传入两个入参时：根据传入的参数构建Layout对象，并基于该Layout创建LocalTensor。 |

## 返回值说明

- 返回LocalTensor<TensorAttribute<Engine, Layout>>类型的张量对象。

## 约束说明

- `iter`必须是迭代器类型。
- `args...`当前仅支持传入参数Layout或参数Shape和Stride。

## 调用示例

  ```cpp
  // 示例1：使用指针和Layout创建张量
  constexpr int tileNum = 4;
  __cbuf__ half dataPtr[tileNum]; // 初始化
  auto ptr = MakeMemPtr(dataPtr);
  auto layout = MakeFrameLayout<NZLayoutPtn, half>(32, 32);
  auto tensor = MakeTensor(ptr, layout);
  
  // 示例2：使用指针和形状创建张量（自动计算步幅）
  auto tensor2 = MakeTensor(ptr, MakeShape(32, 32), MakeStride(32, 32));
  ```
