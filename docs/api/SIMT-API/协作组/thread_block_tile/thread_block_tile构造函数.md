# thread_block_tile构造函数

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

`thread_block_tile`不提供默认构造函数，可通过`tiled_partition`接口从另一个协作组中划分得到。

## 函数原型

```c++
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

## 参数说明

**表 1**  模板版本参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 被划分的父组，类型只能是`thread_block`或`thread_block_tile`。 |
| Size | 输入 | 模板参数，指定划分出的`thread_block_tile`组大小。 |

## 返回值说明

返回划分后当前线程所属的`thread_block_tile`组。

## 约束说明

- `Size`必须是$2^n$，并且必须小于等于32（warpSize），当前可选值范围：1、2、4、8、16、32。

## 调用示例

- SIMT编程场景：

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // 按照32个线程为一组划分thread_block
        auto tile32_auto = tiled_partition<32>(block);                          // 建议使用auto管理返回对象
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // 按照4个线程为一组划分thread_block，对象类型中保留父组信息
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
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // 按照32个线程为一组划分thread_block
        auto tile32_auto = tiled_partition<32>(block);                          // 建议使用auto管理返回对象
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // 按照4个线程为一组划分thread_block，对象类型中保留父组信息
        ...
    }
    ```
