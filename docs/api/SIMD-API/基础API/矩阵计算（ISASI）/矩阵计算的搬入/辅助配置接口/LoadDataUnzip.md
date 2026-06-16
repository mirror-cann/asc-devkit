# LoadDataUnzip<a name="ZH-CN_TOPIC_0000001913015096"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
| ------ | :------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | x </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | √ </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

将GM上的数据解压并搬运到A1/B1/B2上。执行该API前需要执行[LoadUnzipIndex](LoadUnzipIndex.md)加载压缩索引表。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadDataUnzip(const LocalTensor<T>& dst, const GlobalTensor<T>& src)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor，支持的TPosition为A1/B1/B2。<br>LocalTensor的起始地址需要保证：<br>&nbsp;&nbsp;&bull; TPosition为A1/B1时，32字节对齐。<br>&nbsp;&nbsp;&bull; TPosition为B2时，512B对齐。 |
| src | 输入 | 源操作数，类型为GlobalTensor。数据类型需要与dst保持一致。 |

## 数据类型

支持的数据类型为：int8_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section6461234123118"></a>

该调用示例支持的运行平台为Atlas 推理系列产品AI Core。

```cpp
uint32_t srcLen = 896, dstLen = 1024, numOfIndexTabEntry = 1;
AscendC::LocalTensor<int8_t> weightB1 = inQueueB1.AllocTensor<int8_t>();
AscendC::LoadUnzipIndex(indexGlobal, numOfIndexTabEntry); // 加载索引数据，加载GM上的压缩索引表到内部寄存器
AscendC::LoadDataUnzip(weightB1, weGlobal); // 根据内部寄存器里的索引表加载数据
```