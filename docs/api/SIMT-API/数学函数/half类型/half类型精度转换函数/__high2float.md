# \_\_high2float

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x |
| Atlas 200I/500 A2 推理产品 | x |
| Atlas 推理系列产品AI Core | x |
| Atlas 推理系列产品Vector Core | x |
| Atlas 训练系列产品 | x |

## 功能说明

将输入数据的高16位转换为float类型并返回结果。

## 函数原型

```
inline float __high2float(const half2 x)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据的高16位转换为float类型的结果。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/asc\_fp16.h"头文件。

```
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    // 使用短向量可提升数据搬运效率
    __aicore__ void simt_high2float(half2* input, float* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __high2float(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(half* input, float* output, uint32_t input_total_length)
    {
        simt_high2float((half2*)input, output, input_total_length);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_high2float(__gm__ half2* input, __gm__ float* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __high2float(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input, __gm__ float* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_high2float>(dim3(1024), (__gm__ half2*)input, output, input_total_length);
    }
    ```
