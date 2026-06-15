# \_\_hsub

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

计算两个half类型数据相减的结果，并遵循CAST\_RINT模式对结果进行舍入处理。

## 函数原型

```
half __hsub(const half x, const half y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

输入数据相减的结果。本接口受全局饱和模式影响，特殊值如下：

| x值 | y值 | 非饱和模式返回值 | 饱和模式返回值 |
| --- | --- | --- | --- |
| ±inf | 有限值 | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
| 有限值 | ±inf | ∓inf | ∓ASCRT_MAX_NORMAL_FP16 |
| ±inf | ±inf | nan | 0 |
| ±inf | ∓inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
| 有限值（包括±0） | -x | +0 | +0 |
| x，y任意一个为nan |  x，y任意一个为nan |nan | 0 |  |

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
    __global__ __launch_bounds__(1024) void KernelHsub(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHsub(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```
