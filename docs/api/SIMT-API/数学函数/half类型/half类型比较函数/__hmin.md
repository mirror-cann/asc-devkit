# \_\_hmin

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

获取两个输入数据中的最小值。

![](../../../../figures/zh-cn_formulaimage_0000002501694974.png)

## 函数原型

```
inline half __hmin(half x, half y)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

两个输入数据中的最小值。

-   x为-0，y为+0时，返回x。
-   x为+0，y为-0时，返回y。
-   x为nan时，返回值为y。
-   y为nan时，返回值为x。
-   x，y同时为nan时，返回值为nan。
-   x，y任意一个为-inf时，返回值为-inf。
-   x为inf时，返回值为y。
-   y为inf时，返回值为x。

## 约束说明

无

## 需要包含的头文件

使用half类型接口需要包含"simt\_api/asc\_fp16.h"头文件。

```
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void KernelMin(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMin(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin(x[idx], y[idx]);
    }
    ```
