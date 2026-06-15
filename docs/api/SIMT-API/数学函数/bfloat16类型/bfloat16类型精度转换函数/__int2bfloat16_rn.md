# \_\_int2bfloat16\_rn

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

遵循CAST\_RINT模式，将int32类型数据转换为bfloat16类型数据，返回转换后的值。

## 函数原型

```
inline bfloat16_t __int2bfloat16_rn(const int x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入遵循CAST\_RINT模式转换成的bfloat16类型数据。

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
    __global__ __launch_bounds__(1024) void kernel__int2bfloat16_rn(bfloat16_t* dst, int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int2bfloat16_rn(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__int2bfloat16_rn(__gm__ bfloat16_t* dst, __gm__ int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int2bfloat16_rn(x[idx]);
    }
    ```
