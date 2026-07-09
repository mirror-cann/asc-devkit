# asc_uint82uint16

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

将vector_uint8_t类型的源操作数以256B为单位分为两部分，读取其中一部分元素，将其转换成vector_uint16_t类型并写入目的操作数。

由于源操作数与目的操作数类型位宽比为1:2，读取数据时需要将一个VL大小的数据分为两部分，根据不同接口选取索引0或者索引1。

## 函数原型

```cpp
// 选取索引0的位置
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
// 选取索引1的位置
__simd_callee__ inline void asc_uint82uint16_v2(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
```

## 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。            |
| src | 输入    | 源操作数（矢量数据寄存器）。            |
| mask     | 输入    | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。        |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义.md](../reg数据类型定义.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_uint16_t dst;
vector_uint8_t src;
vector_bool mask;
mask = asc_create_mask_b8(PAT_ALL);
asc_uint82uint16(dst, src, mask);    // 将src的上半部分转换成vector_uint16_t类型并写入dst
asc_uint82uint16_v2(dst, src, mask);    // 将src的下半部分转换成vector_uint16_t类型并写入dst
```