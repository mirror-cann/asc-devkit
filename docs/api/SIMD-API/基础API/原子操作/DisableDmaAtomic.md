# DisableDmaAtomic<a name="ZH-CN_TOPIC_0000001839732381"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
|------|----------|
| <cann-filter npu-type="950"><term>Ascend 950PR/Ascend 950DT</term> | √</cann-filter> |
| <cann-filter npu-type="A3"><term>Atlas A3 训练系列产品/Atlas A3 推理系列产品</term> | √</cann-filter> |
| <cann-filter npu-type="910b"><term>Atlas A2 训练系列产品/Atlas A2 推理系列产品</term> | √</cann-filter> |
| <cann-filter npu-type="310b"><term>Atlas 200I/500 A2 推理产品</term> | √</cann-filter> |
| <cann-filter npu-type="310p"><term>Atlas 推理系列产品AI Core</term> | √</cann-filter> |
| <cann-filter npu-type="310p"><term>Atlas 推理系列产品Vector Core</term> | x</cann-filter> |
| <cann-filter npu-type="910"><term>Atlas 训练系列产品</term> | √</cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

关闭数据搬运随路原子操作功能，后续执行数据搬运时，GM中原始数据将被新搬运数据完全覆盖。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void DisableDmaAtomic()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

[SetAtomicAdd](SetAtomicAdd.md)、[SetAtomicMax](SetAtomicMax(ISASI).md)、[SetAtomicMin](SetAtomicMin(ISASI).md)使用完成后，通过DisableDmaAtomic清空原子操作的状态，以免影响后续相关指令功能。

## 调用示例<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// 开启原子累加，将UB数据原子累加到GM
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// 关闭原子累加
AscendC::DisableDmaAtomic();
```

完整样例请参考[DataMovementWithAtomicOperations样例](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/02_features/01_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc)。
