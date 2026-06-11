# thread_rank

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取当前线程在所属的`thread_block_tile`组内的排名，排名从0开始。

## 函数原型

```c++
unsigned long long thread_rank() const
```

## 参数说明

无

## 返回值说明

当前线程在所属的`thread_block_tile`组内的排名。

## 约束说明

无

## 调用示例

示例代码中以4个线程为一组划分线程块，各线程在所属的`thread_block_tile`组内的排名如下图所示。

**图 1**   thread_rank接口返回值示意图  
![](../../../figures/thread_block_tile_rank.png "thread_block_tile_rank")

- SIMT编程场景：

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned long long rank = tile4.thread_rank();
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned long long rank = tile4.thread_rank();
        ...
    }
    ```
