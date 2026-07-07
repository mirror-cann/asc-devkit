# asc_storealign

## 产品支持情况

| 产品         | 是否支持 |
| :-----------------------| :-----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径：`"c_api/reg_compute/reg_store.h"`。

从矢量数据寄存器或掩码寄存器连续对齐搬出到UB的接口，实现NORM搬出模式：正常模式，搬运VL数据。支持数据类型为b8、b16、b32。

支持三种偏移方式：
- 偏移固定传入0，由用户自行更新目的操作数的地址。
- 通过int32_t传入偏移，用户可以选择更新偏移或者更新目的操作数的地址。
- 通过iter_reg地址寄存器传入偏移，用户可以选择更新偏移或者更新目的操作数的地址。

## 函数原型

- 从矢量数据寄存器连续对齐搬出到UB的场景
  ```cpp
  __simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst_align32b, vector_int8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst_align32b, vector_int16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ half* dst_align32b, vector_half src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__ int4b_t* dst_align32b, vector_int4x2_t src, vector_bool mask)
  ```
- 从掩码寄存器连续对齐搬出到UB的场景
  ```cpp
  __simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src)
  ```

## 参数说明

- 从矢量数据寄存器连续对齐搬出到UB的场景

  | 参数名  | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | dst_align32b | 输出 | 目的操作数（矢量）的起始地址。 |
  | src | 输入 | 源操作数（矢量数据寄存器）。 |
  | mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

- 从掩码寄存器连续对齐搬出到UB的场景

  | 参数名  | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | dst | 输出 | 目的操作数（矢量）的起始地址。 |
  | src | 输入 | 源操作数（掩码寄存器）。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义.md](../reg数据类型定义.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
__ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
vector_half src;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_storealign(dst_align32b, src, mask);
```
