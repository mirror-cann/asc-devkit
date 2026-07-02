# AtomicMin<a name="ZH-CN_TOPIC_0000002568105162"></a>

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section163431416121118"></a>

头文件路径为：`"basic_api/kernel_operator_atomic_intf.h"`。

该接口用于在指定GM地址上进行原子取最小值操作，将**address**指向的GM地址上的旧值（**old_value**）与输入标量值（**value**）做比较，将较小值（**new_value**）写回GM地址，返回该地址修改前的值（**old_value**）。

计算公式如下：

$$new\_value = Min(old\_value, value)$$

## 函数原型<a name="section1171112415119"></a>

```cpp
template <typename T>
__aicore__ inline T AtomicMin(__gm__ T *address, T value)
```

## 参数说明<a name="section086962912112"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | 输入GM的地址。 |
| value | 输入 | 标量值，支持数据类型和address指向的数据类型一致。 |

## 数据类型<a name="section832119018128"></a>

address、value、返回值的数据类型相同，支持的数据类型为int32_t、uint32_t、float、int64_t、uint64_t。

## 返回值说明<a name="section97313820116"></a>

返回GM地址上做原子操作前的数据。

## 约束说明<a name="section24727811123"></a>

- 在开启编译器自动同步功能（参见[自动同步使用约束说明](../同步控制/核内同步/关键特性说明.md#自动同步使用约束说明)）的前提下，编译器能够自动在PIPE_MTE2/PIPE_MTE3与PIPE_S之间插入同步（参见[编译器自动同步](../同步控制/核内同步/关键特性说明.md#tpipe-tque框架编程范式和编译器自动同步支持情况)）。但是，AtomicMin为标量计算，在读写GM时如果与搬运单元（MTE2/MTE3）存在数据依赖，编译器却无法自动插入同步，开发者需要根据实际情况手动插入同步。
- Scalar原子操作会绕过DCache，需要调用[DataCacheCleanAndInvalid](../缓存控制/DataCacheCleanAndInvalid.md)接口确保GM与DCache的一致性。

## 调用示例<a name="section191505489122"></a>

```cpp
extern "C" __global__ __aicore__ void atomic_min_reduction_kernel(__gm__ int32_t* global_min, __gm__ int32_t* input, int32_t dataSize)
{
    // 当不使用TPipe-TQUE框架编程时，需手动调用InitSocState初始化全局状态寄存器。
    AscendC::InitSocState();

    // 使用LocalMemAllocator在UB上分配内存。
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<int32_t> srcLocal = ubAllocator.Alloc<int32_t>(dataSize);
    AscendC::LocalTensor<int32_t> dstLocal = ubAllocator.Alloc<int32_t>(1);

    // 使用GlobalTensor封装GM输入数据。
    AscendC::GlobalTensor<int32_t> inputGlobal;
    inputGlobal.SetGlobalBuffer(input, dataSize);

    constexpr uint32_t EVENT_ID0 = 0;

    // 将数据从GM拷贝到UB。
    AscendC::DataCopy(srcLocal, inputGlobal, dataSize);

    // 等待MTE2搬运完成后再进行矢量计算。
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    // 使用矢量归约接口[ReduceMin](../Memory矢量计算/归约计算/ReduceMin.md)计算局部最小值，不获取索引。
    // Ascend 950PR/Ascend 950DT无需sharedTmpBuffer，可传入srcLocal。
    AscendC::ReduceMin<int32_t>(dstLocal, srcLocal, srcLocal, dataSize, false);

    // 等待矢量计算完成后再通过标量读取结果。
    AscendC::SetFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_S>(EVENT_ID0);

    // 使用AtomicMin将局部最小值原子地更新到全局最小值中。
    AscendC::AtomicMin(global_min, dstLocal.GetValue(0));
}
```

假设上述函数在多个核上执行，每个核先通过矢量归约接口[ReduceMin](../Memory矢量计算/归约计算/ReduceMin.md)计算各自分片数据的局部最小值，再通过AtomicMin合并到global_min中，最终得到全局最小值。
