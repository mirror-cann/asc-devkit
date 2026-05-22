# Fusion Compute Practices Sample Introduction
## Overview
Operator samples implemented using SIMT and SIMD hybrid programming, introducing SIMT-based flexible branch logic implementation, as well as high-performance Matmul fusion operator implementation and performance optimization methods using UB to improve discrete memory access efficiency.
 
## Sample List
| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [grouped_matmul](./grouped_matmul) |  This sample introduces high-performance implementation of QuantGroupMatmul operator on NPU, supporting grouped quantization matrix multiplication and Gelu activation computation. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [simt_and_simd_floor_mod](./simt_and_simd_floor_mod) |  Operator sample implemented using SIMT and SIMD hybrid programming, introducing SIMT-based flexible branch logic implementation. | Ascend 950PR/Ascend 950DT |
| [simt_gather_with_ub](./simt_gather_with_ub) |  This sample uses the Gather operator as an example to demonstrate performance optimization using UB to improve discrete memory access efficiency in SIMD and SIMT hybrid programming mode | Ascend 950PR/Ascend 950DT |