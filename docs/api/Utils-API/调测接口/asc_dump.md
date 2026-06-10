# asc\_dump

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

将对应内存上的数据打印出来，同时支持打印自定义的附加信息（仅支持uint32\_t类型的信息），比如打印当前行号等。

使用该接口需要包含"utils/debug/asc\_dump.h"头文件。

> [!CAUTION]注意
>该功能主要用于**调试和性能分析**，通常在调测阶段使用，会对算子性能产生一定影响，**生产环境建议关闭**。
>默认情况下，该功能开启。

## 函数原型

```
// GM内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// L1内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// L0C内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)

// GM内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// L1内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// L0C内存上的数据打印
template<typename T>
__aicore__ inline void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size)
```

以下接口为simd\_vf中所使用的asc\_dump接口，仅支持Ascend 950PR/Ascend 950DT。

```
// 寄存器上的数据打印
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)

// 寄存器上的数据打印
template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// UB内存上的数据打印
template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| T | 输入 | 待dump的数据类型。 |
| U | 输入 | 待dump的寄存器数据类型。 |
| input | 输入 | 所需打印的内存块的起始地址。 |
| desc | 输入 | 自定义信息。uint32_t类型，支持的数据范围是[0, 2^32 - 1]。 |
| dump_size | 输入 | 所需要打印的元素数量。 |

## 返回值说明

无

## 约束说明

-   针对Ascend 950PR/Ascend 950DT，在使用该接口打印L1 Tensor数据时，HDK版本需要至少升级到25.7.0以上。

-   使用该接口时，在每个核上dump的数据总量不能超过1M，请开发者自行控制打印的内容数据量，超出则不会打印。
-   在计算数据量时，若dump的总长度未对齐，需要考虑padding数据的影响。当进行非对齐dump时，如果实际dump的元素长度不满足32字节对齐，系统会自动在其末尾补充一定数量的padding数据，以满足对齐要求。

## 调用示例

```
__gm__ half* src;
uint32_t desc = 1;
uint32_t dump_size = 32;
asc_dump_gm<half>(src, desc, dump_size);
```

SIMD VF调用示例：

```
__simd_vf__ inline void SimdVfDumpReg()
{
    AscendC::Reg::RegTensor<float> src_reg;
    uint32_t desc = 1;
    uint32_t dump_size = 32;
    asc_dump<float>(src_reg, desc, dump_size);
}
```
