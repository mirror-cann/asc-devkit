# Basic API Sample Introduction

## Overview

This sample introduces the usage of Ascend C Basic API, including data movement, matrix computation, memory vector computation, resource management, synchronization control, system access, atomic operations, and scalar computation.

## Sample List

| Directory Name  | Description |
| --------- | --------- |
| [00_data_movement](./00_data_movement) | This directory contains samples for multiple APIs related to data movement, demonstrating the usage of data movement interfaces.|
| [01_matrix_compute](./01_matrix_compute) | This sample directory uses operators such as batch_mmad and fixpipe_co12c1_quantization as examples to demonstrate the usage of matrix computation interfaces.|
| [02_memory_vector_compute](./02_memory_vector_compute) | This sample directory uses operators such as reduce, sort, and transpose as examples to demonstrate the usage of memory vector computation interfaces.|
| [03_reg_vector_compute](./03_reg_vector_compute) | This directory contains samples for multiple APIs related to vector computation, implemented based on Reg programming interfaces.|
| [04_resource_management](./04_resource_management) | This directory contains samples for multiple APIs related to resource management. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [06_sync_control](./06_sync_control) | This directory contains samples for multiple APIs related to synchronization control. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [07_system_access](./07_system_access) | This directory contains samples for multiple APIs related to system variable access. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [08_atomic_operations](./08_atomic_operations) | This directory contains samples for multiple APIs related to atomic operations. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |
| [09_scalar_computation](./09_scalar_computation) | This directory contains samples for multiple APIs related to scalar computation. Each sample is based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same cpp file.         |