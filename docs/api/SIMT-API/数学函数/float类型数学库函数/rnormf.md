# rnormf

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取输入数据a中前n个元素的平方和a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根的倒数。

![](../../../figures/zh-cn_formulaimage_0000002516816371.png)

## 函数原型

```
inline float rnormf(int n, float* a)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| n | 输入 | 源操作数。输入数据a中连续计算的元素个数。 |
| a | 输入 | 源操作数。Unified Buffer、Global Memory或栈空间的地址。 |

## 返回值说明

a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根的倒数。

-   若a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根超出float最大范围，返回值为0。
-   若a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2平方根的倒数超出float最大范围，返回值为inf。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]都为0，返回值为inf。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]\]任意一个或多个为±inf，返回值为0。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]任意一个或多个为nan同时不是±inf，返回值为nan。
-   若n小于1，返回a\[0\]绝对值的倒数，若a\[0\]为0，返回值为inf。

## 约束说明

-   输入数据a的长度必须大于等于参数n。
-   若n过大，接口性能无法保证。

## 需要包含的头文件

使用该接口需要包含"simt\_api/math\_functions.h"头文件。

```
#include "simt_api/math_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void KernelRnorm(float* dst, int* n, float* a)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = rnormf(n[idx], a);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间，栈空间无需添加地址空间限定符。

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelRnorm(__gm__ float* dst, __gm__ int* n, __gm__ float* a)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = rnormf(n[idx], a);
    }
    ```

