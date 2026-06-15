# 使用Unified Buffer提升内存访问效率<a name="ZH-CN_TOPIC_0000002511242528"></a>

>[!NOTE]说明 
>该性能优化建议适用于如下型号：
>-   Ascend 950PR/Ascend 950DT

【优先级】高

【描述】SIMT线程直接访问Global Memory时，访存请求会经过GM访问路径，数据搬运和线程计算耦合在同一段SIMT逻辑中，容易降低整体访存效率。当单次处理的数据量可放入最大可用Unified Buffer空间（256KB - 系统预留8KB - 最小Dcache 32KB）时，可以使用SIMD搬运接口将数据从Global Memory连续搬运到Unified Buffer，使SIMT编程直接访问Unified Buffer中的数据，从而提高内存访问效率，提升算子的整体性能。

【样例介绍】以SIMD与SIMT混合编程方式实现的floor\_mod算子为例。该算子输入x和y的shape均为\[8192, 8192\]，数据类型为int32，输出z的shape为\[8192, 8192\]。完整样例请参考[SIMT与SIMD混合编程高性能优化样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/05_best_practices/08_simd_simt_hybrid/simt_and_simd_high_performance)。

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

SIMT直接访问Global Memory上的x、y和z数据，对应样例中的场景0（SCENARIO\_NUM=0）。该场景未通过DataCopy完成GM与UB之间的数据搬运，SIMT线程直接从GM读取输入并将结果写回GM，代码如下。

```cpp
template <typename T>
__simt_vf__ inline void floor_mod_simt_gm_contiguous(
    __gm__ T* x, __gm__ T* y, __gm__ T* z, uint32_t inputTotalLength)
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

__aicore__ inline void ProcessGmSimt()
{
    asc_vf_call<floor_mod_simt_gm_contiguous<T>>(
        dim3(THREAD_COUNT), const_cast<__gm__ T*>(xGm.GetPhyAddr()),
        const_cast<__gm__ T*>(yGm.GetPhyAddr()), const_cast<__gm__ T*>(zGm.GetPhyAddr()),
        DataLenPerCore);
}
```

【正例】

使用SIMD接口DataCopy将x和y从Global Memory连续搬运到Unified Buffer，基于SIMT编程方式直接从Unified Buffer读取数据并写入Unified Buffer，再通过DataCopy将结果写回Global Memory，对应样例中的场景3（SCENARIO\_NUM=3）。代码如下。

```cpp
__aicore__ inline void CopyIn(
    uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(xLocal, xGm[tileOffset], count);
    AscendC::DataCopy(yLocal, yGm[tileOffset], count);
}

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

__aicore__ inline void Compute(
    uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal,
    AscendC::LocalTensor<T>& zLocal)
{
    __ubuf__ T* xAddr = (__ubuf__ T*)xLocal.GetPhyAddr();
    __ubuf__ T* yAddr = (__ubuf__ T*)yLocal.GetPhyAddr();
    __ubuf__ T* zAddr = (__ubuf__ T*)zLocal.GetPhyAddr();

    asc_vf_call<floor_mod_simt_contiguous<T>>(dim3(THREAD_COUNT), xAddr, yAddr, zAddr, count);
    AscendC::DataSyncBarrier<AscendC::MemDsbT::UB>();
}

__aicore__ inline void CopyOut(uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& zLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(zGm[tileOffset], zLocal, count);
}
```

【性能对比】

场景0和场景3的性能对比如下。

| 场景 | 实现方式 | 核数 | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_mte2\_time\(μs\) | aiv\_mte3\_time\(μs\) |
|:---|:---|:---:|---:|---:|---:|---:|
| 场景0 | SIMT直接访问GM | 64 | 867.222 | 863.175 | 0.005 | 0.003 |
| 场景3 | SIMT连续访问UB | 64 | 457.402 | 319.948 | 437.758 | 110.623 |

场景0中，aiv\_mte2\_time和aiv\_mte3\_time接近0，说明数据读写没有走GM到UB、UB到GM的DataCopy搬运路径，耗时主要集中在SIMT直接访问GM和计算过程上。场景3中，输入和输出通过DataCopy在GM与UB之间连续搬运，SIMT线程直接访问UB完成计算，Task Duration从867.222μs降低至457.402μs，端到端耗时下降约47.3%。
