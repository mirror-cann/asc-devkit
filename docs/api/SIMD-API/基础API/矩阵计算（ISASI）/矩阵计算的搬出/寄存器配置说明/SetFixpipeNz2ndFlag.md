# SetFixpipeNz2ndFlag<a id="ZH-CN_TOPIC_0000002569070993"></a>

## 产品支持情况<a id="zh-cn_topic_0000002516209232_section18204144912492"></a>

| 产品 | 是否支持 |
| ---------- | :----------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √ </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | √ </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |
| <cann-filter npu-type = "x90">Kirin X90 | √ </cann-filter> |
| <cann-filter npu-type = "9030">Kirin 9030 | √ </cann-filter> |

## 功能说明<a id="zh-cn_topic_0000002516209232_section618mcpsimp"></a>

DataCopy数据搬运（[L0C-\>GM](../L0C到GM数据搬运（DataCopy）.md)）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。

## 函数原型<a id="zh-cn_topic_0000002516209232_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride)
```

<cann-filter npu-type = "950">

```cpp
// 如下原型仅Ascend 950PR/Ascend 950DT支持
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
```

</cann-filter>

## 参数说明<a id="zh-cn_topic_0000002516209232_section622mcpsimp"></a>

<cann-filter npu-type = "A3,910b,310b">

针对如下产品型号，参数说明见下表：

<cann-filter npu-type = "A3">

Atlas A3 训练系列产品/Atlas A3 推理系列产品；

</cann-filter>

<cann-filter npu-type = "910b">

Atlas A2 训练系列产品/Atlas A2 推理系列产品；

</cann-filter>

<cann-filter npu-type = "310b">

Atlas 200I/500 A2 推理产品；

</cann-filter>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。取值范围∈[1, 512]，单位为1024B。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[1, 65535]，单位为element元素。 |

</cann-filter>

<cann-filter npu-type = "950">

针对Ascend 950PR/Ascend 950DT：

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。取值范围∈[0, 65535]，单位为C0_SIZE。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[1, 2^32-1]，单位为element元素。 |

</cann-filter>

<cann-filter npu-type = "x90,9030">

针对如下产品型号，参数说明见下表：

<cann-filter npu-type = "x90">

Kirin X90；

</cann-filter>

<cann-filter npu-type = "9030">

Kirin 9030；

</cann-filter>

**表3** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。取值范围∈[0, 65535]，单位为C0_SIZE。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[0, 65535]，单位为element元素。 |

</cann-filter>

## 数据类型

参数的数据类型见[函数原型](#zh-cn_topic_0000002516209232_section620mcpsimp)。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="zh-cn_topic_0000002516209232_section633mcpsimp"></a>

无

## 调用示例<a id="section6461234123118"></a>

完整示例可参考[data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm)。

```cpp
uint16_t ndNum = 2;
uint16_t srcNdStride = 2;
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride); // 设置FIX搬运NZ格式到ND格式转换的参数
```
