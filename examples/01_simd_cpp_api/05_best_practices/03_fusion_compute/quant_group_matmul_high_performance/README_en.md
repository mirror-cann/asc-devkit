# QuantGroupMatmul Best Practice Example

## Overview

This example implements per-token quantized grouped matrix multiplication (QuantGroupMatmul), demonstrating performance tuning methods for Vector bound scenarios during CV fusion.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── quant_group_matmul_high_performance
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── figures                            // Image resources directory for README
│   ├── quant_group_matmul_custom.asc      // Ascend C example implementation & invocation example
│   ├── README.md                          // Example documentation
│   └── scripts
│       ├── gen_data.py                    // Input data and ground truth generation script
│       └── verify_result.py               // Verification script for comparing output data with ground truth
```

## Example Description

- Example Function:  
  The example implements grouped per-token quantized matmul computation with the grouping axis being the m axis, and performs Gelu activation function computation on the result.

  The QuantGroupMatmul computation formula is:

  $$
  s_i = \sum_{t=0}^{i-1} group[t]
  $$

  $$
  y[s_i:s_i + group[i], :] =
  \operatorname{Gelu}\left(
  \left(x[s_i:s_i + group[i], :] \times weight[i]\right)
  \odot scale[i] \odot perTokenScale[s_i:s_i + group[i]]
  \right), \quad 0 \le i < g
  $$

  Where $s_i$ represents the starting position of the $i$-th group along the m axis, and $\odot$ represents element-wise multiplication following broadcast rules.

  - x: Left matrix, shape [m, k], data type int8;
  - weight: Right matrix, logical shape [g, k, n], actual input file in NZ format, data type int8;
  - group: Records the m size of each group, data type int64;
  - scale: Quantization parameter for the right matrix, shape [g, n], data type float, used for dequantization of the matrix multiplication result, using scale[i] for dequantization of the i-th matrix multiplication result;
  - perTokenScale: Per-token scale for the left matrix, shape [m], data type float, used for dequantization of the matrix multiplication result, using the same index range as x rows for dequantization;
  - y: Output, storing the matrix multiplication result matrix, shape [m, n], data type float16;

- Example specifications:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">QuantGroupMatmul</td></tr>
<tr><td rowspan="6" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 1024]</td><td align="center">int8</td><td align="center">ND</td></tr>
<tr><td align="center">weight</td><td align="center">[8, 1024, 8192]</td><td align="center">int8</td><td align="center">NZ</td></tr>
<tr><td align="center">group</td><td align="center">[8]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">scale</td><td align="center">[8, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">perTokenScale</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float16</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">quant_group_matmul_custom</td></tr>
</table>

- Invocation Implementation  
  Use the kernel invocation <<<>>> to call the kernel function.

## Example Implementation

### Scenario Description

This example demonstrates the complete optimization path for Vector bound scenarios through 3 progressive scenarios:

| Scenario | SCENARIO_NUM | __mix__ Mode | PIPELINE_DEPTH | Description |
|------|-------------|------------|----------------|------|
| Scenario 0 | 0 | `__mix__(1, 1)` | 1 | Baseline: Single AIV, no pipeline overlap, Cube and Vector serial |
| Scenario 1 | 1 | `__mix__(1, 2)` | 1 | Dual AIV, no pipeline overlap, multiple AIVs share Vector tasks |
| Scenario 2 | 2 | `__mix__(1, 2)` | 4 | Dual AIV + 4-stage pipeline, Cube-Vector fine-grained pipeline overlap |

**Key Code**:
```cpp
constexpr uint32_t scenarioNum = SCENARIO_NUM;
// ...
constexpr uint32_t PIPELINE_DEPTH = (scenarioNum >= 2) ? 4 : 1;

template <...>
#if SCENARIO_NUM == 0
__global__ __mix__(1, 1)
#else
__global__ __mix__(1, 2)
#endif
void quant_group_matmul_custom(...)
```

### Performance Metric Description

| Metric                    | Description                                                                 |
|------------------------|----------------------------------------------------------------------|
| Task Duration(μs)      | Total execution time of the entire task, example execution time is determined by this parameter                         |
| Block Num              | Number of cores used (Block count)                                              |
| aicore_time(μs)        | Average AI Core execution time                                                |
| aic_mac_time(μs)       | Cube type instruction (matrix computation instructions) duration                                    |
| aic_mac_ratio          | Ratio of cube type instruction cycle count to total cycle count    |
| aic_mte1_time(μs)      | MTE1 type instruction (L1 -> L0A/L0B transfer) duration                               |
| aic_mte1_ratio         | Ratio of MTE1 type instruction cycle count to total cycle count |
| aic_mte2_time(μs)      | MTE2 type instruction (GM -> L1 transfer) duration                                    |
| aic_mte2_ratio         | Ratio of MTE2 type instruction cycle count to total cycle count    |
| aic_fixpipe_time(μs)   | Fixpipe type instruction (L0C -> GM/UB transfer) duration                             |
| aic_fixpipe_ratio      | Ratio of fixpipe type instruction cycle count to total cycle count |
| aiv_time(μs)           | Task execution time on AI Vector Core                               |
| aiv_vec_time(μs)       | vec type instruction (vector computation instructions) duration                                    |
| aiv_vec_ratio          | Ratio of vec type instruction cycle count to total cycle count    |
| aiv_scalar_time(μs)    | Scalar type instruction (scalar computation instructions) duration                                 |
| aiv_scalar_ratio       | Ratio of scalar type instruction cycle count to total cycle count |
| aiv_mte2_time(μs)      | MTE2 type instruction (GM -> UB transfer instructions) duration                              |
| aiv_mte2_ratio         | Ratio of MTE2 type instruction cycle count to total cycle count |
| aiv_mte3_time(μs)      | MTE3 type instruction (UB -> GM transfer instructions) duration                              |
| aiv_mte3_ratio         | Ratio of MTE3 type instruction cycle count to total cycle count |

### Data Flow Path

```text
Storage Location                  Data Type           Pipeline

GM (x, weight)                   int8
  |                                               MTE2
L1                               int8
  |                                               MTE1
L0A / L0B                        int8
  |  Iterate                                     MMAD
L0C                              int32
  |                                               FIXPIPE
GM (workspace)                   int32
  |  DataCopyPad2D                                MTE2
UB vecInQueue                    int32
  |  AscendDequant (×scale)   int32 → float       VECTOR
UB dequantMiddleResult            float
  |  Mul (×perTokenScale)     float → float       VECTOR
UB mulsResultLocal                float
  |  FasterGelu               float → float       VECTOR
UB actResultLocal                 float
  |  Cast                     float → half        VECTOR
UB vecOutQueue                    half
  |  DataCopyPad2D                                MTE3
GM (y)                            half

Note: scale[float] and perTokenScale[float] are independently transferred to UB via MTE2,
    participating in AscendDequant and Mul steps respectively.
```

### Core Features

#### Case 0: Baseline Implementation (`__mix__(1, 1)`, PIPELINE_DEPTH=1)

Case 0 is the basic scenario in this example, using `__mix__(1, 1)` mode (1 AIC + 1 AIV), PIPELINE_DEPTH=1. Cube and Vector synchronize serially at Iterate granularity — after Cube completes one Iterate, it notifies Vector, and after Vector finishes processing, it notifies Cube, with no pipeline overlap. On this basis, Case 0 already includes the following optimization features:

**① Cube+Vector Fused Computation**

This example uses `__mix__` kernel function mode, with AIC (Cube core) and AIV (Vector core) running in parallel on the same physical core. AIC executes matrix multiplication, AIV executes dequantization+activation+type conversion, and both synchronize through `CrossCoreSetFlag`/`CrossCoreWaitFlag`:

```cpp
// AIC execution path: MMCompute → Iterate loop → SetFlag(SYNC_AIC_TO_AIV)
// AIV execution path: Skip MMCompute → VectorCompute → WaitFlag(SYNC_AIC_TO_AIV)
if ASCEND_IS_AIC {
    matmulObj.Iterate();    // Cube computation
    CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_TO_AIV);  // Notify Vector
}
if ASCEND_IS_AIV {
    CrossCoreWaitFlag(SYNC_AIC_TO_AIV);  // Wait for Cube completion
    // Process data...
    CrossCoreSetFlag<2, PIPE_MTE2>(SYNC_AIV_TO_AIC);
}
```

In Case 0, PIPELINE_DEPTH=1, workspace has only 1 Slot (that is, `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)` sized continuous space). After each Cube Iterate, it must wait for Vector to finish processing before reusing that Slot, with Cube and Vector executing serially.

**② Cross-Group Continuous Polling (preCount Mechanism)**

The Process function uses a **cross-group continuous polling** strategy to assign sub-blocks to multiple Cores. The core idea is to logically number all Group sub-blocks continuously, with each Core polling blocks at `CORE_NUM` stride, using the `preCount` variable to record the remainder from the previous Group end that was not divisible by `CORE_NUM`, achieving continuous allocation across Groups and avoiding inter-core load imbalance.

**③ Tiling Constantification**

All Tiling parameters are determined at compile time through `MatmulApiStaticTiling`, requiring no Scalar dynamic computation at runtime, reducing Scalar overhead:

```cpp
constexpr static auto CONSTANT_CFG = GetCustomConstantCFG();
matmul::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
```

**④ Matmul UnitFlag Optimization**

This example enables Matmul UnitFlag optimization through `mmCFG.enUnitFlag = true`, achieving 512B fine-grained pipeline synchronization between Cube computation and transfer:

```cpp
static constexpr MatmulShapeParams shapeParams = {SINGLE_M, SINGLE_N, K, BASE_M, BASE_N, BASE_K};
MatmulConfig mmCFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
mmCFG.enUnitFlag = true;  // Enable UnitFlag
auto constantCFG = AscendC::GetMatmulApiTiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(mmCFG);
```

**⑤ UB Double Buffer**

All TQue queues in this example set `BUFFER_NUM=2` to enable double buffer, achieving pipeline parallelism between Vector side data transfer and computation:

```cpp
constexpr uint32_t BUFFER_NUM = 2; // UB queue Buffer count, set to 2 to enable double buffer

pipe->InitBuffer(scaleInQueue, BUFFER_NUM, BASE_N * sizeof(float));          // 2 × 1024B
pipe->InitBuffer(perTokenScaleInQueue, BUFFER_NUM, BASE_M * sizeof(float));   // 2 × 512B
pipe->InitBuffer(vecInQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(int32_t));      // 2 × 24,576B
pipe->InitBuffer(vecOutQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(half));        // 2 × 12,288B
```

**Performance Data**

Taking Ascend 950PR measured performance as example:

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |

**Optimization Effect Analysis**:
- End-to-end duration: **282.717μs**
- Cube computation duration: 167.952μs, ratio **59.6%**
- Vec computation duration: 229.166μs, ratio **81.3%**

Case 0 as the baseline scenario, Vector computation duration (229.166μs) significantly exceeds Cube computation (167.952μs), Vector duration is long and needs optimization.

**Next Optimization Direction**: Improve Vector side computation parallelism, upgrade from `__mix__(1, 1)` to `__mix__(1, 2)` dual AIV mode.

#### Case 1: Dual AIV Parallel (`__mix__(1, 2)`, PIPELINE_DEPTH=1)

Case 1 upgrades the kernel function mode from `__mix__(1, 1)` to `__mix__(1, 2)` based on Case 0, with each physical core containing 1 AIC and 2 AIVs. Multiple AIVs share Vector side computation tasks to improve Vector throughput. PIPELINE_DEPTH remains 1, Cube and Vector still synchronize serially.

**Performance Data**:
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |

**Optimization Effect Analysis**:
- End-to-end duration: **224.125μs**, duration reduced by 20.72%
- Vec computation duration: 112.441μs, duration reduced by 50.93%, matching expected benefits

**PIPE Pipeline Diagram**:

The PIPE pipeline diagram shows that although Case 1 already introduces dual AIV to share Vector tasks, due to `PIPELINE_DEPTH=1`, Cube and Vector still synchronize serially at Iterate granularity, and workspace has only a single Slot available for reuse, so the pipeline still shows obvious waiting and breakpoints.

![Case 1 PIPE Pipeline Diagram](./figures/CV流水优化1.png)

**Next Optimization Direction**: Introduce 4-stage Cube-Vector pipeline to improve Cube-Vector parallelism and further improve overall computation efficiency.

#### Case 2: 4-Stage Cube-Vector Pipeline (`__mix__(1, 2)`, PIPELINE_DEPTH=4)

Case 2 increases PIPELINE_DEPTH from 1 to 4 based on Case 1, introducing workspace ring Slot reuse mechanism to achieve Cube-Vector fine-grained pipeline overlap.

**Workspace Ring Slot Reuse**

workspace is the relay station between Cube and Vector, using ring slot design. The Slot here refers to a continuous space located by `workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)`, with single Slot size `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)`. `PIPELINE_DEPTH` Slots are used in rotation. While Cube writes to the current Slot, Vector can process data from previous Slots:

```
workspace layout:
Slot 0: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 1: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 2: ...
Slot 3: ...

workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)
```

**4-Stage Pipeline Timing**:

```
Time ──────────────────────────────────────────────────────────────>
Cube:  |─Iter0→Slot0─|─Iter1→Slot1─|─Iter2→Slot2─|─Iter3→Slot3─|─Iter4→Slot0─| ...
                      (Wait)                                     (Wait)
Vec:                 |─Slot0 Process─|─Slot1 Process─|─Slot2 Process─|─Slot3 Process─| ...
```

**Performance Data**:
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |

**Optimization Effect Analysis**:
- End-to-end duration: **204.129μs**, reduced by 8.92% relative to Case 1, reduced by **27.80%** relative to Case 0.
- Cube computation duration: 180.347μs, slightly increased from Case 1, but `aic_mac_ratio` improved from 0.753 to 0.887, indicating better Cube computation continuity.
- Vec computation duration: 112.218μs, basically the same as Case 1's 112.441μs, indicating this optimization round did not change Vector side single computation overhead. The main benefit comes from Cube-Vector pipeline overlap.

**PIPE Pipeline Diagram**:
Case 2 increases `PIPELINE_DEPTH` to 4 based on Case 1, allowing Cube to write to different Slots continuously across multiple Iterates, with better Cube computation continuity.

![Case 2 PIPE Pipeline Diagram](./figures/CV流水优化2.png)

## Performance Data Analysis

### Atlas A2 Training Series Chip Performance Data

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 583.512 | 24 | 24 | 564.06 | 192.02 | 0.34 | 115.317 | 0.204 | 168.645 | 0.299 | 141.648 | 0.251 | 79.47 | 0.141 | 565.12 | 471.71 | 0.835 | 228.275 | 0.404 | 139.25 | 0.246 | 94.682 | 0.168 |
| Scenario 1 | MIX_AIC | 353.887 | 24 | 48 | 344.01 | 192.612 | 0.56 | 143.518 | 0.417 | 170.031 | 0.494 | 160.396 | 0.466 | 107.662 | 0.313 | 345.22 | 235.871 | 0.683 | 122.424 | 0.355 | 107.715 | 0.312 | 56.494 | 0.164 |
| Scenario 2 | MIX_AIC | 299.286 | 24 | 48 | 288.65 | 193.576 | 0.671 | 196.668 | 0.681 | 173.003 | 0.599 | 170.193 | 0.59 | 167.936 | 0.582 | 285.21 | 235.872 | 0.827 | 126.227 | 0.443 | 99.321 | 0.348 | 56.645 | 0.199 |


### Ascend 950PR Chip Performance Data
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |


### Performance Analysis Summary

This example has fixed shape `M=8192, N=8192, K=1024`. Grouping only changes the M axis scheduling method. Total Cube computation volume remains:

$$
Ops_{\text{cube}} = \sum_{i=0}^{G-1} group_i \times N \times K = M \times N \times K
$$

For int8 matrix multiplication, Cube single cycle is estimated at `16 × 32 × 16` multiply-accumulate operations. Theoretical Cube computation duration is:

$$
T_{\text{cube}} =
\frac{M \times N \times K}{16 \times 32 \times 16 \times f \times CORE\_NUM}
$$

The final optimization scenario is Scenario 2 (`__mix__(1, 2)` + 4-stage Cube-Vector pipeline). Cube side utilization and bottleneck assessment for this scenario:

| Chip | CORE_NUM | Theoretical Cube Duration (μs) | aic_mac_time(μs) | aic_mac_ratio | aiv_vec_time(μs) | Theoretical Peak Percentage | Bottleneck Assessment |
|------|----------|------------------|------------------|---------------|------------------|----------------|----------|
| Atlas A2/A3 (DAV_2201) | 24 | 188.93 | 193.576 | 0.671 | 235.872 | 97.60% | Still偏向 Vector side |
| Ascend 950PR (DAV_3510) | 32 | 158.88 | 180.347 | 0.887 | 112.218 | 88.10% | No longer Vector side |

From the final results, on Ascend 950PR, Vector side computation duration has been reduced to 112.218μs through dual AIV parallelism, significantly lower than Cube computation duration 180.347μs, and `aic_mac_ratio` reaches 0.887, indicating the performance bottleneck is no longer Vector computation itself. At this point, end-to-end duration is primarily affected by Cube computation, workspace reuse synchronization, FIXPIPE transfer, and pipeline tail-end overhead.

Atlas A2/A3 shows different behavior: In Scenario 2, `aiv_vec_time` is 235.872μs, higher than `aic_mac_time`'s 193.576μs, and `aiv_vec_ratio` reaches 0.827, indicating Vector side dequantization, per-token scaling, FasterGelu, and Cast chain still occupies a large proportion, with overall still偏向 Vector side bottleneck. Meanwhile, A2 measured Cube computation duration 193.576μs is already close to theoretical Cube duration 188.93μs, reaching approximately 97.60% of theoretical peak, indicating Cube matrix multiplication itself is already well utilized. Continuing to optimize end-to-end performance requires prioritizing Vector side computation chain and Cube-Vector synchronization overhead.

Although Atlas A2/A3 is still偏向 Vector bound in the end, the three-level optimization is still effective: Scenario 1 reduces Vector computation duration from 471.710μs to 235.871μs through dual AIV parallelism, and end-to-end duration from 583.512μs to 353.887μs; Scenario 2 further reduces end-to-end duration to 299.286μs through 4-stage workspace pipeline improving Cube-Vector overlap while Vector computation duration remains basically unchanged, a total duration reduction of 48.71% compared to Scenario 0.


## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=1                                                       # Select execution scenario
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # Build the project
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `SCENARIO_NUM` | `0`, `1`, `2` | Scenario number: 0-Baseline single AIV, 1-Dual AIV no pipeline, 2-Dual AIV+4-stage pipeline |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution results
  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```
