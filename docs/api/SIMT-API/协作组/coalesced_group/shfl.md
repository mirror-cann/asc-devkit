# shfl

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

`coalesced_group`组内线程的数据交换接口，不通过共享内存实现直接读取组内指定线程的寄存器中的值。

## 函数原型

```c++
template <typename T>
T shfl(T var, int src_rank) const
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| var | 输入 | 线程用于交换的数据。支持的数据类型为：half、int32_t、uint32_t、float、half2、int64_t、uint64_t。 |
| src_rank | 输入 | 期望获取的`var`值所在的线程在组内的排名。当`src_rank`大于等于组内线程数时，获取`src_rank % num_threads()`对应rank线程的`var`值。 |

## 返回值说明

`coalesced_group`组内指定线程输入的`var`值。

## 约束说明

无

## 调用示例

示例代码中的条件分支将一个warp中所有线程id是偶数的线程组成`coalesced_group`协作组，组内各线程shfl接口返回结果如下图所示。

**图 1**  shfl结果示意图  
![](../../../figures/coalesced_group_shfl.png)

- SIMT编程场景：

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```
