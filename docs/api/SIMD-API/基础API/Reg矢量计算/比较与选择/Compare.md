# Compare<a name="ZH-CN_TOPIC_0000001929827672"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3  -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"`。

逐元素比较两个[RegTensor](../寄存器数据类型/RegTensor.md)大小，根据模板参数指定的比较模式，将比较结果写入目的操作数[MaskReg](../寄存器数据类型/MaskReg.md)中对应比特位，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。

其中cmp由模板参数mode指定比较模式，取值为EQ/GT/LT/GE/NE/LE。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U>
__simd_callee__ inline void Compare(MaskReg& dst, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 操作数数据类型。 |
| mode | 比较模式。支持如下取值：<br>• LT：小于<br>• GT：大于<br>• GE：大于或等于<br>• EQ：等于<br>• NE：不等于<br>• LE：小于或等于 |
| U | 源操作数的`RegTensor`类型，例如`RegTensor<half>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | [MaskReg](../寄存器数据类型/MaskReg.md)类型，目的操作数。 |
| srcReg0、srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>两个源操作数的数据类型需要与目的操作数保持一致。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表3**  数据类型组合

| srcReg0和srcReg1数据类型 | dst数据类型 |
| :----------------------- | :---------- |
| int8_t | MaskReg |
| uint8_t | MaskReg |
| int16_t | MaskReg |
| uint16_t | MaskReg |
| half | MaskReg |
| bfloat16_t | MaskReg |
| int32_t | MaskReg |
| uint32_t | MaskReg |
| float | MaskReg |
| int64_t | MaskReg |
| uint64_t | MaskReg |

## 约束说明<a name="section633mcpsimp"></a>

- 通过mask参数控制的未选中元素在目的操作数中被置零。
- 操作数重叠约束：srcReg0和srcReg1可以是同一个`RegTensor`。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void CompareVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg dstMask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);        
        AscendC::Reg::Compare<T, AscendC::CMPMODE::EQ>(dstMask, srcReg0, srcReg1, mask);
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, dstMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
