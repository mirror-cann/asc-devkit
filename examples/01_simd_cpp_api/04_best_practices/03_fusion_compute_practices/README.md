# Fusion Compute Practices样例介绍
## 概述
基于SIMT与SIMD混合编程方式实现的算子样例，介绍基于SIMT灵活实现分支判断逻辑，以及Matmul融合算子的高性能实现和使用UB提升离散内存访问效率的性能优化方式。
 
## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [grouped_matmul](./grouped_matmul) |  本样例介绍QuantGroupMatmul算子在NPU上高性能实现，支持分组量化矩阵乘与Gelu激活计算。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  本样例展示Cube-Vector融合的高性能实现，将Matmul矩阵乘法与GELU激活函数融合到同一AI Core中并行执行。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [simt_and_simd_floor_mod](./simt_and_simd_floor_mod) |  基于SIMT与SIMD混合编程方式实现的算子样例，介绍基于SIMT灵活实现分支判断逻辑。 | Ascend 950PR/Ascend 950DT |
| [simt_gather_with_ub](./simt_gather_with_ub) |  本样例以Gather算子为示例，展示了在SIMD与SIMT混合编程模式下使用UB提升离散内存访问效率的性能优化方式 | Ascend 950PR/Ascend 950DT |
