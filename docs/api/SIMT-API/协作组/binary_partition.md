# binary_partition

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

`binary_partition` API用于根据一个标签（0或1）将父组划分为两个子组，标签相同的线程会被分配到同一组中。

## 函数原型

```c++
coalesced_group binary_partition(const coalesced_group& g, bool pred)
```

```c++
template <unsigned int Size, typename ParentT>
coalesced_group binary_partition(const thread_block_tile<Size, ParentT>& g, bool pred)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 被划分的父组，类型可以是`coalesced_group`或`thread_block_tile`。 |
| pred | 输入 | 标签，用于划分子组。 |

## 返回值说明

返回划分出的子组`coalesced_group`对象。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(int *inputArr)
    {
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr 中是随机的整数
        int elem = inputArr[block.thread_rank()];
        // 根据 elem&1 是否为 true 将tile32划分为两个子组
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr 中是随机的整数
        int elem = inputArr[block.thread_rank()];
        // 根据 elem&1 是否为 true 将tile32划分为两个子组
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```
