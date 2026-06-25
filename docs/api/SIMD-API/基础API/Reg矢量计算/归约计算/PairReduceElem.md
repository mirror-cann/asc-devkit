# PairReduceElem<a name="ZH-CN_TOPIC_0000002045329737"></a>

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

头文件路径为：basic_api/kernel_operator_vec_reduce_intf.h。

将传入的srcReg中相邻两个数值相加，并将产生的结果保存在dstReg中的低位位置。

![PairReduceElem示意图](../../../../figures/reg_pair_reduce_elem.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <PairReduce type = PairReduce::SUM, typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PairReduceElem(U& dstReg, U srcReg, MaskReg mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| type | 具体的PairReduce类型，当前仅支持归约求和计算。<br><pre>enum class PairReduce {<br>    SUM = 0,<br>};</pre> |
| T | 目的操作数和源操作数的数据类型。 |
| mode | [MaskMergeMode](../数据类型/MaskMergeMode.md)，选择MERGING模式或ZEROING模式。<br>&bull; ZEROING，mask未筛选的元素在dst中置零。目前仅支持该模式。<br>&bull; MERGING，当前不支持。 |
| U | 目的操作数和源操作数的RegTensor类型，由编译器自动推导，用户不需要填写。 |

**表2** 函数参数说明

| 参数名 | 描述 |
| --- | --- |
| dstReg | 目的操作数。<br>类型为[RegTensor](../概述/寄存器数据类型/RegTensor.md)。 |
| srcReg | 源操作数。<br>类型为[RegTensor](../概述/寄存器数据类型/RegTensor.md)。 |
| mask | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../概述/寄存器数据类型/MaskReg.md)。 |

## 数据类型

支持的数据类型为：half、float。

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void PairReduceElemVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::PairReduceElem<AscendC::Reg::PairReduce::SUM>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
