# Unsqueeze<a name="ZH-CN_TOPIC_0000002009161778"></a>

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

以dstReg为源操作数和目的操作数，根据mask进行解压缩。解压缩方式：dstReg中第0个元素置为0，dstReg中的第i个元素等于mask中从第0个到第\(i-1\)个元素中1的数量。mask最高位被忽略不参与统计。以uint8\_t类型为例，算法逻辑表示如下：

```
dstReg[0] = 0;
for(int i = 1; i < AscendC::GetVecLen() / sizeof(uint8_t); i++){
    dstReg[i] = mask[i - 1] ? (dstReg[i - 1] + 1) : dstReg[i - 1];
}
```

其中AscendC::GetVecLen\(\) / sizeof\(uint8\_t\)为dstReg包含的元素个数。

## 定义原型<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Unsqueeze(U& dstReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.970000000000002%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="81.03%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.970000000000002%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="81.03%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>目的操作数的数据类型。</p>
<p id="p1474319251999"><a name="p1474319251999"></a><a name="p1474319251999"></a><span id="ph191588264914"><a name="ph191588264914"></a><a name="ph191588264914"></a>Ascend 950PR/Ascend 950DT</span>，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。</p>
</td>
</tr>
<tr id="row2013785594119"><td class="cellrowborder" valign="top" width="18.970000000000002%" headers="mcps1.2.3.1.1 "><p id="p141384557419"><a name="p141384557419"></a><a name="p141384557419"></a>U</p>
</td>
<td class="cellrowborder" valign="top" width="81.03%" headers="mcps1.2.3.1.2 "><p id="p1513845554113"><a name="p1513845554113"></a><a name="p1513845554113"></a>目的操作数的RegTensor类型，由编译器自动推导，用户不需要填写。</p>
</td>
</tr>
</tbody>
</table>

**表2**  函数参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| dstReg | 输入/输出 | 源操作数和目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| mask | 输入 | mask用于提供dstReg解压缩信息。<br>类型为[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明

无

## 数据类型

支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。

## 约束说明<a name="section177921451558"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void UnsqueezeVF(__ubuf__ T* dstAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Unsqueeze(dstReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
