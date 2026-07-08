# asc_trunc / asc_rint / asc_round / asc_floor / asc_ceil

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |


## 功能说明

头文件路径：`"c_api/reg_compute/reg_convert.h"`。

将源操作数的浮点数元素截断到整数位，同时源操作数的数据类型保持不变，并支持多种舍入模式。

关于舍入模式的详细说明，请参见[舍入模式](./rounding_mode.md)。

## 函数原型

```cpp
// RINT舍入模式
__simd_callee__ inline void asc_rint(vector_half& dst, vector_half src, vector_bool mask)
// RINT舍入模式
__simd_callee__ inline void asc_rint(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT舍入模式
__simd_callee__ inline void asc_rint(vector_float& dst, vector_float src, vector_bool mask)
// ROUND舍入模式
__simd_callee__ inline void asc_round(vector_half& dst, vector_half src, vector_bool mask)
// ROUND舍入模式
__simd_callee__ inline void asc_round(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND舍入模式
__simd_callee__ inline void asc_round(vector_float& dst, vector_float src, vector_bool mask)
// FLOOR舍入模式
__simd_callee__ inline void asc_floor(vector_half& dst, vector_half src, vector_bool mask)
// FLOOR舍入模式
__simd_callee__ inline void asc_floor(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR舍入模式
__simd_callee__ inline void asc_floor(vector_float& dst, vector_float src, vector_bool mask)
// CEIL舍入模式
__simd_callee__ inline void asc_ceil(vector_half& dst, vector_half src, vector_bool mask)
// CEIL舍入模式
__simd_callee__ inline void asc_ceil(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL舍入模式
__simd_callee__ inline void asc_ceil(vector_float& dst, vector_float src, vector_bool mask)
// TRUNC舍入模式
__simd_callee__ inline void asc_trunc(vector_half& dst, vector_half src, vector_bool mask)
// TRUNC舍入模式
__simd_callee__ inline void asc_trunc(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC舍入模式
__simd_callee__ inline void asc_trunc(vector_float& dst, vector_float src, vector_bool mask)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义.md](../reg数据类型定义.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_half src;
vector_half dst;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_rint(dst, src, mask);
```
