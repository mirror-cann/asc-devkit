# Histograms<a name="ZH-CN_TOPIC_0000002079418145"></a>

## 产品支持情况<a name="section1550532418810"></a>

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


## 功能说明<a name="section618mcpsimp"></a>

对直方图数据进行统计，在目的操作数dstReg的基础数据上加上源操作数srcReg数据的统计结果，包括数据的频率统计和累计统计。

-   频率统计

    如下图所示，在低位模式下，dstReg（即dst0）用于统计srcReg中\[0-127\]范围内（前半部分）各个值的出现频率；而在高位模式下，dstReg（即dst1）则统计\[128-255\]范围内（后半部分）的频率。dst0和dst1中的第n位（bit）表示srcReg中数值n的出现次数，并在原始dstReg数据的基础上进行累加。

    **图1**  频率统计<a name="fig1436621813216"></a>  
    ![](../../../../figures/频率统计.png "频率统计")

-   累计统计

    如下图所示，在低位模式下，目的寄存器dstReg（即dst0）会统计源寄存器srcReg中值落在低位区间\[0-127\]的数据分布情况；在高位模式下，目的寄存器dstReg（即dst1）则会统计srcReg中值落在高位区间\[128-255\]的数据分布情况。在dst0 和dst1中，第n位的数据表示srcReg中从0到n的所有数值在对应区间中出现的总频率。最终，统计结果会在目的寄存器原始数据的基础上进行累加。

    **图2**  累计统计<a name="fig117001116340"></a>  
    ![](../../../../figures/累计统计.png "累计统计")

## 函数原型<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U = DefaultType, HistogramsBinType mode, HistogramsType type, typename S, typename V>
__simd_callee__ inline void Histograms(V& dstReg, S& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 源操作数的数据类型。 |
| U | 目的操作数的数据类型。 |
| mode | HistogramsBinType枚举类型，用于选择统计srcReg的低位区间或高位区间。Histograms将uint8_t类型srcReg的[0, 255]取值范围划分为低位、高位两个区间；每个区间包含128个取值，对应uint16_t类型dstReg在一个VL（256Byte）内可保存的128个统计结果。单次调用只统计其中一个区间，并将该区间的统计结果写入传入的dstReg。若需要获得完整[0, 255]范围的统计结果，需要分别以BIN0和BIN1调用两次接口，并使用两个目的寄存器保存结果，例如dst0保存低位区间结果，dst1保存高位区间结果。<br>• BIN0：统计srcReg中[0, 127]范围内的数据，结果写入传入的dstReg。<br>• BIN1：统计srcReg中[128, 255]范围内的数据，结果写入传入的dstReg。 |
| type | HistogramsType 枚举类型，表示统计模式。<br>• FREQUENCY：频率统计模式，统计srcReg中[0, 255]每个数的数量。每个dst有128个元素，其中低位模式dstReg中每个元素对应src中[0, 127]每个元素的累加个数，高位模式dstReg中每个元素对应src中[128,255]每个元素的累加个数。<br>• ACCUMULATE：累计统计模式，统计srcReg中[0, 255]每个数及其之前出现数的数量总和。每个dst有128个元素，其中低位模式dstReg中每个元素对应src中[0, 127]每个元素区间累加个数，高位模式dstReg中每个元素对应src中[128,255]每个元素区间累加个数。 |
| S | 源操作数RegTensor类型，由编译器自动推导，用户不需要填写。 |
| V | 目的操作数RegTensor类型，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p>
</td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>目的操作数。</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph134278176129"><a name="ph134278176129"></a><a name="ph134278176129"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</span></p>
</td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg</p>
</td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>源操作数。</p>
<p id="p157411581277"><a name="p157411581277"></a><a name="p157411581277"></a><span id="ph890017117407"><a name="ph890017117407"></a><a name="ph890017117407"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</span></p>
</td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p>
</td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>源操作数元素操作的有效指示，详细说明请参考<a href="../寄存器数据类型/MaskReg.md">MaskReg</a>。</span></p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 当mask位数为0时，对应位置的src源操作数的数值被忽略，dst对应位置数值为忽略该位置src后计算得到的值。
- dst的数据类型为uint16_t，最大值为65535，使用时需注意累加溢出问题。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T, typename U, AscendC::Reg::HistogramsBinType mode, AscendC::Reg::HistogramsType type>
__simd_vf__ inline void HistogramsVF(__ubuf__ U* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<U> dstReg;
    AscendC::Reg::MaskReg mask0 = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::MaskReg mask1 = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::Duplicate<U>(dstReg, 0);
    for (uint16_t i = 0; i < repeatTimes; ++i){
        AscendC::Reg::LoadAlign(srcReg, srcAddr + oneRepeatSize * i);
        AscendC::Reg::Histograms<T, U, mode, type>(dstReg, srcReg, mask0);
    }
    AscendC::Reg::StoreAlign(dstAddr, dstReg, mask1);
}
```
