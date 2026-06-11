# L1ToFixpipe-Buffer数据搬运<a id="ZH-CN_TOPIC_0000002569070951"></a>

## 产品支持情况<a id="zh-cn_topic_0000002538001510_section18204144912492"></a>

|产品|是否支持|
|----------|:----------:|
|Ascend 950PR/Ascend 950DT|√|
|Atlas A3 训练系列产品/Atlas A3 推理系列产品|√|
|Atlas A2 训练系列产品/Atlas A2 推理系列产品|√|
|Atlas 200I/500 A2 推理产品|x|
|Atlas 推理系列产品AI Core|x|
|Atlas 推理系列产品Vector Core|x|
|Atlas 训练系列产品|x|
|Kirin X90|x|
|Kirin 9030|x|

## 功能说明<a id="zh-cn_topic_0000002538001510_section618mcpsimp"></a>

头文件路径为："basic_api/kernel_operator_data_copy_intf.h"

DataCopy数据搬运支持将随路量化参数从L1 Buffer移动到Fixpipe Buffer。Fixpipe Buffer包含两种参数：pre_stage的量化参数以及pre_stage的ReLU参数(AscendC待实现)，这些参数各自独立存储，并拥有独立的地址空间。量化参数和ReLU参数通过dst地址的高16位（dst[31:16]）进行区分，具体如下：

**图 1**  Fixpipe Buffer图示

![](../../../../../figures/fixpipe_buffer_02.png)

## 函数原型<a id="section_function_prototype"></a>

```cpp
// 连续搬运场景
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// 连续或非连续搬运场景（高维切分）
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## 参数说明<a id="section_param_desc"></a>

**表 1**  模板参数说明

|参数名|描述|
|----------|----------|
|T|操作数的数据类型。对于pre_stage的量化参数的搬运需要设置为uint64_t。支持的数据类型请参考[数据类型](#section_data_type)。|

**表 2**  参数说明

|参数名|输入/输出|含义|
|----------|----------|----------|
|dst|输出|目的操作数，类型为LocalTensor。存储位置为Fixpipe Buffer，目的地址需要128字节对齐。|
|src|输入|源操作数，类型为LocalTensor，存储位置为L1 Buffer，源地址需要32字节对齐。|
|repeatParams|输入|搬运参数，DataCopyParams类型。通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。<br>具体定义请参考"basic_api/interface/kernel_struct_data_copy.h"|
|count|输入|参与搬运的元素个数。count * sizeof(T) 需要32字节对齐，若未对齐，则搬运量会向下取整到32字节对齐。|

**表 3**  DataCopyParams结构体参数定义

|参数名|含义|
|----------|----------|
|blockCount|待搬运的连续传输数据块个数。uint16_t类型，取值范围：blockCount∈[1, 4095]。<br>**注：blockCount = 0表示不执行搬运，该接口将被视为NOP（空操作）。**|
|blockLen|待搬运的每个连续传输数据块长度，dst位于Fixpipe Buffer时，单位为128字节。uint16_t类型，取值范围：blockLen∈[1, 65535]。<br>**注：blockLen = 0表示不执行搬运，该接口将被视为NOP（空操作）。**|
|srcGap|在L1 Buffer -> Fixpipe Buffer场景中，srcGap特指源操作数相邻连续数据块的间隔（前面一个数据块的头与后面数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，srcGap ∈[0, 2^16-1]。<br>&bull; blockCount = 1时，srcGap无意义，设置为0即可。<br>&bull; blockCount ≠ 1时，当srcGap=0时，表示重复搬出源操作数的第一个数据块。|
|dstGap|在L1 Buffer -> Fixpipe Buffer场景中，dstGap特指目的操作数相邻连续数据块的间隔（前面一个数据块的头与后面数据块的头的间隔），单位为DataBlock（128字节）。uint16_t类型，dstGap不要超出该数据类型的取值范围，dstGap  ∈[1, 2^16-1]。<br>&bull; blockCount = 1时，dstGap无意义，设置为0即可。<br>&bull; blockCount ≠ 1时，当dstGap=0时，表示目的矩阵中搬入的每个数据块都会覆盖第一个数据块。|

## 数据类型<a id="section_data_type"></a>

src数据类型支持uint64_t，dst数据类型支持uint64_t。

## 返回值说明

无

## 约束说明<a id="section_constraint"></a>

- dst物理内存为Fixpipe Buffer起始地址要求128字节对齐，src物理内存为L1 Buffer，起始地址要求32字节对齐。
- blockCount取值范围：blockCount∈[1, 4095]，设置为0表示不执行，此指令将被视为NOP并报告警告。
- srcGap∈[0, 2^16-1]，dstGap∈[1, 2^16-1]。
- 如果读写地址大于对应的mem_block，地址将被绕行并引发警告。
- 如果Fixpipe Buffer中的目标dst地址溢出，将引发异常。

## 调用示例<a id="section_call_example"></a>

无
