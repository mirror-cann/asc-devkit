# 数据搬运类api样例介绍

## 概述

本路径下包含了与数据搬运相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。

## 样例列表

| 目录名称　　　　　　　　　　　　　　　　　　　　　　　　 | 功能描述　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| ----------------------------------------------------------| -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [broadcast_ub2l0c](./broadcast_ub2l0c)　　　　　　　　　 | 本样例基于BroadCastVecToMM实现数据广播搬运，将位于UB（Unified Buffer）上数据进行广播并搬运到CO1（L0C Buffer）　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [copy_ub2ub](./copy_ub2ub)　　　　　　　　　　　　　　　 | 本样例基于Copy实现数据搬运，适用于VECIN，VECOUT之间的数据搬运，支持mask连续模式和counter模式。　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| [data_copy_gm2ub_slice](./data_copy_gm2ub_slice)　　　　 | 本样例基于DataCopy实现数据切片搬运，提取多维Tensor数据的子集进行GM（Global Memory）与UB（Unified Buffer）通路之间的搬运　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [data_copy_gm2ub_nddma](./data_copy_gm2ub_nddma)　　　　 | 本样例介绍如何使用多维数据搬运接口实现GM（Global Memory）到UB（Unified Buffer）通路的数据搬运，通过自由配置搬入的维度信息以及对应的Stride，可以用于Padding、Transpose、BroadCast、Slice等多种数据变换操作 |
| [data_copy_l0c2gm](./data_copy_l0c2gm)　　　　　　　　　 | 本样例在卷积场景下，基于DataCopy实现数据随路量化激活搬运　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| [data_copy_pad_gm2ub_ub2gm](./data_copy_pad_gm2ub_ub2gm) | 本样例基于DataCopyPad实现数据非32字节对齐数据的搬运，并进行数据填充　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [data_copy_ub2l1](./data_copy_ub2l1)　　　　　　　　　　 | 本样例在Mmad矩阵乘场景下，基于DataCopy实现UB（Unified Buffer）到L1（L1 Buffer）的数据搬运　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [ld_st_reg_mask](./ld_st_reg_mask)　　　　　　　　　　　 | 本样例基于Reg编程接口实现UB(Unified Buffer)对MaskReg(掩码寄存器)的搬入搬出，以及使用mask进行掩码搬出的操作　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| [ld_st_reg_align](./ld_st_reg_align)　　　　　　　　　　 | 本样例基于Reg编程接口实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的连续和非连续的对齐数据搬运操作　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [ld_st_reg_unalign](./ld_st_reg_unalign)　　　　　　　　 | 本样例基于Reg编程接口实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的非对齐数据搬运操作　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
| [gather_ld_reg](./gather_ld_reg)　　　　　　　　　　　　 | 本样例演示使用Gather接口实现离散数据搬入，包含高维Gather（源为LocalTensor）和Reg::GatherB（按DataBlock收集）两种场景　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| [scatter_st_reg](./scatter_st_reg)　　　　　　　　　　　 | 本样例演示使用Reg::Scatter接口实现离散数据搬出（按元素分散到UB）　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|
| [auxscalar_reg](./auxscalar_reg)　　　　　　　　　　　　　　　　 | 本样例演示使用AuxScalar方式从UB读取多个标量数据参与计算|
| [move_reg](./move_reg)　　　　　　　　　　　　　　　　　　　　　　 | 本样例基于Reg编程接口实现UB(Unified Buffer)对RegTensor的数据搬入搬出操作　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　|