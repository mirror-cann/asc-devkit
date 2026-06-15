# WriteGmByPassDCache\(ISASI\)<a name="ZH-CN_TOPIC_0000002327151478"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
| :----------------------- | :------: |
|<cann-filter npu-type="950">Ascend 950PR/Ascend 950DT| √</cann-filter>|
|<cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品| √</cann-filter>|
|<cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品| √</cann-filter>|
|<cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品| x</cann-filter>|
|<cann-filter npu-type="310p">Atlas 推理系列产品AI Core| x</cann-filter>|
|<cann-filter npu-type="310p">Atlas 推理系列产品Vector Core| x</cann-filter>|
|<cann-filter npu-type="910">Atlas 训练系列产品| x</cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_scalar_intf.h"`。

不经过DCache向GM地址上写数据。使用场景：

- 当多个核写入的数据落在同一条Cache Line内时，经过DCache的读写将以64B为粒度，可能引发多核数据随机覆盖问题（参考[DataCacheCleanAndInvalid调用示例3](../缓存控制/DataCacheCleanAndInvalid.md#example3_multi_core)）。使用该接口不经过DCache直接按操作数大小读GM，可避免此问题。
- 经过DCache写GM时可能导致多核间的数据不一致问题（详细原因请参考[Cache写策略与Cache一致性问题](../缓存控制/系统缓存概述.md#zh-cn_topic_0000002583420201_section053731716357)），使用该接口不经过DCache直接向GM写数据，可避免此问题。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 操作数的数据类型。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 含义 |
|--------|-----------|------|
| addr | 输入 | 目标GM地址。 |
| value | 输入 | 待写入目标数据。 |

## 数据类型

支持的数据类型为int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section6191129670"></a>

```cpp
__gm__ int32_t *addr = const_cast<__gm__ int32_t *>(srcGlobal.GetPhyAddr());
int32_t value = AscendC::ReadGmByPassDCache<int32_t>(addr);
// 同步指令：用于阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束。
AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();  // DDR，等待GM访问指令。
AscendC::WriteGmByPassDCache<int32_t>(reinterpret_cast<__gm__ int32_t *>(dstGm), value + ADD_VALUE);
```

完整样例请参考[GmByPassDCache类样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/09_scalar_computation/gm_by_pass_dcache)。
