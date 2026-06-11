# Features样例介绍

## 概述

介绍 Ascend C SIMD 特性样例，覆盖编译工程、算子调用、框架接入、Basic API、AI CPU 以及 Tensor API 等场景，用于展示不同特性的典型实现方式和编译运行流程。

## 样例列表

| 目录名称  | 功能描述 |
| --------- | --------- |
| [00_framework](./00_framework) | 本样例介绍了PyTorch、TensorFlow和ONNX框架的自定义算子实现方法 |
| [01_basic_api](./01_basic_api) | 本样例介绍了Ascend C Basic API的使用方法，包括数据搬运、矩阵计算、内存向量计算、资源管理、同步控制、系统访问、原子操作和标量计算等 |
| [06_aclrtc](./06_aclrtc) | 样例主要介绍使用Aclrtc（运行时编译）接口在Host侧编译字符串形式的Ascend C核函数，并完成编译结果的加载和执行的过程 |
| [99_aclnn_op](./99_aclnn_op) | 本样例介绍了自定义算子编译工程和Aclnn/Aclop算子调用的实现方法 |
