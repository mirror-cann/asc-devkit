# asc_loadalign_deintlv

## 产品支持情况

| 产品         | 是否支持 |
| :-----------------------| :-----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

此函数用于将32B对齐UB地址起始的连续2 x VL长度的数据，以数据类型宽度为单个元素，随Load搬运解交织后，写入2个矢量数据寄存器。

如下图，以float数据类型，Ascend 950PR/Ascend 950DT产品为例：VL = 256B，可包含64个float元素，当调用`asc_loadalign_deintlv`后，其数据从UB到Reg的排布如下图。

![](../../figures/asc_loadalign_deintlv.png)



> 提示：使用此函数需包含`reg_load.h`头文件。

## 函数原型

`asc_loadalign_deintlv`按源地址的寻址方式分为三类：[基址寻址接口](#base-addressing-api)、[标量偏移寻址接口](#base-scalar-offset-api)、[地址寄存器偏移寻址接口](#base-register-indexed-api)。其支持的入参可参考[数据类型](#data-type)章节。

### 基址寻址接口<a name="base-addressing-api"></a>

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src)
```

### 标量偏移寻址接口<a name="base-scalar-offset-api"></a>

> 源数据UB的地址通过`src + offset`计算得到，并要求src + offset计算得到的UB地址满足32B对齐要求。


```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, int32_t offset)
```

### 地址寄存器偏移寻址接口<a name="base-register-indexed-api"></a>

> 源数据UB的地址由硬件根据循环嵌套层数以及迭代次数，自动更新`iter_reg offset`后计算得到，并要求src 满足32B对齐要求。需配合[`asc_create_iter_reg`](../reg_vector/asc_create_iter_reg.md)使用。

```cpp
// DEINTLV搬入模式
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, iter_reg offset)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst0 | 输出 | 目的操作数（矢量数据寄存器）。 |
| dst1 | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量）UB起始地址，需要32B对齐。 |
| offset | 输入 | 地址偏移量（标量偏移寻址接口：元素为单位）。 |

矢量数据寄存器详细说明请参见[reg数据类型定义.md](../reg数据类型定义.md)。

## 数据类型<a name="data-type"></a>

`asc_loadalign_deintlv`支持的数据类型如下表所示。三类寻址接口均支持表中的数据类型。

| 数据位宽 | 数据类别 | 源数据类型(src) | 目的寄存器类型(dst0/dst1) |
| :------- | :------- | :-------------- | :------------------------ |
| b4       | 整型     | `int4b_t`       | `vector_int4x2_t`         |
| b4       | 浮点     | `fp4x2_e1m2_t`  | `vector_fp4x2_e1m2_t`     |
| b4       | 浮点     | `fp4x2_e2m1_t`  | `vector_fp4x2_e2m1_t`     |
| b8       | 整型     | `int8_t`        | `vector_int8_t`           |
| b8       | 整型     | `uint8_t`       | `vector_uint8_t`          |
| b8       | 浮点     | `fp8_e4m3fn_t`  | `vector_fp8_e4m3fn_t`     |
| b8       | 浮点     | `fp8_e5m2_t`    | `vector_fp8_e5m2_t`       |
| b8       | 浮点     | `fp8_e8m0_t`    | `vector_fp8_e8m0_t`       |
| b8       | 浮点     | `hifloat8_t`    | `vector_hifloat8_t`       |
| b16      | 整型     | `int16_t`       | `vector_int16_t`          |
| b16      | 整型     | `uint16_t`      | `vector_uint16_t`         |
| b16      | 浮点     | `half`          | `vector_half`             |
| b16      | 浮点     | `bfloat16_t`    | `vector_bfloat16_t`       |
| b32      | 整型     | `int32_t`       | `vector_int32_t`          |
| b32      | 整型     | `uint32_t`      | `vector_uint32_t`         |
| b32      | 整型     | `float`         | `vector_float`            |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- [基址寻址接口](#base-addressing-api)中的src需要32B对齐，且src地址不超过UB最大地址减去2 x VL，否则会导致读UB越界报错。
- [标量偏移寻址接口](#base-scalar-offset-api)和[地址寄存器偏移寻址接口](#base-register-indexed-api)中的src + offset结果需要32B对齐，且结果不超过UB最大地址减去2 x VL，否则会导致读UB越界报错。
- [地址寄存器偏移寻址接口](#base-register-indexed-api)需要配合[`asc_create_iter_reg`](../reg_vector/asc_create_iter_reg.md)使用。

## 调用示例

读取UB上32B对齐的地址`src`开始的连续float数据。

### 基址寻址接口

```cpp
for (uint16_t i= 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size);
    ...
}
```

### 标量偏移寻址接口

```cpp
// 方式1: 通过offset偏移
for (uint16_t i= 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src, i * one_repeat_size);
    ...
}

// 方式2：通过src偏移
for (uint16_t i= 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size, 0);
    ...
}
```

### 地址寄存器偏移寻址接口

```
for (uint16_t i = 0; i < repeat_times; i++) {
    iter_reg addr_reg = asc_create_iter_reg_b32(one_repeat_size);
    asc_loadalign_deintlv(reg0, reg1, src, addr_reg);
    ...
}
```

