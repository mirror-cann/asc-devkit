# \_\_hfma\_relu

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

对输入half类型数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST\_RINT模式对结果进行舍入。负数结果置为0。

## 函数原型

```
half __hfma_relu(const half x, const half y, const half z)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |
| z | 输入 | 源操作数。 |

## 返回值说明

x \* y + z的值，若结果为负数，置为0。本接口受全局饱和模式影响，特殊值如下：

| x值 | y值 | z值 | 非饱和模式返回值 | 饱和模式返回值 |
| --- | --- | --- | --- | --- |
| ±inf | ±0 | — | nan | 0 |
| ±0 | ±inf | — | nan | 0 |
| x*y = inf | x*y = inf |-inf | nan | 0 |
| x*y = -inf | x*y = -inf |inf | nan | 0 |
| x*y+z超出ASCRT_MAX_NORMAL_FP16 | - | - | inf | ASCRT_MAX_NORMAL_FP16 |
| x*y+z小于ASCRT_MIN_NORMAL_FP16 | - | - | 0 | 0 |
| x、y、z任意一个为nan |  x、y、z任意一个为nan | x、y、z任意一个为nan |nan | 0 |  |  |

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
    __global__ __launch_bounds__(1024) void KernelHfma_relu(half* dst, half* x, half* y, half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHfma_relu(__gm__ half* dst, __gm__ half* x, __gm__ half* y, __gm__ half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```
