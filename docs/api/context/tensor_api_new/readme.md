# README

Tensor API 提供面向张量描述的数据搬运和矩阵计算接口。接口以 `AscendC::Te` 命名空间为入口，通过 `MakeMemPtr`、`MakeFrameLayout`、`MakeTensor` 构造带有存储位置、数据类型和数据排布信息的张量，再通过 `Copy` 或 `Mmad` 完成硬件指令封装调用。

## 使用入口

头文件为：

```cpp
#include "tensor_api/tensor.h"
```

Tensor API 当前主要包含以下能力：

- 数据搬运：使用 `Copy` 或 `MakeCopy` 构造的 `CopyAtom`，在 GM、L1、L0A、L0B、L0C、UB、BiasTable Buffer、Fixpipe Buffer 之间搬运数据。
- 矩阵计算：使用 `Mmad` 或 `MakeMmad` 构造的 `MmadAtom`，完成 L0A 与 L0B 输入、L0C 输出的矩阵乘加计算。
- 张量基础结构：使用 `MakeMemPtr`、`MakeFrameLayout`、`MakeLayout`、`MakeTensor`、`Slice` 等接口描述内存位置、数据排布、形状、步长和切片。

## 文档索 引

- [tensor_api_list](tensor_api_list.md)
- [数据搬运导览](data_movement/data_movement_overview.md)
- [GM到L1数据搬运](data_movement/gm_to_l1_data_movement.md)
- [L1到L0A数据搬运](data_movement/l1_to_l0a_data_movement.md)
- [L1到L0B数据搬运](data_movement/l1_to_l0b_data_movement.md)
- [L1到BiasTable Buffer数据搬运](data_movement/l1_to_bias_table_buffer_data_movement.md)
- [L1到Fixpipe Buffer数据搬运](data_movement/l1_to_fixpipe_buffer_data_movement.md)
- [L1到UB数据搬运](data_movement/l1_to_ub_data_movement.md)
- [L0C到GM数据搬运](data_movement/l0c_to_gm_data_movement.md)
- [L0C到UB数据搬运](data_movement/l0c_to_ub_data_movement.md)
- [矩阵计算流程](matrix_computation/matrix_computation_flow.md)
- [普通矩阵计算](matrix_computation/normal_matrix_computation.md)
- [MX矩阵计算](matrix_computation/mx_matrix_computation.md)
