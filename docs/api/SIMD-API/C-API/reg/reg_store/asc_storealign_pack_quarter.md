# asc_storealign_pack_quarter

## 产品支持情况

| 产品         | 是否支持 |
| :-----------------------| :-----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径：`"c_api/reg_compute/reg_store.h"`。

从矢量数据寄存器压缩搬出到UB的接口，根据mask将src中有效元素的低8bits数据连续存储于dst_align32b中，支持数据类型为b32。

支持三种偏移方式：
- 偏移固定传入0，由用户自行更新目的操作数的地址。
- 通过int32_t传入偏移，用户可以选择更新偏移或者更新目的操作数的地址。
- 通过iter_reg地址寄存器传入偏移，用户可以选择更新偏移或者更新目的操作数的地址。

支持Post Update模式，接口调用后自动更新目的操作数地址。

## 函数原型

- 偏移固定传入0，由用户自行更新目的操作数的地址
  ```cpp
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask)
  ```
- 通过int32_t传入偏移，用户可以选择更新偏移或者更新目的操作数的地址
  ```cpp
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask)
  ```
- 通过iter_reg地址寄存器传入偏移，用户可以选择更新偏移或者更新目的操作数的地址
  ```cpp
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst_align32b, vector_int32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ float* dst_align32b, vector_float src, iter_reg offset, vector_bool mask)
  ```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst_align32b | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| offset | 输入 | 偏移量。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义.md](../reg数据类型定义.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
__ubuf__ float* dst_align32b = (__ubuf__ float*)asc_get_phy_buf_addr(0);
vector_float src;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_storealign_pack_quarter(dst_align32b, src, mask);
```
