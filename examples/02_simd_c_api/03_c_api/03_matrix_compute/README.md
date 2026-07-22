# Matrix Compute API样例介绍

## 概述

本样例集介绍了矩阵计算API不同特性的典型用法，给出了对应的端到端实现。目录下的样例命名按照"API名称_通路"，如下：
1. **load_data**：以"load_data"开头的样例介绍L1 Buffer -> L0 Buffer的相关API；
2. **mmad**：以"mmad"开头的样例介绍矩阵乘法Mmad API；
3. **fixpipe**：以"fixpipe"开头的样例介绍L0C Buffer → Global Memory/L1 Buffer/Unified Buffer的相关API；

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [mmad](./mmad) |  本样例以 int8_t 和 bfloat16 两种输入数据类型为例，演示如何通过C API实现矩阵乘法（C = A × B + Bias） | Ascend 950PR/Ascend 950DT |
