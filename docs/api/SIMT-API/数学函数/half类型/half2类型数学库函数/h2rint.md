# h2rint

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取与输入数据各元素最接近的整数，若存在两个同样接近的整数，则取其中的偶数。

## 函数原型

```
inline half2 h2rint(half2 x)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

与输入各元素最接近的整数值。特别场景说明如下：

-   当输入元素为0时，返回值为0。
-   当输入元素为0.5时，返回值为0。
-   当输入元素为1.5时，返回值为2。
-   当输入元素为nan时，返回值为nan。
-   当输入元素为inf时，返回值为inf。
-   当输入元素为-inf时，返回值为-inf。

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
    __global__ __launch_bounds__(1024) void KernelRint(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rint(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelRint(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rint(x[idx]);
    }
    ```

