# 资源管理类api样例介绍

## 概述

本路径下包含了与资源管理相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [get_tpipe_ptr](./get_tpipe_ptr) |  样例基于GetTPipePtr获取全局TPipe指针，核函数无需显式传入TPipe指针，即可进行TPipe相关的操作。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [tpipe_reuse](./tpipe_reuse) |  本样例基于TPipe::Init和TPipe::Destroy，实现TPipe重复申请与使用。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
