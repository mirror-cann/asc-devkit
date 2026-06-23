# 通过SIMT实现分支判断<a name="ZH-CN_TOPIC_0000002552896537"></a>

>[!NOTE]说明 
>该性能优化建议适用于如下型号：
>-   Ascend 950PR/Ascend 950DT

【优先级】高

【描述】基于SIMD编程模型实现的批量数据计算性能很高，但在算子实现逻辑中涉及分支判断时，基于SIMD的计算操作会变得相对复杂，导致性能下降。此时，可以考虑采用SIMT方式，因为SIMT编程更为灵活，更适合处理分支判断的场景。

【样例介绍】以floor\_mod算子为例，算子功能为将输入x的每个元素除以输入y的对应元素，获取余数。该余数应与除数y具有相同的符号，且其绝对值应小于y的绝对值。在计算过程中，需要判断y中每个元素的符号以及余数与该元素绝对值的大小关系。完整的算子实现代码请参考[SIMT与SIMD混合编程高性能优化样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance)。

**表1**  样例规格

<table>
<thead>
<tr>
<th>名称</th>
<th>name</th>
<th>shape</th>
<th>data type</th>
<th>format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">算子输入</td>
<td>x</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>y</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>算子输出</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

SIMT线程层次结构为：

-   Kernel启动核数：64
-   单次SIMT VF调用线程数：1024

【反例】

基于SIMD RegBase的floor\_mod算子实现：对应样例中的场景1（SCENARIO\_NUM=1）。该场景使用DataCopy完成GM与UB之间的数据搬运，在UB上使用RegBase VF函数实现计算。由于SIMD无法直接通过普通if else语句表达逐元素分支判断，因此需要使用多个Reg矢量计算API完成符号比较、条件组合和结果选择，相关代码如下。

```cpp
template <typename T>
__simd_vf__ inline void floor_mod_simd(__ubuf__ T* zAddr, __ubuf__ T* xAddr, __ubuf__ T* yAddr, const uint32_t count)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t loopTimes = AscendC::CeilDivision(count, oneRepeatSize);
    AscendC::Reg::RegTensor<T> xValue;
    AscendC::Reg::RegTensor<T> yValue;
    AscendC::Reg::RegTensor<T> modValue;
    AscendC::Reg::RegTensor<T> tempValue;
    AscendC::Reg::RegTensor<T> defaultValue;
    AscendC::Reg::RegTensor<T> signValue;

    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg selectMask;
    AscendC::Reg::MaskReg adjustMask;
    uint32_t maskCount = count;

    AscendC::Reg::Duplicate(defaultValue, T(-1));
    AscendC::Reg::Duplicate(signValue, FMOD_B32_SIGN);

    for (uint16_t i = 0; i < loopTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(maskCount);
        AscendC::Reg::LoadAlign(xValue, xAddr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(yValue, yAddr + i * oneRepeatSize);

        AscendC::Reg::Div(tempValue, xValue, yValue, mask);
        AscendC::Reg::Mul(tempValue, yValue, tempValue, mask);
        AscendC::Reg::Sub(modValue, xValue, tempValue, mask);

        AscendC::Reg::Compares<T, AscendC::CMPMODE::NE>(selectMask, yValue, T(0), mask);
        AscendC::Reg::Select(tempValue, modValue, defaultValue, selectMask);

        AscendC::Reg::Add(modValue, tempValue, yValue, mask);
        AscendC::Reg::Compares<T, AscendC::CMPMODE::NE>(adjustMask, tempValue, T(0), mask);
        AscendC::Reg::And(xValue, tempValue, signValue, mask);
        AscendC::Reg::And(yValue, yValue, signValue, mask);
        AscendC::Reg::Compare<T, AscendC::CMPMODE::NE>(selectMask, xValue, yValue, mask);
        AscendC::Reg::MaskAnd(adjustMask, selectMask, adjustMask, mask);
        AscendC::Reg::Select(modValue, modValue, tempValue, adjustMask);
        AscendC::Reg::StoreAlign(zAddr + i * oneRepeatSize, modValue, mask);
    }
}
```

【正例】

基于SIMT的floor\_mod算子实现：对应样例中的场景3（SCENARIO\_NUM=3）。该场景同样使用DataCopy完成GM与UB之间的数据搬运，在UB上采用SIMT编程方式实现计算过程，通过if else语句完成分支判断，代码如下所示。

```cpp
template <typename T>
__simt_vf__ inline void floor_mod_simt_contiguous(
    __ubuf__ T* x, __ubuf__ T* y, __ubuf__ T* z, uint32_t inputTotalLength)
{
    for (uint32_t index = static_cast<uint32_t>(threadIdx.x); index < inputTotalLength;
         index += static_cast<uint32_t>(blockDim.x)) {
        T yValue = y[index];
        const auto rem = x[index] % yValue;
        bool signsDiffer = ((rem < 0) != (yValue < 0));
        if (signsDiffer && (rem != 0)) {
            z[index] = rem + yValue;
        } else {
            z[index] = rem;
        }
    }
}
```

【性能对比】

在核数相同、输入输出规格相同、均使用DataCopy完成GM与UB之间数据搬运的情况下，对比场景1和场景3的性能数据如下。

| 场景 | 实现方式 | 核数 | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_vec\_ratio | aiv\_mte2\_time\(μs\) | aiv\_mte2\_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|
| 场景1 | SIMD RegBase | 64 | 532.144 | 523.082 | 0.985 | 237.127 | 0.446 |
| 场景3 | SIMT连续访问UB | 64 | 457.402 | 319.948 | 0.701 | 437.758 | 0.959 |

相比场景1，场景3使用SIMT直接表达floor\_mod中的分支判断逻辑，aiv\_vec\_time从523.082μs降低至319.948μs，下降约38.8%，说明Vector计算侧耗时明显降低。端到端Task Duration从532.144μs降低至457.402μs，下降约14.0%，场景3的aiv\_mte2\_ratio达到0.959，主要瓶颈已转为MTE2 Bound，即算子性能的最大影响转变为GM到UB的搬运效率。
