# Fusion Compute Practices样例介绍

## 概述

本目录提供融合计算相关的最佳实践样例，覆盖QuantGroupMatmul分组量化矩阵乘、Matmul+GELU的Cube-Vector融合实现，以及SIMT与SIMD混合编程下的优化方法。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [quant_group_matmul_high_performance](./quant_group_matmul_high_performance) |  本样例介绍QuantGroupMatmul算子的高性能实现，支持per-token量化的分组矩阵乘法与GELU激活计算，并展示CV融合时Vector bound场景下的性能调优方法。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  本样例展示Cube-Vector融合的高性能实现，将Matmul矩阵乘法与GELU激活函数融合到同一AI Core中并行执行。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matrix_transpose](./matrix_transpose) |  本样例以矩阵转置为例，介绍Ascend C SIMD与SIMT混合编程场景下的访存合并优化思路，展示通过UB中转优化GM读写访问模式的方法。 | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_high_performance](./simt_and_simd_high_performance) |  本样例以FloorMod计算为例，介绍SIMD与SIMT混合编程场景下的性能调优方法，展示SIMT直接访问GM、SIMD RegBase计算、SIMT访问UB以及调整线程映射使Warp内连续访问UB优化的性能差异。 | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_grid_dim_config](./simt_and_simd_grid_dim_config) |  本样例以Gather计算为例，介绍Ascend C SIMD与SIMT混合编程场景下的gridDim配置和vf函数调用优化思路。 | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_integer_fast_div](./simt_and_simd_integer_fast_div) |  本样例以整数除法为例，展示SIMD与SIMT混合编程场景下针对固定除数使用乘法和移位替代普通除法的指令优化方式。 | Ascend 950PR/Ascend 950DT |
