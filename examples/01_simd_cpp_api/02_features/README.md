# Features样例介绍

## 概述

介绍 Ascend C SIMD 特性样例，覆盖编译工程、算子调用、框架接入、Basic API、AI CPU 以及 Tensor API 等场景，用于展示不同特性的典型实现方式和编译运行流程。

## 样例列表

| 目录名称  | 功能描述 |
| --------- | --------- |
| [00_compilation](./00_compilation) | 本样例介绍了自定义算子编译工程和静态Aclnn调用的实现方法 |
| [01_invocation](./01_invocation) | 本样例介绍了Aclnn和Aclop算子调用的实现方法 |
| [02_framework](./02_framework) | 本样例介绍了PyTorch、TensorFlow和ONNX框架的自定义算子实现方法 |
| [03_basic_api](./03_basic_api) | 本样例介绍了Ascend C Basic API的使用方法，包括数据搬运、矩阵计算、内存向量计算、资源管理、同步控制、系统访问、原子操作和标量计算等 |
| [04_aicpu](./04_aicpu) | 本样例介绍了使用AI CPU算子进行Tiling下沉计算的实现方法 |
| [05_tensor_api](./05_tensor_api) | 本样例展示了使用TENSOR_API构建多核场景Matmul算子的实现方法，并使能Relu和随路量化 |