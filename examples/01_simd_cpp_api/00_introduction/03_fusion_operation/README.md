# FusionOperation样例介绍

## 概述

样例介绍自定义融合计算，包含Cube与Vector融合计算和SIMT + SIMD混合编程融合计算。

## 样例列表

| 目录名称 | 功能描述 |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [matmul_leakyrelu_high_level_api](./matmul_leakyrelu_high_level_api) | 基于高阶API实现Matmul矩阵乘与LeakyRelu激活函数计算融合 |
| [matmul_leakyrelu_basic_api](./matmul_leakyrelu_basic_api) | 基于基础API实现Matmul矩阵乘与LeakyRelu激活函数计算融合 |
| [gather_adds_simt_simd_hybrid](./gather_adds_simt_simd_hybrid) | 基于SIMT和SIMD实现Gather和Adds融合计算 |
| [matrix_transpose](./matrix_transpose/) | SIMD与SIMT混合编程下使用SIMT实现矩阵转置 |
