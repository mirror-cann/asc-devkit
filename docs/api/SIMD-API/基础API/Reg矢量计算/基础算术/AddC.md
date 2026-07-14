# AddC<a name="ZH-CN_TOPIC_0000002008389964"></a>

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

根据mask对输入数据srcReg0、srcReg1以及进位数据carrySrc进行按元素相加操作，将结果写入dstReg。如果srcReg0, srcReg1输入转换为uint32\_t类型，加上进位值carrySrc相加时超出uint32\_t最大值，在MaskReg carry中对应位置每4bit写1，否则写0。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002535717372.png)

对carry的操作示例如下：

-   int32\_t类型，a\_i, b\_i∈\[-2147483648, 2147483647\]
    -   假设a\_i = -2147483648, b\_i = -2, carrySrc\_i = 1

        \(uint32\_t\)a\_i + \(uint32\_t\)b\_i + \(uint32\_t\)carrySrc\_i = \(uint64\_t\)uint\_dst\_i

        因为uint\_dst\_i \>\> 32 大于0，所以carry\[\(i%64\):4\*\(i%64\)\] = 1

    -   假设a\_i = 2, b\_i = 5, carrySrc\_i = 1

        \(uint32\_t\)a\_i + \(uint32\_t\)b\_i + \(uint32\_t\)carrySrc\_i = \(uint64\_t\)uint\_dst\_i

        因为uint\_dst\_i \>\> 32 等于0，所以carry\[\(i%64\):4\*\(i%64\)\] = 0

-   uint32\_t类型，a\_i, b\_i∈\[0, 4294967295\]
    -   假设a\_i = 4294967295, b\_i = 2, carrySrc\_i = 0

        \(uint32\_t\)a\_i + \(uint32\_t\)b\_i + \(uint32\_t\)carrySrc\_i = \(uint64\_t\)uint\_dst\_i

        因为uint\_dst\_i \>\> 32 大于0，所以carry\[\(i%64\):4\*\(i%64\)\] = 1

    -   假设a\_i = 3, b\_i = 2, carrySrc\_i = 0

        \(uint32\_t\)a\_i + \(uint32\_t\)b\_i + \(uint32\_t\)carrySrc\_i = \(uint64\_t\)uint\_dst\_i

        因为uint\_dst\_i \>\> 32 等于0，所以carry\[\(i%64\):4\*\(i%64\)\] = 0

## 函数原型<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.56%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="81.44%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.56%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="81.44%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>操作数数据类型。</p>
<p id="p1696610389284"><a name="p1696610389284"></a><a name="p1696610389284"></a><span id="ph9966153816285"><a name="ph9966153816285"></a><a name="ph9966153816285"></a>Ascend 950PR/Ascend 950DT</span>，支持的数据类型为：uint32_t/int32_t。</p>
</td>
</tr>
<tr id="row103041482169"><td class="cellrowborder" valign="top" width="18.56%" headers="mcps1.2.3.1.1 "><p id="p430416488162"><a name="p430416488162"></a><a name="p430416488162"></a>U</p>
</td>
<td class="cellrowborder" valign="top" width="81.44%" headers="mcps1.2.3.1.2 "><p id="p1330454851613"><a name="p1330454851613"></a><a name="p1330454851613"></a>操作数RegTensor类型， 例如RegTensor&lt;uint32_t&gt;，由编译器自动推导，用户不需要填写。</p>
</td>
</tr>
</tbody>
</table>

**表2**  参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="10.040000000000001%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="71.41999999999999%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1685815326526"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1085873217524"><a name="p1085873217524"></a><a name="p1085873217524"></a>carry</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p18621102165312"><a name="p18621102165312"></a><a name="p18621102165312"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p682115465311"><a name="p682115465311"></a><a name="p682115465311"></a>目的操作数</p>
<p id="p11809617145318"><a name="p11809617145318"></a><a name="p11809617145318"></a>类型为<a href="../寄存器数据类型/MaskReg.md">MaskReg</a>。</p>
</td>
</tr>
<tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p108051250181214"><a name="p108051250181214"></a><a name="p108051250181214"></a>dstReg</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>目的操作数。</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
</td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg0</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>源操作数。</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>srcReg0、srcReg1和carrySrc的数据类型需要与目的操作数保持一致。</p>
</td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>srcReg1</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p151325484342"><a name="p151325484342"></a><a name="p151325484342"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p181326485341"><a name="p181326485341"></a><a name="p181326485341"></a>源操作数。</p>
<p id="p813204817342"><a name="p813204817342"></a><a name="p813204817342"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
<p id="p12132154853419"><a name="p12132154853419"></a><a name="p12132154853419"></a>srcReg0、srcReg1和carrySrc的数据类型需要与目的操作数保持一致。</p>
</td>
</tr>
<tr id="row574413121111"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p14751813191114"><a name="p14751813191114"></a><a name="p14751813191114"></a>carrySrc</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p137521315114"><a name="p137521315114"></a><a name="p137521315114"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p075513101113"><a name="p075513101113"></a><a name="p075513101113"></a>源操作数。输入进位值。</p>
<p id="p1149514185128"><a name="p1149514185128"></a><a name="p1149514185128"></a>类型为<a href="../寄存器数据类型/MaskReg.md">MaskReg</a>。</p>
<p id="p0912849161314"><a name="p0912849161314"></a><a name="p0912849161314"></a>srcReg0、srcReg1和carrySrc的数据类型需要与目的操作数保持一致。</p>
<p id="p15433203914273"><a name="p15433203914273"></a><a name="p15433203914273"></a><span id="ph643353912711"><a name="ph643353912711"></a><a name="ph643353912711"></a>Ascend 950PR/Ascend 950DT</span>，支持的数据类型为：uint32_t/int32_t。</p>
</td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p>
</td>
<td class="cellrowborder" valign="top" width="10.040000000000001%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="71.41999999999999%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>源操作数元素操作的有效指示，详细说明请参考<a href="../寄存器数据类型/MaskReg.md">MaskReg</a>。</span></p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

```
template <typename T>
static __simd_vf__ inline void AddVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t repeatTimes, uint32_t oneRepeatSize){
    
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg0;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg carry = AscendC::Reg::CreateMask<uint8_t>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(carry, dstReg0, srcReg0, srcReg1, mask);
        // 8*4B=32B align
        AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>((__ubuf__ uint32_t*)dst1Addr + i * 8, carry);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
    }
}
```

