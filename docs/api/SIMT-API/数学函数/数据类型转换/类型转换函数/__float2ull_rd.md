# \_\_float2ull\_rd

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

遵循CAST\_FLOOR模式，将浮点数转换为64位无符号整数，返回转换后的值。

## 函数原型

```
inline unsigned long long int __float2ull_rd(const float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入遵循CAST\_FLOOR模式转换成的64位无符号整数。特别场景说明如下：

-   当x为nan时，返回值为0。
-   当x为inf时，返回值为18446744073709551615。
-   当x为-inf时，返回值为0。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/device\_functions.h"头文件。

```
#include "simt_api/device_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void kernel__float2ull_rd(uint64_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2ull_rd(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2ull_rd(__gm__ uint64_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2ull_rd(x[idx]);
    }
    ```
