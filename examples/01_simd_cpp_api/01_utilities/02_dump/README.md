# Dump样例介绍
## 概述
本样例展示了Ascend C asc_dump系列接口和simd_vf侧asc_dump接口的基本使用方法。

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [simple_dump](./simple_dump) | 使用静态Tensor编程模式实现矩阵乘法，展示asc_dump_gm/asc_dump_l1buf/asc_dump_cbuf/asc_dump_ubuf接口的基本使用方法 | Ascend 950PR/Ascend 950DT<br>Atlas A3训练系列产品/Atlas A3推理系列产品<br>Atlas A2训练系列产品/Atlas A2推理系列产品 |
| [simd_vf_dump](./simd_vf_dump) | 使用vector编程模式，展示simd_vf侧asc_dump_ubuf/asc_dump/asc_dump_reg接口的基本使用方法 | Ascend 950PR/Ascend 950DT |
| [special_buffer_dump](./special_buffer_dump) | 实现HiFloat8量化MatMul算子，使用DumpTensor打印HiFloat8输入矩阵、Bias和per-channel scale在L1 Buffer、Bias Table Buffer和Fixpipe Buffer中的数据 | Ascend 950PR/Ascend 950DT |
