# UBToUB连续数据搬运(DataCopy)<a name="ZH-CN_TOPIC_0000002382908021"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
| :--- | :---: |
|<cann-filter npu-type="950"> Ascend 950PR/Ascend 950DT | √ </cann-filter>|
|<cann-filter npu-type="A3"> Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="910b"> Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="310b"> Atlas 200I/500 A2 推理产品 | x </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品AI Core | √ </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品Vector Core | x </cann-filter>|
|<cann-filter npu-type="910"> Atlas 训练系列产品 | √ </cann-filter>|
|<cann-filter npu-type="x90"> Kirin X90 | x </cann-filter>|
|<cann-filter npu-type="9030"> Kirin 9030 | x </cann-filter>|

## 功能说明<a name="section474617392321"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

支持Unified Buffer与Unified Buffer之间的连续数据搬运，数据在传输过程中保持原始格式和内容不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../../数据结构/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

## 函数原型<a name="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 操作数的数据类型。支持的数据类型请参考[数据类型](#section4219135304818)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数。类型为[LocalTensor](../../../数据结构/LocalTensor和GlobalTensor定义/LocalTensor/LocalTensor简介.md)，存储位置为Unified Buffer，目的地址需要32字节对齐。 |
| src | 输入 | 源操作数。类型为LocalTensor，存储位置为Unified Buffer，源地址需要32字节对齐。 |
| count | 输入 | 参与搬运的元素个数。<br>说明：count * sizeof(T)需要32字节对齐，若不对齐，搬运量将对32字节做向下取整。 |

以half数据类型为例，源操作数的shape为1 \* 128。当count = 128时，[图1](#zh-cn_topic_0000002567699425_fig13481537531)将源操作数中128个元素连续搬运至目的操作数。

**图 1**  连续搬运示意图<a name="zh-cn_topic_0000002567699425_fig13481537531"></a>  
![](../../../../../figures/continuous_data_copy_diagram.png "连续搬运示意图")

## 数据类型<a name="section4219135304818"></a>

源操作数和目的操作数支持的数据类型保持一致，具体如下：

<cann-filter npu-type="950">

- Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

</cann-filter>

<cann-filter npu-type="A3">

- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

<cann-filter npu-type="910b">

- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

<cann-filter npu-type="310p">

- Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

<cann-filter npu-type="910">

- Atlas 训练系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

## 返回值说明<a name="section44801012174220"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 位于Unified Buffer的地址必须32字节对齐。
- 调用连续搬运接口时，count \* sizeof\(T\)需要32字节对齐，若未对齐，则搬运量会向下取整到32字节对齐。
<cann-filter npu-type="910b,A3">

- 针对如下产品型号：
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品

    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过`npu-smi info -t topo`命令查询HCCS物理链路。

</cann-filter>

## 调用示例<a name="section122101199486"></a>

```cpp
// srcLocal、dstLocal为half类型的LocalTensor。
// 使用传入count参数的搬运接口，完成连续搬运。
AscendC::DataCopy(dstLocal, srcLocal, 512);
```

结果示例：

```text
输入数据srcGlobal：[1 2 3 ... 512]
输出数据dstGlobal：[1 2 3 ... 512]
```
