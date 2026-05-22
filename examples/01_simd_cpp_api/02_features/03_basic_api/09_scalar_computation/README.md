# 标量计算类api样例介绍

## 概述

本路径下包含了与标量计算相关的API样例。样例基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------- | -------- | --- |
| [gm_by_pass_dcache](./gm_by_pass_dcache) |  本样例基于ReadGmByPassDcache和WriteGmByPassDcache接口，实现不经过DCache从GM读取和写入数据。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
