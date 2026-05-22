# Matrix Compute API样例介绍
## 概述
本样例集介绍了矩阵计算API不同特性的典型用法，给出了对应的端到端实现。目录下的样例命名按照"API名称_通路"，如下：
1. **load_data**：以"load_data"开头的样例介绍L1 Buffer -> L0 Buffer的相关API；
2. **mmad**：以"mmad"开头的样例介绍矩阵乘法Mmad API；
3. **fixpipe**：以"fixpipe"开头的样例介绍L0C Buffer → Global Memory/L1 Buffer/Unified Buffer的相关API；

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [fixpipe_l0c2gm](./fixpipe_l0c2gm) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从CO1（L0C Buffer）搬出到GM（Global Memory） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fixpipe_l0c2ub](./fixpipe_l0c2ub) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从CO1（L0C Buffer）搬出到UB（Unified Buffer） | Ascend 950PR/Ascend 950DT |
| [fixpipe_l0c2l1](./fixpipe_l0c2l1) |  本样例介绍如何使用Fixpipe将矩阵乘的结果从CO1（L0C Buffer）搬出到L1（L1 Buffer） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [load_data_l12l0](./load_data_l12l0) |  本样例介绍了在 B4/ B8 / B16 / B32 四种数据类型下，左、右矩阵转置不转置组合 共 14 种矩阵乘法场景中，相关指令的使用方法。重点介绍基于基础API LoadData实现A1至A2和B1至B2的数据搬运，包括Load2D、Load3D、LoadDataWithTranspose | Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [load_data_2dv2_l12l0](./load_data_2dv2_l12l0) |  本样例介绍基于基础API LoadData实现A1至A2和B1至B2的数据搬运，包括Load2Dv2 | Ascend 950PR/Ascend 950DT |
| [load_data_2dmx_l12l0](./load_data_2dmx_l12l0) |  本样例介绍了在 FP4/ FP8两种数据类型下，左矩阵和左量化矩阵、右矩阵和右量化矩阵转置、不转置组合共 6 种量化矩阵乘法，相关指令的使用方法。重点介绍基于基础API LoadData实现A和scaleA、B和scaleB矩阵从L1到L0的数据搬运 | Ascend 950PR/Ascend 950DT |
| [mmad_load3dv2](./mmad_load3dv2) |  本样例介绍LoadData3DV2指令将A、B矩阵从L1搬运到L0A/L0B的过程，其中 A 和 B 分别表示矩阵乘法的左右输入矩阵。LoadData3DV2指令参数配置及执行指令前后各个矩阵数据排布变化，均配合示意图进行了说明 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [batch_mmad](./batch_mmad) |  本样例介绍在输入为float数据类型并且左、右矩阵均不转置的场景下，带batch的矩阵乘法，其中从GM-->L1、L0C-->GM、L0C-->L1这三条通路分别采用了DataCopy ND2NZ和Fixpipe批量搬运数据，从L1-->L0A/L0B以及Mmad执行矩阵乘这两个步骤则是循环batch次，每次循环内只处理一对左、右矩阵 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad](./mmad) |  本样例介绍了输入为ND格式，B4 / B8 / B16 / B32输入数据类型（具体以int4_t / int8_t / bfloat16 / float为例），四种输入数据类型下的矩阵乘法，说明如何通过Mmad指令实现矩阵乘法计算（C = A x B + Bias） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_unitflag](./mmad_unitflag) |  本样例介绍了如何在调用Mmad指令时，使用unitFlag功能。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_gemv](./mmad_gemv) |  本样例介绍了Gemv（M=1）模式下的矩阵乘法 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [mmad_with_sparse](./mmad_with_sparse) |  本样例介绍基础API MmadWithSparse调用 | Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
