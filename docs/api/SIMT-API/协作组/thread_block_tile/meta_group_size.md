# meta_group_size

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

获取当前线程所在的协作组其直接父组被划分时创建的子组的数量。

## 函数原型

```c++
unsigned long long meta_group_size() const
```

## 参数说明

无

## 返回值说明

直接父组被划分时创建的子组的数量。

## 约束说明

无

## 调用示例

每个线程块中有1024个线程，以4个线程为一小组划分，总共可划分出256个子组。

- SIMT编程场景：

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned long long group_size = tile4.meta_group_size();    // 返回256
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
        unsigned long long group_size = tile4.meta_group_size();    // 返回256
        ...
    }
    ```