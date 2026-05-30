# BestPractices样例介绍

## 概述

基于Ascend C SIMT的算子开发实践，展示SIMT特性在实际算子开发中的运用。

## 样例列表

| 目录名称                                               | 功能描述                                                                                        |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| [01_simt_matrix_transpose](./01_simt_matrix_transpose)   | 样例基于Ascend C SIMT编程方式实现MatrixTranspose算子，展示Ascend C SIMT编程方式的性能调优思路。 |
| [02_short_vector_add](./02_short_vector_add)            | 样例基于Ascend C SIMT编程方式实现Add算子，主要演示基于短向量half2性能调优方法。 |
| [03_simt_data_cache_hint](./03_simt_data_cache_hint)     | 样例基于Ascend C SIMT编程方式实现Sin算子，展示SIMT算子的缓存优化能力。                        |
