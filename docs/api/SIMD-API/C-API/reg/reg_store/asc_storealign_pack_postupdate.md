# asc_storealign_pack_postupdate

## 产品支持情况

| 产品         | 是否支持 |
| :-----------------------| :-----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径：`"c_api/reg_compute/reg_store.h"`。

从矢量数据寄存器或掩码寄存器压缩搬出到UB的接口，根据mask将src中有效元素的低半部分bit数据连续存储于dst中，接口调用后自动更新目的操作数地址。支持数据类型为b16、b32、b64。

## 函数原型

- 从矢量数据寄存器连续对齐搬出到UB的场景
  ```cpp
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ int16_t*& dst_align32b, vector_int16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint16_t*& dst_align32b, vector_uint16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ int64_t*& dst_align32b, vector_int64_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint64_t*& dst_align32b, vector_uint64_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ half*& dst_align32b, vector_half src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ bfloat16_t*& dst_align32b, vector_bfloat16_t src, int32_t offset, vector_bool mask)
  ```
- 从掩码寄存器连续对齐搬出到UB的场景
  ```cpp
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset)
  ```

## 参数说明

- 从矢量数据寄存器连续对齐搬出到UB的场景

  | 参数名  | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | dst_align32b | 输入/输出 | 目的操作数（矢量）的起始地址，搬运完成后该地址会被硬件自动Post Update。 |
  | src | 输入 | 源操作数（矢量数据寄存器）。 |
  | offset | 输入 | 地址偏移量。 |
  | mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

- 从掩码寄存器连续对齐搬出到UB的场景

  | 参数名  | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | dst | 输入/输出 | 目的操作数（矢量）的起始地址，搬运完成后该地址会被硬件自动Post Update。 |
  | src | 输入 | 源操作数（掩码寄存器）。 |
  | offset | 输入 | 地址偏移量。 |

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
int32_t offset = 128;
asc_storealign_pack_postupdate(dst_align32b, src, offset, mask);
```
