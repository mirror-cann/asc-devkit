# \_\_floats2bfloat162\_rn

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

将输入数据x、y遵循CAST\_RINT模式分别转换为bfloat16类型，并填充到bfloat16x2的前后两部分，返回转换后的bfloat16x2类型数据。

## 函数原型

```
inline bfloat16x2_t __floats2bfloat162_rn(const float x, const float y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

将输入float类型数据遵循CAST\_RINT模式分别转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回转换后的数据。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/asc\_bf16.h"头文件。

```
#include "simt_api/asc_bf16.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    // 使用短向量可提升数据搬运效率
    __aicore__ void simt_floats2bfloat162_rn(float* input1, float* input2, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __floats2bfloat162_rn(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(float* input1, float* input2, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_floats2bfloat162_rn(input1, input2, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_floats2bfloat162_rn(__gm__ float* input1, __gm__ float* input2, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __floats2bfloat162_rn(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input1,  __gm__ float* input2, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_floats2bfloat162_rn>(dim3(1024), input1, input2, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```
