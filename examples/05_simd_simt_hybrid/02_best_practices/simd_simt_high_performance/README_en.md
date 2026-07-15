# SIMD and SIMT Hybrid Programming High-Performance Optimization Example

## Overview

This example uses the FloorMod computation to demonstrate performance tuning methods in SIMD and SIMT hybrid programming scenarios. The example shows the performance differences among directly accessing GM via SIMT, using SIMD RegBase computation, accessing UB via SIMT with non-contiguous thread mapping, and adjusting thread mapping so that adjacent threads within a Warp access UB contiguously.

**Optimization Path**:

| Case | SCENARIO_NUM | Implementation | Description |
|:---|:---:|:---|:---|
| Case 0 | 0 | SIMT direct GM access | Both data access and computation are implemented via SIMT. SIMT threads directly access GM, serving as the pure SIMT baseline. |
| Case 1 | 1 | SIMD RegBase | Both data access and computation are implemented via SIMD. FloorMod computation is based on RegBase, serving as the pure SIMD baseline. |
| Case 2 | 2 | SIMT non-contiguous UB access | GM data is contiguously transferred to UB via SIMD DataCopy. Each SIMT thread processes a contiguous segment, but adjacent threads within a Warp access non-contiguous addresses. |
| Case 3 | 3 | SIMT contiguous UB access | GM data is contiguously transferred to UB via SIMD DataCopy. Thread mapping is adjusted so that adjacent threads within a Warp access contiguous addresses. |

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.0.0 |

## Directory Structure

```
├── simd_simt_high_performance
│   ├── CMakeLists.txt        // Build project file
│   ├── data_utils.h          // Data read/write functions
│   ├── figures               // Illustrations
│   ├── floor_mod.asc         // Ascend C example implementation
│   ├── README.md             // Example documentation
│   └── scripts
│       ├── gen_data.py       // Input data and ground truth generation script
│       └── verify_result.py  // Ground truth comparison script
```

## Example Description

**Example Function**:

This example implements FloorMod computation. FloorMod is commonly used in tensor computation for periodic indexing, group mapping, and other scenarios that require consistent negative modulo semantics. Here, $r$ is the C/C++ remainder result (that is, $r = x - \operatorname{trunc}(x / y) \times y$, with the sign following the dividend $x$), and the FloorMod result $z$ is obtained through sign correction (with the sign following the divisor $y$). The computation formula is as follows:

$$
r = x - \operatorname{trunc}(x / y) \times y
$$

$$
z =
\begin{cases}
r + y, & r \ne 0 \land ((r < 0) \ne (y < 0)) \\
r, & \text{otherwise}
\end{cases}
$$

**Example Specifications**:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">FloorMod</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="4" align="center">Kernel Function Name</td><td align="center">SCENARIO_NUM=0</td><td colspan="3" align="center"><code>floor_mod_gm_simt_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=1</td><td colspan="3" align="center"><code>floor_mod_simd_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=2</td><td colspan="3" align="center"><code>floor_mod_simt_non_contiguous_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=3</td><td colspan="3" align="center"><code>floor_mod_simt_contiguous_custom</code></td></tr>
</table>

> **Input constraint:** The divisor $y$ must be non-zero (the SIMT path directly performs modulo, and $y$ being 0 triggers a division-by-zero issue). The data generation script avoids 0 values.

## Example Implementation

### Performance Metric Description

| Field Name | Field Description |
|:---|:---|
| Task Duration(μs) | Total task latency, including scheduling time to the accelerator, execution time on the accelerator, and response completion time. |
| aiv_time(μs) | Task execution time on the AI Vector Core. |
| aiv_vec_time(μs) | vec-type instruction latency. Thread computation in SIMT VF calls is also reflected in this metric. |
| aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles. |
| aiv_scalar_time(μs) | scalar-type instruction latency. |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles. |
| aiv_mte2_time(μs) | mte2-type instruction latency, primarily corresponding to GM-to-UB transfers. |
| aiv_mte2_ratio | Ratio of mte2-type instruction cycles to total cycles. |
| aiv_mte3_time(μs) | mte3-type instruction latency, primarily corresponding to UB-to-GM transfers. |
| aiv_mte3_ratio | Ratio of mte3-type instruction cycles to total cycles. |

### Case 0: SIMT Direct GM Access

**Implementation**: Refer to the `floor_mod_simt_gm_contiguous()` function.

This implementation does not use MTE2/MTE3 for data transfer between GM and UB. Instead, SIMT threads directly read `x` and `y` from GM and write results back to GM after computation.

**Key Code**:

```cpp
__aicore__ inline void ProcessGmSimt()
{
    asc_vf_call<floor_mod_simt_gm_contiguous<T>>(
        dim3(THREAD_COUNT), const_cast<__gm__ T*>(xGm.GetPhyAddr()),
        const_cast<__gm__ T*>(yGm.GetPhyAddr()), const_cast<__gm__ T*>(zGm.GetPhyAddr()),
        DataLenPerCore);
}

__simt_vf__ inline void floor_mod_simt_gm_contiguous(
    __gm__ T* x, __gm__ T* y, __gm__ T* z, uint32_t inputTotalLength)
{
    for (uint32_t index = static_cast<uint32_t>(threadIdx.x); index < inputTotalLength;
         index += static_cast<uint32_t>(blockDim.x)) {
        T yValue = y[index];
        const auto rem = x[index] % yValue;
        bool signsDiffer = ((rem < 0) != (yValue < 0));
        z[index] = (signsDiffer && (rem != 0)) ? rem + yValue : rem;
    }
}
```

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | SIMT direct GM access | 64 | 867.222 | 863.175 | 0.996 | 0.005 | 0.000 | 0.003 | 0.000 |

**Performance Data Analysis**:

- In this case, `aiv_mte2_time` and `aiv_mte3_time` are close to 0, indicating that data reads and writes do not go through the MTE2/MTE3 GM->UB/UB->GM transfer path.
- `Task Duration` is **867.222μs**, of which `aiv_vec_time` is **863.175μs** and `aiv_vec_ratio` reaches **0.996**. The latency is primarily concentrated on SIMT internal GM reads/writes and FloorMod computation.

**Principle Description**:

When SIMT threads directly access GM, each thread must read `x` and `y` from GM and write results back to GM during computation. This results in low access efficiency, and data transfer is coupled with computation in the same SIMT thread, preventing parallel transfer and computation pipelining.

The input and output in this example are contiguously arranged 2D ND data on GM, which is suitable for contiguous block transfer. Subsequent implementations introduce the DataCopy interface from the SIMD programming paradigm. GM->UB and UB->GM transfers are performed using DataCopy, executed by MTE, which can transfer larger contiguous data blocks from GM to UB and then contiguously write results from UB back to GM. Compared to SIMT threads accessing GM element by element, this approach provides higher memory access efficiency and enables parallel data transfer and computation pipelining.

**Next Optimization Direction**:

This case serves as the baseline for pure SIMT direct GM access, showing the performance data of a pure SIMT implementation. The next step introduces SIMD/RegBase implementation, using contiguous DataCopy for GM->UB and UB->GM transfers, and implementing FloorMod computation based on RegBase to show the performance data of a pure SIMD implementation.

### Case 1: SIMD RegBase Computation

**Implementation**: Refer to the `floor_mod_simd()` function.

This implementation uses DataCopy for data transfer between GM and UB, and in the RegBase VF function, loads UB data into registers via `LoadAlign`, executes vector instructions such as `Div`, `Mul`, `Sub`, `Compare`, and `Select`, and finally writes back to UB via `StoreAlign`. Both computation and transfer are implemented through SIMD.

**Key Code**:

```cpp
__aicore__ inline void CopyIn(
    uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(xLocal, xGm[tileOffset], count);
    AscendC::DataCopy(yLocal, yGm[tileOffset], count);
}
// SIMD Compute(UB)
template <typename T>
__simd_vf__ inline void floor_mod_simd(
    __ubuf__ T* zAddr, __ubuf__ T* xAddr, __ubuf__ T* yAddr, const uint32_t count)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t loopTimes = DivCeil(count, oneRepeatSize);
    AscendC::Reg::RegTensor<T> xValue, yValue, modValue, tempValue, defaultValue, signValue;
    AscendC::Reg::MaskReg mask, selectMask, adjustMask;
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

__aicore__ inline void CopyOut(uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& zLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(zGm[tileOffset], zLocal, count);
}
```

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | SIMD RegBase | 64 | 532.144 | 531.270 | 523.082 | 0.985 | 3.544 | 0.007 | 237.127 | 0.446 | 55.417 | 0.104 |

**Performance Data Analysis**:

- Compared with Case 0, Case 1 introduces DataCopy for GM->UB/UB->GM transfer. `Task Duration` decreases from **867.222μs** to **532.144μs**, an end-to-end latency reduction of **38.6%**.
- `Task Duration` is **532.144μs**, of which `aiv_vec_time` is **523.082μs** and `aiv_vec_ratio` reaches **98.5%**, indicating that the latency is primarily concentrated on the RegBase vector computation path.
- `aiv_mte2_ratio` is **44.6%** and `aiv_mte3_ratio` is **10.4%**. Transfer latency can partially overlap with computation. The current bottleneck is not the GM transfer process but the FloorMod computation itself in vector instruction execution.

**Principle Description**:

FloorMod computation involves not only remainder calculation but also conditional correction based on the sign relationship between the remainder and divisor. When implemented with SIMD RegBase, branch decisions cannot be written as simple `if/else` statements. Instead, they must be decomposed into multiple vector instructions such as `Compare`, `And`, `MaskAnd`, and `Select` to express conditional logic. Meanwhile, `Div`, `Mul`, `Sub`, sign determination, and final selection have long data dependency chains, and subsequent instructions must wait for preceding results, resulting in high `aiv_vec_ratio`, manifesting as Vec bound.

Compared with SIMD, SIMT is better suited for handling this type of element-wise computation with branch decisions. It can directly use `%` and conditional statements within threads to complete FloorMod correction, reducing the extra computation chain caused by assembling branch logic with vector mask instructions.

**Next Optimization Direction**:

This case serves as the baseline for SIMD RegBase computation, showing the performance data of a pure SIMD implementation. The next step optimizes the Vector computation part using SIMT while retaining the contiguous DataCopy transfer approach.

### Case 2: SIMT Non-Contiguous UB Access

**Implementation**: Refer to the `floor_mod_simt_non_contiguous()` function.

This implementation first transfers `x` and `y` from GM to UB contiguously via DataCopy, then uses SIMT programming for the computation process, completing branch decisions through if-else statements. Each thread processes a contiguous segment of 8 elements, but adjacent threads within the same Warp access non-contiguous addresses:

**Key Code**:

```cpp
// Single tile core flow: CopyIn(GM->UB) -> SIMT Compute(UB) -> CopyOut(UB->GM)
__aicore__ inline void CopyIn(
    uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(xLocal, xGm[tileOffset], count);
    AscendC::DataCopy(yLocal, yGm[tileOffset], count);
}

// SIMT Compute(UB)
template <typename T, uint32_t TileLength>
__simt_vf__ inline void floor_mod_simt_non_contiguous(
    __ubuf__ T* x, __ubuf__ T* y, __ubuf__ T* z, uint32_t inputTotalLength)
{
    constexpr uint32_t elems_per_thread = TileLength / THREAD_COUNT;
    uint32_t tid = threadIdx.x;
    for (uint32_t i = 0; i < elems_per_thread; i++) {
        uint32_t index = tid * elems_per_thread + i;
        if (index >= inputTotalLength) {
            break;
        }
        T yValue = y[index];
        auto rem = x[index] % yValue;
        bool signsDiffer = ((rem < 0) != (yValue < 0));
        if (signsDiffer && (rem != 0)) {
            z[index] = rem + yValue;
        } else {
            z[index] = rem;
        }
    }
}

__aicore__ inline void CopyOut(uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& zLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(zGm[tileOffset], zLocal, count);
}
```

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|
| 2 | SIMT non-contiguous UB access | 64 | 542.492 | 507.571 | 0.937 | 534.293 | 0.986 | 41.851 | 0.077 |

**Performance Data Analysis**:

- Compared with Case 0, Case 2 uses DataCopy for contiguous GM->UB/UB->GM transfer. `Task Duration` decreases from **867.222μs** to **542.492μs**, an end-to-end latency reduction of **37.4%**, indicating that the overhead from direct GM access is significantly mitigated after introducing UB as an intermediary.
- Compared with Case 1, Case 2's `aiv_vec_time` decreases from **523.082μs** to **507.571μs**, a **3.0%** reduction, indicating some benefit on the computation side after using SIMT to express FloorMod element-wise computation.
- However, Case 2's `Task Duration` is **1.9%** higher than Case 1, with no end-to-end optimization benefit. The primary reason is that `aiv_mte2_time` increases from **237.127μs** in Case 1 to **534.293μs**, a **125.3%** increase; `aiv_mte2_ratio` also reaches **0.986**.

**Principle Description**:

The indexing pattern in Case 2 is `index = tid * elems_per_thread + i`, where `elems_per_thread = 8`. From a single thread's perspective, it processes 8 contiguous elements.

The UB on Ascend 950PR/950DT is divided into 16 banks organized into 8 bank groups, with each bank row being 32B (every contiguous 32B falls into the next bank). The vector computation unit can read/write one row (32B) from each bank group per cycle. The 32 threads within a Warp execute the same instruction simultaneously, each with independent addressing.

In Case 2, adjacent threads `tid` and `tid+1` access elements that differ by `elems_per_thread = 8` in the same iteration `i`, meaning the byte address difference is `8 * sizeof(int32) = 32B`, exactly one bank row span. This means adjacent threads fall on **different bank rows**, and the hardware cannot serve multiple threads with a single row read (32B). The same amount of data requires more cycles to fetch, and UB access is prolonged.

<div align="center">
  <img src="./figures/ubBank.png" alt="Ascend 950PR UB bank diagram" width="600">
</div>

**Next Optimization Direction**:

Although Case 2 improves FloorMod computation expression with SIMT, non-contiguous UB access significantly prolongs MTE2 transfer time, resulting in worse end-to-end performance than the pure SIMD RegBase implementation. The next step adjusts the thread-to-data mapping while retaining contiguous DataCopy transfer and SIMT computation expression, changing from "each thread processes a contiguous block of data" to "adjacent threads within a Warp access contiguous elements in the same iteration," so that adjacent threads fall into the same 32B bank row, enabling a single row read to serve multiple threads simultaneously and improving UB access efficiency.

### Case 3: SIMT Contiguous UB Access

**Implementation**: Refer to the `floor_mod_simt_contiguous()` function.

This implementation also transfers `x` and `y` from GM to UB contiguously via DataCopy, but changes the SIMT thread mapping so that adjacent threads within the same Warp access adjacent elements:

**Key Code**:

```cpp
// Single tile core flow: CopyIn(GM->UB) -> SIMT Compute(UB) -> CopyOut(UB->GM)
__aicore__ inline void CopyIn(
    uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(xLocal, xGm[tileOffset], count);
    AscendC::DataCopy(yLocal, yGm[tileOffset], count);
}
// SIMT Compute(UB)
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

__aicore__ inline void CopyOut(uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& zLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(zGm[tileOffset], zLocal, count);
}
```

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|
| 3 | SIMT contiguous UB access | 64 | **457.402** | **319.948** | 0.701 | 437.758 | 0.959 | 110.623 | 0.242 |

**Performance Data Analysis**:

- Compared with Case 2, Case 3 still uses DataCopy for contiguous GM->UB/UB->GM transfer, but adjusts thread mapping so that adjacent threads within a Warp access adjacent elements. `Task Duration` decreases from **542.492μs** to **457.402μs**, an end-to-end latency reduction of **15.7%**.
- Compared with Case 1, Case 3's `Task Duration` decreases by **14.0%**.
- `aiv_vec_time` decreases from **507.571μs** in Case 2 to **319.948μs**, a **37.0%** reduction. Case 3 ensures that adjacent threads within a Warp access adjacent elements, so 32 adjacent threads fall into contiguous 32B bank rows. Each row read can simultaneously serve multiple threads in that row, UB access is coalesced, and the required number of cycles decreases.
- `aiv_mte2_time` decreases from **534.293μs** in Case 2 to **437.758μs**, an **18.1%** reduction, indicating that contiguous access has alleviated UB resource contention from the SIMT computation phase on MTE2. However, `aiv_mte2_ratio` still reaches **0.959**, indicating that the current bottleneck has shifted to the MTE2 transfer path.

## Performance Comparison Summary

### Ascend 950PR Performance Comparison

The following table shows the performance data comparison for this example running on Ascend 950 series products:

| Case | Implementation | Cores | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | Primary Bottleneck |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|:---|
| 0 | SIMT direct GM access | 64 | 867.222 | 863.175 | 0.996 | 0.005 | 0.000 | 0.003 | 0.000 | SIMT direct GM access |
| 1 | SIMD RegBase | 64 | 532.144 | 523.082 | 0.985 | 237.127 | 0.446 | 55.417 | 0.104 | Vec bound |
| 2 | SIMT non-contiguous UB access | 64 | 542.492 | 507.571 | 0.937 | 534.293 | 0.986 | 41.851 | 0.077 | Cross-bank-row UB access |
| 3 | SIMT contiguous UB access | 64 | **457.402** | **319.948** | 0.701 | 437.758 | 0.959 | 110.623 | 0.242 | MTE2 bound |

### Optimization Key Points Summary

| Optimization Method | Core Principle | Example Demonstration |
|:---|:---|:---|
| Use SIMD for contiguous data transfer | When SIMT directly accesses GM, access granularity and cache paths may result in low bandwidth utilization. First transfer data contiguously into UB using DataCopy, then have SIMT access UB, which improves access efficiency | Case 3 reduces end-to-end latency by 47.3% compared to Case 0 |
| Use SIMT for branch decisions | For element-wise computation with conditional correction, SIMD RegBase requires multiple Compare/Select/Mask instructions to express branches, resulting in long dependency chains. SIMT can directly express `%` and conditional decisions within threads, reducing the overhead of assembling branch logic with vector mask instructions | Case 2 reduces `aiv_vec_time` by 3.0% compared to Case 1; after contiguous access, Case 3 reduces `aiv_vec_time` by 37.0% compared to Case 2 |
| Adjust thread mapping for contiguous UB access within Warp | UB is organized as 16 banks / 8 bank groups with 32B per bank row. The hardware reads/writes one row per bank group per cycle. Having adjacent threads within the same Warp access adjacent elements that fall into the same 32B bank row enables a single row read to serve multiple threads simultaneously, which is more efficient than "contiguous within a single thread but adjacent threads spanning 32B rows" | Case 3 changes `tid*8+i` to `tid+i*1024` pattern, reducing end-to-end latency by 15.7% compared to Case 2 |

---

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Cases

  Specify the Case to build via `-DSCENARIO_NUM=N` during cmake compilation. Case descriptions:
  - `0`: SIMT direct GM access
  - `1`: SIMD RegBase
  - `2`: SIMT non-contiguous access
  - `3`: SIMT contiguous access

  Example:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=3
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1                                                       # Select the execution scenario
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm algorithm logic correctness
  ```

  To use the NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

- Build option description

  | Option | Values | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture, corresponding to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`, `1`, `2`, `3` | Case number: 0=SIMT direct GM access, 1=SIMD RegBase, 2=SIMT non-contiguous access, 3=SIMT contiguous access |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```

### Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run operator tuning on the executable demo with `msopprof`:

    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the specific performance analysis results:

```bash
# View Task Duration and various metrics
cat ./OPPROF_*/PipeUtilization.csv
```
