# fdimf

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取两个输入数据的差值，差值小于0时，返回0。

![](../../../figures/zh-cn_formulaimage_0000002486622056.png)

## 函数原型

```
inline float fdimf(float x, float y)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

输入数据的差值，差值小于0时，返回0。

-   x，y任意一个为nan时，返回值为nan。
-   x为inf，y为-inf，返回值为inf。
-   x为-inf，y为inf，返回值为0。
-   x为inf，y为inf，返回值为0。

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
    __global__ __launch_bounds__(1024) void KernelDim(float* dst, float* x, float* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = fdimf(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelDim(__gm__ float* dst, __gm__ float* x, __gm__ float* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = fdimf(x[idx], y[idx]);
    }
    ```

