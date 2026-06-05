# Fusion Compute Practices Sample Introduction

## Overview

This directory provides best-practice samples for fusion compute, covering QuantGroupMatmul grouped quantized matrix multiplication, Cube-Vector fusion for Matmul+GELU, and optimization methods for SIMT and SIMD hybrid programming.

## Sample List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [quant_group_matmul_high_performance](./quant_group_matmul_high_performance) |  This sample introduces the high-performance implementation of the QuantGroupMatmul operator, supports per-token quantized grouped matrix multiplication and GELU activation computation, and demonstrates performance tuning for Vector-bound scenarios in CV fusion. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  This sample demonstrates a high-performance Cube-Vector fusion implementation that fuses Matmul matrix multiplication and the GELU activation function in the same AI Core for parallel execution. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matrix_transpose](./matrix_transpose) |  This sample uses matrix transpose as an example to introduce memory coalescing optimization in Ascend C SIMD and SIMT hybrid programming scenarios, demonstrating how UB staging optimizes GM read and write access patterns. | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_high_performance](./simt_and_simd_high_performance) |  This sample uses FloorMod as an example to introduce performance tuning in SIMD and SIMT hybrid programming scenarios, showing the performance differences among direct SIMT access to GM, SIMD RegBase computation, SIMT access to UB, and adjusting the thread mapping so that adjacent threads within a warp access UB contiguously. | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_grid_dim_config](./simt_and_simd_grid_dim_config) |  This sample uses Gather computation as an example to introduce gridDim configuration and vf function call optimization strategies in Ascend C SIMD and SIMT hybrid programming scenarios. | Ascend 950PR/Ascend 950DT |
