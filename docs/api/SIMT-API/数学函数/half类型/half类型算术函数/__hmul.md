# \_\_hmul

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

计算两个half类型数据相乘的结果，并遵循CAST\_RINT模式对结果进行舍入处理。

## 函数原型

```
half __hmul(const half x, const half y)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

输入数据相乘的结果。

-   当输入和结果都不为nan时，x\*y的符号为x和y符号的异或。
-   \_\_hmul\(x, y\)等价于\_\_hmul\(y, x\)。
-   本接口受全局饱和模式影响，特殊值如下：

    | x值 | y值 | 非饱和模式返回值 | 饱和模式返回值 |
    | --- | --- | --- | --- |
    | 非0值 | ±inf | 符号由x和y的符号异或决定，值为inf | 符号由x和y的符号异或决定，值为ASCRT_MAX_NORMAL_FP16 |
    | ±0 | ±inf | nan | 0 |
    | ±0 | 有限值 | 返回值符号由x和y的符号异或决定，值为0 | 返回值符号由x和y的符号异或决定，值为0 |
    | ±inf | ∓inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
    | x，y任意一个为nan |  x，y任意一个为nan |nan | 0 |

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
    __global__ __launch_bounds__(1024) void KernelHmul(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHmul(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```
