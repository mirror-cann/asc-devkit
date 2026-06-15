# DataCachePreload<a name="ZH-CN_TOPIC_0000001788032974"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持  |
| :----------------------- | :------: |
|<cann-filter npu-type="950"> Ascend 950PR/Ascend 950DT | √ </cann-filter>|
|<cann-filter npu-type="A3"> Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="910b"> Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="310b"> Atlas 200I/500 A2 推理产品 | √ </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品AI Core | √ </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品Vector Core | x </cann-filter>|
|<cann-filter npu-type="910"> Atlas 训练系列产品 | x </cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_cache_intf.h"`。

从源地址所在的特定GM地址预加载数据到DCache中，每次调用只能预加载一个Cache Line大小的数据。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void DataCachePreload(const GlobalTensor<uint64_t>& src, const T cacheOffset)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| src | 输入 | 源操作数，类型为GlobalTensor。支持的数据类型为：uint64_t。 |
| cacheOffset | 输入 | 在源操作数上偏移cacheOffset大小开始加载数据，单位为byte，支持的数据类型为：int16_t/int64_t。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

频繁调用此接口可能导致保留站（Reservation Station，用于暂存待执行指令的硬件队列）发生拥塞。在此情况下，该指令将被视为NOP指令，进而阻塞Scalar流水线。因此，建议连续调用此接口的次数不要超过4次。

## 调用示例<a name="section837496171220"></a>

```cpp
AscendC::GlobalTensor<uint64_t> srcGlobal;
int64_t cacheOffset = 0;
AscendC::DataCachePreload(srcGlobal, cacheOffset);
