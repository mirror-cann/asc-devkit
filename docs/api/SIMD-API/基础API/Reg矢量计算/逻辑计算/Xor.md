# Xor<a name="ZH-CN_TOPIC_0000001956827101"></a>

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

本节介绍两种接口，分别用于对RegTensor和MaskReg进行操作：

-   对RegTensor操作：

    根据mask对输入数据srcReg0、srcReg1按位异或（^）操作，将结果写入dstReg。计算公式如下：

    ![](../../../../figures/zh-cn_formulaimage_0000002499005628.png)

-   对MaskReg操作：

    将两个输入MaskReg的有效bits进行逻辑异或运算得到新的[MaskReg](../寄存器数据类型/MaskReg.md)。

## 函数原型<a name="section620mcpsimp"></a>

-   对RegTensor操作

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Xor(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

-   对MaskReg操作

    ```
    __simd_callee__ inline void Xor(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
    ```

## 参数说明<a name="section622mcpsimp"></a>

-   对RegTensor操作

    **表1**  模板参数说明

    <a name="table1878811121758"></a>
    <table><thead align="left"><tr id="row97885121457"><th class="cellrowborder" valign="top" width="18.2%" id="mcps1.2.3.1.1"><p id="p2078817124511"><a name="p2078817124511"></a><a name="p2078817124511"></a>参数名</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.8%" id="mcps1.2.3.1.2"><p id="p278811214515"><a name="p278811214515"></a><a name="p278811214515"></a>描述</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row167887129511"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p137887124517"><a name="p137887124517"></a><a name="p137887124517"></a>T</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p197882121851"><a name="p197882121851"></a><a name="p197882121851"></a>操作数数据类型。</p>
    <p id="p37882126514"><a name="p37882126514"></a><a name="p37882126514"></a><span id="ph1178812126519"><a name="ph1178812126519"></a><a name="ph1178812126519"></a>Ascend 950PR/Ascend 950DT</span>，支持的数据类型为：bool/uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t</p>
    </td>
    </tr>
    <tr id="row1278818121756"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p87885121156"><a name="p87885121156"></a><a name="p87885121156"></a>mode</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p278817121952"><a name="p278817121952"></a><a name="p278817121952"></a>选择MERGING模式或ZEROING模式。</p>
    <a name="ul978812125510"></a><a name="ul978812125510"></a><ul id="ul978812125510"><li>ZEROING，mask未筛选的元素在dst中置零。</li><li>MERGING，当前不支持。</li></ul>
    </td>
    </tr>
    <tr id="row07893121515"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p27898128518"><a name="p27898128518"></a><a name="p27898128518"></a>U</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p1678910126514"><a name="p1678910126514"></a><a name="p1678910126514"></a>srcReg0/srcReg1/dstReg RegTensor类型， 例如RegTensor&lt;uint32_t&gt;，由编译器自动推导，用户不需要填写。</p>
    </td>
    </tr>
    </tbody>
    </table>

    **表2**  参数说明

    <a name="table378911128510"></a>
    <table><thead align="left"><tr id="row157893121054"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="p978911212516"><a name="p978911212516"></a><a name="p978911212516"></a>参数名</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="p12789141217510"><a name="p12789141217510"></a><a name="p12789141217510"></a>输入/输出</p>
    </th>
    <th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="p1278919121515"><a name="p1278919121515"></a><a name="p1278919121515"></a>描述</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row278911126511"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p278915124518"><a name="p278915124518"></a><a name="p278915124518"></a>dstReg</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p678910121514"><a name="p678910121514"></a><a name="p678910121514"></a>输出</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1078910122054"><a name="p1078910122054"></a><a name="p1078910122054"></a>目的操作数。</p>
    <p id="p3789151211518"><a name="p3789151211518"></a><a name="p3789151211518"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
    </td>
    </tr>
    <tr id="row0789181219515"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p478915121056"><a name="p478915121056"></a><a name="p478915121056"></a>srcReg0</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p578981217517"><a name="p578981217517"></a><a name="p578981217517"></a>输入</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p87891412752"><a name="p87891412752"></a><a name="p87891412752"></a>源操作数。</p>
    <p id="p5789712252"><a name="p5789712252"></a><a name="p5789712252"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
    <p id="p197891412453"><a name="p197891412453"></a><a name="p197891412453"></a>数据类型需要与目的操作数保持一致。</p>
    </td>
    </tr>
    <tr id="row078911212512"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p18789812955"><a name="p18789812955"></a><a name="p18789812955"></a>srcReg1</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p678914121258"><a name="p678914121258"></a><a name="p678914121258"></a>输入</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p15789612652"><a name="p15789612652"></a><a name="p15789612652"></a>源操作数。</p>
    <p id="p3789191213516"><a name="p3789191213516"></a><a name="p3789191213516"></a>类型为<a href="../寄存器数据类型/RegTensor.md">RegTensor</a>。</p>
    <p id="p107897127515"><a name="p107897127515"></a><a name="p107897127515"></a>数据类型需要与目的操作数保持一致。</p>
    </td>
    </tr>
    <tr id="row107890126513"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p187891122520"><a name="p187891122520"></a><a name="p187891122520"></a>mask</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p27895121358"><a name="p27895121358"></a><a name="p27895121358"></a>输入</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p177898121258"><a name="p177898121258"></a><a name="p177898121258"></a><span id="ph4789201213510"><a name="ph4789201213510"></a><a name="ph4789201213510"></a>源操作数元素操作的有效指示，详细说明请参考<a href="../寄存器数据类型/MaskReg.md">MaskReg</a>。</span></p>
    </td>
    </tr>
    </tbody>
    </table>

-   对MaskReg操作

    **表3**  参数说明

    <a name="table481714378514"></a>
    <table><thead align="left"><tr id="row98171837059"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1181793716511"><a name="p1181793716511"></a><a name="p1181793716511"></a>参数名</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p208175371155"><a name="p208175371155"></a><a name="p208175371155"></a>描述</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1481753715511"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p781783711518"><a name="p781783711518"></a><a name="p781783711518"></a>dst</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p198175377514"><a name="p198175377514"></a><a name="p198175377514"></a>目的操作数。</p>
    </td>
    </tr>
    <tr id="row2818103719519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>src0</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>源操作数。</p>
    </td>
    </tr>
    <tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>src1</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>源操作数。</p>
    </td>
    </tr>
    <tr id="row3926164202714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1892618426271"><a name="p1892618426271"></a><a name="p1892618426271"></a>mask</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>指示在计算过程中哪些bit有效。</p>
    </td>
    </tr>
    </tbody>
    </table>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

-   对RegTensor操作

    ```
    template <typename T>
    __simd_vf__ inline void XorVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);       
            AscendC::Reg::Xor(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   对MaskReg操作

    ```
    template <typename T>
    __simd_vf__ inline void XorVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
        AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::MaskReg dst;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < (uint16_t)repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Xor(dst, src0, src1, mask);
            AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
        }
    }
    ```

