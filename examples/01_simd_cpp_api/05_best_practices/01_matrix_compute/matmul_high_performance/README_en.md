# Matmul Best Practice Example

## Overview

This example implements matrix multiplication based on the Matmul high-level API, demonstrating the complete tuning path from basic implementation to high-performance optimization through 9 progressive optimization cases, including single-core basic version, Tiling optimization, multi-core parallel splitting, MDL mode, L1Cache/L2Cache optimization, constant Tiling, UnitFlag optimization, and other optimization methods.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── matmul.h                // Header file definitions for all optimization cases
│   ├── matmul.asc              // Ascend C example implementation
│   └── README.md               // Example documentation
```

## Example Description

- Computation formula: C = A * B
  - A, B are source operands. A is the left matrix, shape [M, K]; B is the right matrix, shape [K, N]
  - C is the destination operand, storing the matrix multiplication result matrix, shape [M, N]

- Example specifications:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_custom</td></tr>
</table>

## Example Implementation

### Class Implementation Description

This example implements different optimization strategies through three independent classes, each corresponding to specific Case versions.

| Class Name | Corresponding Case | Implementation Characteristics | Kernel Function Used | Optimization Feature |
|------|---------|---------|-------------|---------|
| **MatmulKernel** | Case 0-5 | Basic implementation, runtime Tiling | matmul_custom (isMdl=false)<br>matmul_custom_mdl (isMdl=true) | - Case 0: Single-core basic version<br>- Case 1: Single-core Tiling optimization<br>- Case 2: Multi-core splitting 2x12<br>- Case 3: Multi-core splitting 4x6<br>- Case 4: MDL mode<br>- Case 5: MDL mode with L1Cache optimization |
| **MatmulKernelL2Cache** | Case 6 | L2Cache optimization, runtime Tiling | matmul_custom_mdl_l2cache | - MDL mode<br>- L1Cache optimization<br>- L2Cache optimization (A matrix M axis splitting) |
| **MatmulKernelMdlL2CacheConstant** | Case 7-8 | Constant Tiling, compile-time computation | matmul_custom_mdl_l2cache_constant (useUnitFlag=false, Case7)<br>matmul_custom_mdl_l2cache_constant_unitflag (useUnitFlag=true, Case8) | - MDL mode<br>- L1Cache optimization<br>- L2Cache optimization<br>- Constant Tiling (compile-time computation)<br>- Case 8: UnitFlag optimization |

#### 1. Tiling Mechanism Characteristics

- `MatmulKernel` and `MatmulKernelL2Cache` use the `TCubeTiling` type, requiring complete Tiling data structure copy from GM memory. Tiling parameters are computed by the Scalar unit at runtime
- `MatmulKernelMdlL2CacheConstant` uses a custom `MatmulProblemShape` structure containing only shape information (M, N, K, singleCoreM, etc.). Tiling parameters are already computed at compile time through `CONSTANT_CFG`, requiring no Scalar computation at runtime

#### 2. Process Method Computation Flow Characteristics

- **Computation Flow**: `MatmulKernel` single iteration, `MatmulKernelL2Cache` and `MatmulKernelMdlL2CacheConstant` loop 2 times (L2Cache optimization, A matrix M axis splitting)

### Performance Metric Description

| Metric | Description |
|------|------|
| Task Duration(μs) | Total execution time of the entire task, operator execution time is determined by this parameter |
| Block Num | Number of cores used (Block count) |
| aicore_time(μs) | Average AI Core execution time |
| aic_mac_time(μs) | Cube computation unit execution time |
| aic_mac_ratio | Cube computation unit time ratio, reflecting computation unit utilization |
| aic_scalar_time(μs) | Scalar computation unit execution time |
| aic_scalar_ratio | Scalar computation unit time ratio |
| aic_mte1_time(μs) | MTE1 (L1 to L0A/L0B transfer) execution time |
| aic_mte1_ratio | MTE1 time ratio, reflecting L1 to L0 data transfer pressure |
| aic_mte2_time(μs) | MTE2 (GM to L1 transfer) execution time |
| aic_mte2_ratio | MTE2 time ratio, reflecting GM to L1 data loading pressure |
| aic_fixpipe_time(μs) | FixPipe (L0C to GM transfer) execution time |
| aic_fixpipe_ratio | FixPipe time ratio, reflecting result write-back memory access pressure |

**Note**: The following Case performance change analysis uses A2 chip (Ascend 910B1) performance data as example. Ascend 950PR performance tuning data please refer to [below](#ascend-950pr-chip-performance-data).

### Case 0: Single-Core Basic Version (SINGLE_CORE_BASIC)

**Example Objective**: Implement basic Matmul functionality, ensuring computation correctness

**Core Implementation**:
- Uses single-core computation, **numBlocks=1**
- Basic Tiling configuration, **baseM=baseN=baseK=64**, base blocks are the basic blocks participating in one matrix multiplication instruction. baseM, baseN, baseK represent the M, N, K axis lengths on L0 during Matmul computation, in element units
- Transfer strategy: Input A, B matrices on GM are transferred in baseM×baseK, baseK×baseN blocks sequentially to L1, then from L1 to L0A/L0B. The Cube unit performs one baseM×baseN×baseK size Matmul computation for each transferred base block

**Key Code**:
```cpp
tilingApi.SetShape(M, N, K);
tilingApi.SetFixSplit(64, 64, 64);
tilingData.set_baseK(64);
```

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 759363.98 | 1 | 759363.46 | 141699.459 | 0.187 | 508787.509 | 0.67 | 162104.184 | 0.213 | 582515.828 | 0.767 | 34303.063 | 0.045 |

**Analysis**:
- Single-core computation time **759363.98μs**
- Computation unit utilization aic_mac_ratio only **18.7%**. This scenario is only used as a Matmul operation performance comparison example and is not recommended for users

### Case 1: Single-Core Tiling Optimization (SINGLE_CORE_TILING)

**Optimization Objective**: Optimize base block parameters in Tiling to improve single-core computation efficiency

**Core Implementation**:
- Uses single-core computation, **numBlocks=1**
- Uses optimized Tiling configuration, **baseM=128, baseN=256, baseK=64**

**Key Code**:
```cpp
tilingApi.SetShape(M, N, K);
tilingApi.SetFixSplit(128, 256, 64);
```

**Optimization Method**:
- **Base block selection principle**: In Case 0, the base block set in Tiling is [baseM, baseN, baseK] = [64, 64, 64], with high **memory-to-computation ratio** (the data volume needed per computation cycle). For the current large shape scenario, the base block selection principle is to minimize the memory-to-computation ratio, meaning with the same Cube computation volume, the required data volume for memory access is minimized.

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 249467.08 | 1 | 249466.54 | 81477.188 | 0.327 | 63608.818 | 0.255 | 52003.703 | 0.208 | 195613.432 | 0.784 | 11313.746 | 0.045 |

**Analysis**:
- Compared to Case 0, Task Duration decreased from 759363.98μs to 249467.08μs, reduced by **509896.90μs**, performance improved **3.04x**
- MTE2 data transfer duration decreased from 582515.828μs to 195613.432μs, reduced by **66.42%**
- Next optimization direction: Introduce multi-core parallel computation to fully utilize multi-core resources for improving overall throughput

### Case 2: Multi-Core Splitting 2x12 (MULTI_CORE_SPLIT_2_12)

**Optimization Objective**: Introduce multi-core parallel computation to improve overall throughput

**Core Implementation**:
- Multi-core parallel computation, splitting 8192×8192 matrix multiplication across 24 cores
- Splitting strategy: M direction 2 blocks, N direction 12 blocks, **singleM=4096, singleN=683, tail block tailN=679**

<img src="figures/2_12_split_core.png">

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 12541.22 | 24 | 12537.56 | 3462.802 | 0.276 | 2987.47 | 0.238 | 2260.551 | 0.18 | 10177.798 | 0.812 | 875.439 | 0.07 |

**Analysis**:
- Compared to Case 1, Task Duration decreased from 249467.08μs to 12541.22μs, performance improved **19.89x**. Multi-core parallel computation significantly improved overall throughput
- aic_mte2_time duration **10177.798μs**, ratio **81.2%**, becoming the performance improvement bottleneck
- Next optimization direction: The current multi-core splitting strategy does not satisfy 512B address alignment and does not evenly split M, N across cores. The splitting strategy will be optimized to improve address alignment and memory access efficiency

### Case 3: Multi-Core Splitting 4x6 (MULTI_CORE_SPLIT_4_6)

**Optimization Objective**: Optimize multi-core splitting strategy

**Core Implementation**:
- Multi-core parallel computation, splitting 8192×8192 matrix multiplication across 24 cores
- Splitting strategy: Even splitting of M, N. M direction 4 blocks, N direction 6 blocks

<img src="figures/4_6_split_core.png">

**Optimization Method**:
- Addresses satisfy **512B alignment**, setting **singleM=2048, singleN=1536, tail block tailN=512**, improving effective bandwidth utilization
- **Avoid same-address access**: Same-address access means multiple cores simultaneously reading the same row of A matrix need to access the same memory address. Hardware must serialize multiple accesses to the same address. The more cores with same-address access, the more severe the performance degradation from serialization. Compared to the 2×12 core splitting strategy in Case 2, Case 3's 4×6 splitting has smaller same-address conflict delay

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 12283.84 | 24 | 10870.96 | 3394.884 | 0.312 | 2656.33 | 0.244 | 2166.824 | 0.199 | 8616.671 | 0.793 | 488.829 | 0.045 |

**Analysis**:
- Compared to Case 2, Task Duration decreased from 12541.22μs to 12283.84μs, reduced by 2.05%
- aic_mte2_time decreased from 10177.798μs to 8616.671μs, reduced by **15.33%**, reduction of **1561.127μs**

### Case 4: Multi-Core Using MDL Template (MULTI_CORE_MDL)

**Optimization Objective**: Use MDL template, enable L1 multi-block cache functionality, enable "large packet" transfer, reduce MTE2 loop transfer count

**Core Implementation**:
- Uses 24-core parallel computation, splitting strategy same as above (4x6)
- Uses MDL template, enables "large packet" transfer functionality

**Optimization Method**:
- Enable MDL mode, supporting "large packet" transfer
- **Large packet transfer**: MTE2 GM to L1 transfer no longer transfers only one basic block at a time, but caches multiple basic blocks in L1, significantly reducing GM to L1 transfer count. In this example scenario, depthA1=4 with L1 transfer double buffer enabled, meaning L1 caches 4 baseM × baseK data blocks, with ping and pong buffers each loading two blocks during transfer

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 5039.86 | 24 | 4487.89 | 3116.472 | 0.694 | 2073.97 | 0.462 | 2332.914 | 0.52 | 4051.458 | 0.903 | 44.749 | 0.01 |

**Analysis**:
- Compared to Case 3, Task Duration decreased from 12283.84μs to 5039.86μs, duration reduced by **58.98%**
- aic_mte2_time decreased from 8616.671μs to 4051.458μs, MTE2 transfer time reduced by **52.97%**
- aic_mac_ratio improved from **31.2%** to **69.4%**

### Case 5: Multi-Core MDL + L1Cache Optimization (MULTI_CORE_MDL_L1CACHE)

**Optimization Objective**: Increase MTE2 transfer intensity, fully utilize L1 cache space

**Core Implementation**:
- Uses 24-core parallel computation, splitting strategy same as above (4x6)
- Uses MDL template, enables "large packet" transfer functionality
- Manually optimizes Tiling parameters to **depthA1=16, stepKa=8**, fully utilizing L1 cache space

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 4156.76 | 24 | 3925.61 | 3352.087 | 0.854 | 1464.492 | 0.373 | 2750.454 | 0.701 | 3778.555 | 0.963 | 47.853 | 0.012 |

**Analysis**:
- Compared to Case 4, Task Duration decreased from 5039.86μs to 4156.76μs, duration reduced by **17.52%**
- aic_mac_ratio improved from 69.4% to 85.4%, Cube utilization improved by **23.05%**
- aic_mte2_ratio reaches **96.3%**, reaching MTE2 bound, becoming the performance improvement bottleneck

### Case 6: Multi-Core MDL + L1Cache + L2Cache (MULTI_CORE_MDL_L1CACHE_L2CACHE)

**Optimization Objective**: Enable L2Cache optimization to mitigate MTE2 bound

**Core Implementation**:
- Uses 24-core parallel computation, splitting strategy same as above (4x6)
- Uses MDL template with optimized L1 Cache parameters same as above
- Enables `L2Cache`, splitting A matrix M axis

**Optimization Method**:
- **Enable L2Cache optimization**:
  - L2Cache characteristics: L2Cache is the shared external cache for AI Cores. L2Cache pure read bandwidth is approximately 3 to 4 times that of GM. When transferring the same data volume in or out, accessing data in L2Cache is faster than GM
  - Cache hit optimization: If data cannot hit L2Cache, requiring GM access for read/write, bandwidth utilization efficiency is low, potentially making MTE2 the performance bottleneck during the entire example run
  - **Block computation adapted to L2Cache**: Current L2Cache size is **192MB**, total data needed for matrix computation is **384MB** (A matrix 128MB + B matrix 128MB + C matrix 128MB). Since L2Cache capacity is less than total matrix computation data, the A matrix can be split into 2 parts along M axis, with region 1 completing full computation with B matrix, then region 2 completing full computation with B matrix, improving L2Cache hit rate through splitting

Block computation adapted to L2Cache diagram:

<img src="figures/L2Cache.png">

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 4088.36 | 24 | 3786.26 | 3254.31 | 0.86 | 1753.463 | 0.463 | 2680.849 | 0.708 | 3625.42 | 0.958 | 47.398 | 0.013 |

**Analysis**:
- Compared to Case 5, Task Duration decreased from 4156.76μs to 4088.36μs, reduced by **68.40μs**
- aic_mte2_time decreased from 3778.555μs to 3625.42μs, MTE2 transfer time reduced by **4.05%**
- aic_mac_ratio improved from **85.4%** to **86%**

### Case 7: Multi-Core MDL + L1Cache + L2Cache + Constants Tiling (MULTI_CORE_MDL_L1CACHE_L2CACHE_CONSTANTS)

**Optimization Objective**: Use constant Tiling to reduce runtime Scalar computation overhead

**Core Implementation**:
- Uses 24-core parallel computation, splitting strategy same as above (4x6)
- Uses MDL template with optimized L1 Cache parameters same as above
- L2 Cache strategy same as above
- Enables full Tiling constantification

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 4053.44 | 24 | 3665.12 | 3163.682 | 0.863 | 968.616 | 0.264 | 2609.617 | 0.712 | 3513.806 | 0.959 | 45.76 | 0.012 |

**Analysis**:
- Compared to Case 6, Task Duration decreased from 4088.36μs to 4053.44μs
- aic_scalar_time decreased from 1753.463μs to 968.616μs, reduced by **44.76%**
- Constant Tiling effectively reduces Scalar unit performance overhead

### Case 8: Multi-Core MDL + L1Cache + L2Cache + Constants Tiling + UnitFlag (MULTI_CORE_MDL_L1CACHE_L2CACHE_CONSTANTS_UNITFLAG)

**Optimization Objective**: Enable UnitFlag optimization, parallelize computation and transfer pipeline

**Core Implementation**:
- Uses 24-core parallel computation, splitting strategy same as above (4x6)
- Uses MDL template with optimized L1 Cache parameters same as above
- L2 Cache strategy same as above
- Enables full Tiling constantification same as above
- Enables UnitFlag, optimizing computation and transfer parallelism

**Optimization Method**:
- **Enable UnitFlag optimization**:
  - Optimize computation and transfer parallelism: Without UnitFlag enabled, AIC core MMAD computation instructions and FIXPIPE data transfer instructions are instruction-level synchronized. FIXPIPE instructions must wait for MMAD instructions to complete before transferring results, with MMAD and FIXPIPE pipeline serial. After enabling UnitFlag, MMAD and FIXPIPE achieve **512B size fine-grained synchronization**. During one MMAD instruction execution, whenever one 512B data result computation completes, FIXPIPE immediately transfers that 512B data, achieving Cube computation unit and FIXPIPE transfer unit pipeline parallelism

UnitFlag functionality diagram:

<img src="figures/unitflag_close.png">
<img src="figures/unitflag_open.png">

**Performance Data**:
| Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| 4012.44 | 24 | 3559.06 | 3076.396 | 0.864 | 1026.069 | 0.288 | 2533.512 | 0.712 | 3435.584 | 0.965 | 272.576 | 0.077 |

**Analysis**:
- Compared to Case 7, Task Duration decreased from 4053.44μs to 4012.44μs
- Current performance benefit is small because the performance constraint remains MTE2 bound, with the operator's MMAD and FIXPIPE pipeline masked by MTE2 bound

## Performance Comparison Summary

### Atlas A2 Training Series Chip Performance Data

**Comprehensive Optimization Effect**:
- This example Cube utilization improved by **67.7%** (18.7% → 86.4%), reaching **86.4%** of chip peak compute
- Through progressive optimization from Case 0 to Case 8, example duration decreased by **99.47%** (759363.98μs → 4012.44μs)

| Case version | Task Duration(μs) | End-to-End Duration vs Case 0 | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|----------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Case 0 | 759363.98 | **1x** | 1 | 759363.46 | 141699.459 | 0.187 | 508787.509 | 0.67 | 162104.184 | 0.213 | 582515.828 | 0.767 | 34303.063 | 0.045 |
| Case 1 | 249467.08 | **3.04x** | 1 | 249466.54 | 81477.188 | 0.327 | 63608.818 | 0.255 | 52003.703 | 0.208 | 195613.432 | 0.784 | 11313.746 | 0.045 |
| Case 2 | 12541.22 | **60.55x** | 24 | 12537.56 | 3462.802 | 0.276 | 2987.47 | 0.238 | 2260.551 | 0.18 | 10177.798 | 0.812 | 875.439 | 0.07 |
| Case 3 | 12283.84 | **61.82x** | 24 | 10870.96 | 3394.884 | 0.312 | 2656.33 | 0.244 | 2166.824 | 0.199 | 8616.671 | 0.793 | 488.829 | 0.045 |
| Case 4 | 5039.86 | **150.67x** | 24 | 4487.89 | 3116.472 | 0.694 | 2073.97 | 0.462 | 2332.914 | 0.52 | 4051.458 | 0.903 | 44.749 | 0.01 |
| Case 5 | 4156.76 | **182.68x** | 24 | 3925.61 | 3352.087 | 0.854 | 1464.492 | 0.373 | 2750.454 | 0.701 | 3778.555 | 0.963 | 47.853 | 0.012 |
| Case 6 | 4088.36 | **185.74x** | 24 | 3786.26 | 3254.31 | 0.86 | 1753.463 | 0.463 | 2680.849 | 0.708 | 3625.42 | 0.958 | 47.398 | 0.013 |
| Case 7 | 4053.44 | **187.34x** | 24 | 3665.12 | 3163.682 | 0.863 | 968.616 | 0.264 | 2609.617 | 0.712 | 3513.806 | 0.959 | 45.76 | 0.012 |
| Case 8 | 4012.44 | **189.25x** | 24 | 3559.06 | 3076.396 | 0.864 | 1026.069 | 0.288 | 2533.512 | 0.712 | 3435.584 | 0.965 | 272.576 | 0.077 |

### Ascend 950PR Chip Performance Data

| Case version | Task Duration(μs) | End-to-End Duration vs Case 0 | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|----------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Case 0 | 1096626.431 | **1x** | 1 | 1096625.66 | 198351.65 | 0.181 | 583195.222 | 0.532 | 115705.132 | 0.106 | 960571.993 | 0.876 | 28615.988 | 0.026 |
| Case 1 | 130560.475 | **8.40x** | 1 | 130559.56 | 88685.142 | 0.679 | 36462.067 | 0.279 | 22489.156 | 0.172 | 106793.342 | 0.818 | 4200.385 | 0.032 |
| Case 2 | 4294.619 | **255.35x** | 32 | 4293.9 | 2788.781 | 0.649 | 1149.24 | 0.268 | 707.592 | 0.165 | 3540.645 | 0.825 | 141.876 | 0.033 |
| Case 3 | 4332.557 | **253.11x** | 32 | 4331.82 | 2774.213 | 0.64 | 1143.276 | 0.264 | 703.896 | 0.162 | 3582.246 | 0.827 | 144.525 | 0.033 |
| Case 4 | 2668.224 | **410.99x** | 32 | 2667.49 | 2571.074 | 0.964 | 1377.736 | 0.516 | 799.378 | 0.3 | 2531.912 | 0.949 | 33.864 | 0.013 |
| Case 5 | 2591.366 | **423.18x** | 32 | 2590.51 | 2547.046 | 0.983 | 612.956 | 0.237 | 834.311 | 0.322 | 1926.358 | 0.744 | 35.44 | 0.014 |
| Case 6 | 2589.888 | **423.43x** | 32 | 2589.18 | 2547.518 | 0.984 | 765.125 | 0.296 | 826.429 | 0.319 | 1879.029 | 0.726 | 33.261 | 0.013 |
| Case 7 | 2589.09 | **423.55x** | 32 | 2588.38 | 2547.049 | 0.984 | 426.398 | 0.165 | 827.939 | 0.32 | 1895.165 | 0.732 | 33.648 | 0.013 |
| Case 8 | 2558.155 | **428.68x** | 32 | 2557.49 | 2549.657 | 0.997 | 412.29 | 0.161 | 835.579 | 0.327 | 1900.322 | 0.743 | 213.789 | 0.084 |

## Tuning Recommendations

1. **Start from small scale**: First use single-core basic version to verify functional correctness
2. **Optimize progressively**: Follow the case order to progressively introduce optimization methods and observe performance improvement
3. **Multi-core splitting strategy**: Set multi-core splitting strategies reasonably to avoid same-address access
4. **Utilize MDL mode**: MDL mode provides highly optimized implementation, prioritize its use
5. **L2Cache**: L2Cache can further mitigate MTE2 bound, recommended for data requiring repeated reads
6. **Constant Tiling**: For fixed shape scenarios, use constant Tiling to reduce Scalar runtime overhead
7. **UnitFlag optimization**: Enabling UnitFlag can parallelize computation and transfer pipeline

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=0                       # Select execution scenario
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # Build the project (default npu mode)
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;   # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `SCENARIO_NUM` | `0`-`8` | Example type (0-8), default is 0 |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```

## Performance Analysis

Use the `msprof` tool to obtain detailed performance data:

```bash
msprof ./demo   # Analyze case performance
```

A PROF_ prefixed folder is generated in the current directory. The `mindstudio_profiler_output` directory stores Host and Device performance data summaries. Performance data analysis is recommended to view files in this directory.
```bash
PROF_xxxx_XXXXXX
├── device_{id}
└── host
└── mindstudio_profiler_log
└── mindstudio_profiler_output    # Stores Host and Device performance data summaries
    ├── msprof_*.json
    ├── xx_*.csv
    └── README.txt
```
