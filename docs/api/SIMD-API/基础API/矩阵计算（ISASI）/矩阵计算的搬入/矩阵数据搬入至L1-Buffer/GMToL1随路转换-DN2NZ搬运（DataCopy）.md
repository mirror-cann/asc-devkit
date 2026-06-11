# GMToL1随路转换-DN2NZ搬运（DataCopy）<a id="ZH-CN_TOPIC_0000002563847574"></a>

## 产品支持情况<a id="section1550532418810"></a>

|产品|是否支持|
|----------|:----------:|
|Ascend 950PR/Ascend 950DT|√|
|Atlas A3 训练系列产品/Atlas A3 推理系列产品|x|
|Atlas A2 训练系列产品/Atlas A2 推理系列产品|x|
|Atlas 200I/500 A2 推理产品|x|
|Atlas 推理系列产品AI Core|x|
|Atlas 推理系列产品Vector Core|x|
|Atlas 训练系列产品|x|
|Kirin X90|x|
|Kirin 9030|x|

## 功能说明<a id="section474617392321"></a>

头文件路径为："basic_api/kernel_operator_data_copy_intf.h"。

随路格式转换数据搬运，适用于在搬运时进行DN到NZ格式的转换。数据从Global Memory搬运至L1 Buffer（TPosition为A1/B1），搬运过程中完成DN->NZ格式转换。

## 函数原型<a id="section1954364615315"></a>

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
```

## 参数说明<a id="section622mcpsimp"></a>

**表 1**  模板参数说明

|参数名|描述|
|----------|----------|
|T|源操作数或者目的操作数的数据类型。|
|enableSmallC0|SmallC0模式开关：当dValue小于等于4的时候，C0_SIZE会补齐到4 * sizeof(T)字节，默认不开启。|

**表 2**  参数说明

|参数名称|输入/输出|含义|
|----------|----------|----------|
|dst|输出|目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为A1/B1）。|
|src|输入|源操作数，类型为GlobalTensor，存储位置为Global Memory。|
|intriParams|输入|搬运参数，Dn2NzParams类型，具体参数说明请参考[表3](#table9182515919)。|

**表 3**  Dn2NzParams结构体参数定义<a id="table9182515919"></a>

|参数名称|含义|
|----------|----------|
|dnNum|传输DN矩阵的数目，取值范围：dnNum∈[0, 4095]。<br>**注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**|
|nValue|DN矩阵的列数，取值范围：nValue∈[0, 16384]。<br>**注：nValue=0表示不执行搬运，该接口将被视为NOP（空操作）。**|
|dValue|DN矩阵的行数，取值范围：dValue∈[0, 2^21-1]。<br>**注：dValue=0表示不执行搬运，该接口将被视为NOP（空操作）。**|
|srcDnMatrixStride|源操作数相邻DN矩阵起始地址间的偏移，取值范围：srcDnMatrixStride∈[0, 2^40-1]，单位为元素。|
|srcDValue|源操作数同一DN矩阵的相邻行起始地址间的偏移，取值范围：srcDValue∈[1, 2^40-1]，单位为元素。|
|dstNzC0Stride|DN转换到NZ格式后，源操作数中的一列会转换为目的操作数的多行。dstNzC0Stride表示，目的NZ矩阵中，来自源操作数同一列的多行数据相邻行起始地址间的偏移，取值范围：dstNzC0Stride∈[1, 65535]，单位：C0_SIZE（32字节）。|
|dstNzNStride|目的NZ矩阵中，Z型矩阵相邻行起始地址之间的偏移。取值范围：dstNzNStride∈[1, 65535]，单位：C0_SIZE（32字节）。|
|dstNzMatrixStride|目的NZ矩阵中，相邻NZ矩阵起始地址间的偏移，取值范围：dstNzMatrixStride∈[0, 65535]，单位为元素。|

DN2NZ转换示意图如下，样例中参数设置值和解释说明如下（以half数据类型为例）：

- dnNum = 2，表示搬运的ND矩阵的数量为2。
- nValue = 8，一个DN矩阵的列数。
- dValue = 23，一个DN矩阵的行数。当dValue \* sizeof\(T\)不满足32字节对齐时，在目的矩阵中会补0对齐到32字节。
- srcDnMatrixStride  = 500，表示相邻DN矩阵起始地址的偏移，每行16个元素，共25行，偏移为16 \* 25= 400个元素。
- srcDValue = 16，表示源矩阵一行的所含元素个数，即16个half数据类型的元素。
- dstNzC0Stride = 37，NZ矩阵中相邻Z矩阵起始地址的偏移，偏移为37个C0\_SIZE。
- dstNzNStride = 2，表示NZ矩阵中相邻行起始地址的偏移。
- dstNzMatrixStride = 320，相邻NZ矩阵起始地址的偏移，每行16个元素，共20行，偏移为16 \* 20 = 320个元素。

![](../../../../../figures/gm2l1_dn2nz.png)

## 数据类型<a id="section4219135304818"></a>

支持的数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、int16_t、uint16_t、int32_t、uint32_t、half、bfloat16_t、float。

## 返回值说明

无

## 约束说明

无

## 调用示例<a id="section122101199486"></a>

示例代码片段如下：

```cpp
// dstLocal：存放DataCopy的输出Tensor，仅支持L1 Buffer（A1/B1）
// srcGlobal：存放DataCopy的输入Tensor，仅支持Global Memory

AscendC::Dn2NzParams dn2nzParams(
    /* dnNum             */ 1,
    /* nValue            */ 32,
    /* dValue            */ 32,
    /* srcDnMatrixStride */ 0,
    /* srcDValue         */ 32,
    /* dstNzC0Stride     */ 32,
    /* dstNzNStride      */ 1,
    /* dstNzMatrixStride */ 0
);
// 将GM中DN的格式的数据，按照dn2nzParams定义的规则，转换为NZ并拷贝到L1 Buffer（A1/B1）中
AscendC::DataCopy(dstLocal, srcGlobal, dn2nzParams);
```
