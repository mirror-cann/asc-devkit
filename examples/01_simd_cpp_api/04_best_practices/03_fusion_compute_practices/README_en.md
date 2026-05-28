# Fusion Compute Practices Sample Introduction

## Overview

This directory provides best-practice samples for fusion compute, covering QuantGroupMatmul grouped quantized matrix multiplication, Cube-Vector fusion for Matmul+GELU, and optimization methods for SIMT and SIMD hybrid programming.

## Sample List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [quant_group_matmul_high_performance](./quant_group_matmul_high_performance) |  This sample introduces the high-performance implementation of the QuantGroupMatmul operator, supports per-token quantized grouped matrix multiplication and GELU activation computation, and demonstrates performance tuning for Vector-bound scenarios in CV fusion. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  This sample demonstrates a high-performance Cube-Vector fusion implementation that fuses Matmul matrix multiplication and the GELU activation function in the same AI Core for parallel execution. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matrix_transpose](./matrix_transpose) |  This sample uses matrix transpose as an example to introduce memory coalescing optimization in Ascend C SIMD and SIMT hybrid programming scenarios, demonstrating how UB staging optimizes GM read and write access patterns. | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_floor_mod](./simt_and_simd_floor_mod) |  This sample uses the FloorMod operator as an example to demonstrate SIMT and SIMD hybrid operator development: SIMD DataCopy is used for data movement, SIMD is used for float computation, and SIMT is used for int32_t computation. | Ascend 950PR/Ascend 950DT |
| [simt_and_simd_high_performance](./simt_and_simd_high_performance) |  This sample uses FloorMod as an example to introduce performance tuning in SIMD and SIMT hybrid programming scenarios, showing the performance differences among direct SIMT access to GM, SIMD RegBase computation, SIMT access to UB, and memory coalescing optimization within a warp. | Ascend 950PR/Ascend 950DT |
| [simt_gather_with_ub](./simt_gather_with_ub) |  This sample uses Gather as an example to demonstrate how UB improves discrete memory access efficiency in SIMD and SIMT hybrid programming, comparing direct SIMT access to GM with accessing data after preloading it to UB. | Ascend 950PR/Ascend 950DT |
