# sincosf

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取输入数据的三角函数正弦值和余弦值。

![](../../../figures/zh-cn_formulaimage_0000002516816357.png)

## 函数原型

```
inline void sincosf(float x, float *s, float *c)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| s | 输出 | Unified Buffer、Global Memory或栈空间的地址，用于存储输入数据的三角函数正弦值。 |
| c | 输出 | Unified Buffer、Global Memory或栈空间的地址，用于存储输入数据的三角函数余弦值。 |

## 返回值说明

-   当输入x为inf、-inf、nan时，输出值为nan。

## 约束说明

使用本接口时，线程配置最大不超过1024，否则有栈溢出风险。

## 需要包含的头文件

使用该接口需要包含"simt\_api/math\_functions.h"头文件。

```
#include "simt_api/math_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void KernelSinCos(float* s, float* c, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        sincosf(x[idx], s + idx, c + idx); // 对源地址的第idx个元素取三角函数正弦值和余弦值
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间，栈空间无需添加地址空间限定符。

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSinCos(__gm__ float* s, __gm__ float* c, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        sincosf(x[idx], s + idx, c + idx); // 对源地址的第idx个元素取三角函数正弦值和余弦值
    }
    ```

