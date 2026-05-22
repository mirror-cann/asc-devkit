# 系统变量访问类api样例介绍

## 概述

本路径下包含了与系统变量访问相关的API样例。样例基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------- | -------- | --- |
| [ctrl_spr](./ctrl_spr) |  本样例基于SetCtrlSpr、GetCtrlSpr、ResetCtrlSpr接口实现对CTRL寄存器（控制寄存器）的特定比特位的设置、读取和重置，并验证非饱和模式是否正确。 | Ascend 950PR/Ascend 950DT |
