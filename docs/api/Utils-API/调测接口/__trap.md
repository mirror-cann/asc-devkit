# \_\_trap

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持
- Kirin X90：不支持
- Kirin 9030：不支持

## 功能说明

在SIMT VF实现代码中调用此接口会中断算子的运行，适用于Kernel侧异常场景的调试。

## 函数原型

```
__simt_callee__ inline void __trap()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"utils/debug/asc\_assert.h"头文件。

```
#include "utils/debug/asc_assert.h"
```

## 调用示例

```
__simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ bool* dst, __gm__ float* x)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (isnan(x[idx])) {
        __trap();
    }
    dst[idx] = x[idx];
}
```

