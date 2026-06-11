# norm3df

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取输入数据a、b、c的平方和a^2 + b^2 + c^2的平方根。

![](../../../figures/zh-cn_formulaimage_0000002484776406.png)

## 函数原型

```
inline float norm3df(float a, float b, float c)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| a | 输入 | 源操作数。 |
| b | 输入 | 源操作数。 |
| c | 输入 | 源操作数。 |

## 返回值说明

a^2 + b^2 + c^2的平方根。

-   当a^2 + b^2+ c^2 的平方根超出float最大范围，返回值为inf。

-   若a, b, c任意一个或多个为±inf，返回值为inf。

-   若a, b, c任意一个或多个为nan同时不是±inf，返回值为nan。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/math\_functions.h"头文件。

```
#include "simt_api/math_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void KernelNorm3d(float* dst, float* a, float* b, float* c)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = norm3df(a[idx], b[idx], c[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelNorm3d(__gm__ float* dst, __gm__ float* a, __gm__ float* b, __gm__ float* c)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = norm3df(a[idx], b[idx], c[idx]);
    }
    ```

