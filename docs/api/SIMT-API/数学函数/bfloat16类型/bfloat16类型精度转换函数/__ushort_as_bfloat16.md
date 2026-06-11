# \_\_ushort\_as\_bfloat16

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

将unsigned short int的数据按位重新解释为bfloat16，即将unsigned short int的数据存储的位按照bfloat16的格式进行读取。

## 函数原型

```
inline bfloat16_t __ushort_as_bfloat16(const unsigned short int x)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

unsigned short int的数据按位重新解释为bfloat16的值。

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
    __global__ __launch_bounds__(1024) void kernel__ushort_as_bfloat16(bfloat16_t* dst, unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__ushort_as_bfloat16(__gm__ bfloat16_t* dst, __gm__ unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```

