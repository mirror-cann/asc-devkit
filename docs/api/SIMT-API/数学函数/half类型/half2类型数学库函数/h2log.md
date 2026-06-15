# h2log

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取以e为底，输入数据各元素的对数。

![](../../../../figures/zh-cn_formulaimage_0000002513334400.png)

## 函数原型

```
inline half2 h2log(half2 x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

以e为底的输入数据各元素的对数。

-   当输入元素小于0或为nan时，返回值为nan。
-   当输入元素为0时，返回值为-inf。
-   当输入元素为inf时，返回值为inf。

## 约束说明

无

## 需要包含的头文件

使用half2类型接口需要包含"simt\_api/asc\_fp16.h"头文件。

```
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void KernelLog(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelLog(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log(x[idx]);
    }
    ```
