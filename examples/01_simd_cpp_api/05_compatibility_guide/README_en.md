# Compatibility Guide Example Introduction

## Overview

This section provides several examples for features that are incompatible with Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Ascend 950PR/Ascend 950DT. Users can migrate according to the examples.

This section uses the `<<<>>>` kernel call operator to complete the basic flow of running operator kernel functions on the NPU side, providing corresponding end-to-end implementations.

## Example List

|  Directory Name  |  Description  |
| -------------------------------------------------- | ---------------------------------------------------- |
| [data_copy_l1togm](./data_copy_l1togm) | This example demonstrates the end-to-end flow of transferring L1 data to GM. |
| [fill](./fill) | This example shows how to use the Fill interface to initialize L0A Buffer and L0B Buffer. |
| [matmul_s4](./matmul_s4) | This example demonstrates the end-to-end implementation of int4 matrix multiplication computation. |
| [pattern_transformation](./pattern_transformation) | This example demonstrates the fractal transformation logic for L1 Buffer -> L0A Buffer pathway based on the basic mmad example. |
| [scatter](./scatter) | This example demonstrates the scatter function, which can scatter the input tensor to the result tensor based on the input tensor and destination address offset tensor. |
| [set_loaddata_boundary](./set_loaddata_boundary) | This example implements the setting of L1 Buffer boundary values. |