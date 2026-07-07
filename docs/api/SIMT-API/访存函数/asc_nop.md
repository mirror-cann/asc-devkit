# asc\_nop

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

本接口的功能为生成一条空操作指令，占用当前线程15个时钟周期，但不执行任何实际计算与访存操作。该接口主要用于避免多个线程读写同地址情况下可能发生的卡死问题。

当不同核连续读取相同GM地址时，会造成总线被长期占用，此时会影响其它访问该地址的请求，进而引发卡死现象。在此场景下，建议在读指令后调用此接口来释放总线。

## 函数原型

```
inline void asc_nop()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/device\_functions.h"头文件。

```
#include "simt_api/device_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```
    __global__ __launch_bounds__(1024) void kernel_nop(int32_t* flag)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        ...
        int32_t warp_num = blockDim.x / warpSize;
        // 连续读取同地址场景：循环等待其他线程将flag置为1
        while (*reinterpret_cast<volatile int32_t*>(flag) == 0) {
            for (int i = 0; i < 800 * warp_num / 15; i++) {
                asc_nop(); // 读同地址后插入空操作指令，释放总线避免卡死，建议指令数量为（800 * warp_num / 15）条
            }
        }
        ...
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_nop(__gm__ int32_t* flag)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        ...
        int32_t warp_num = blockDim.x / warpSize;
        // 连续读取同地址场景：循环等待其他线程将flag置为1
        while (*reinterpret_cast<volatile __gm__ int32_t*>(flag) == 0) {
            for (int i = 0; i < 800 * warp_num / 15; i++) {
                asc_nop(); // 读同地址后插入空操作指令，释放总线避免卡死，建议指令数量为（800 * warp_num / 15）条
            }
        }
        ...
    }
    ```