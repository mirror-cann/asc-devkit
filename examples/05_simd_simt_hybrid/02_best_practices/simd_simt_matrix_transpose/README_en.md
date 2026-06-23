# MatrixTranspose Example

## Overview

This example uses matrix transpose to demonstrate memory access coalescing optimization in Ascend C SIMD and SIMT hybrid programming scenarios. The example includes 2 case versions, starting from direct index transpose, then adjusting the data write-back method through UB intermediary to make GM reads and writes closer to contiguous access, showcasing global memory access optimization methods for matrix transpose in hybrid programming.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
├── simd_simt_matrix_transpose
│   ├── CMakeLists.txt         // Build project file
│   ├── matrix_transpose.asc   // Matrix transpose example implementation
│   ├── figures                // Image resources for README
│   └── README.md
```

## Example Description

- Computation formula:

  $$
  output(x, y) = input(y, x)
  $$

  - input is the input matrix with shape [H,W] and data type float
  - output is the output matrix with shape [W,H] and data type float
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transpose_kernel</td></tr>
  </table>

## Example Implementation

### Case Implementation Description

This example implements different memory access strategies by calling different SIMT VF functions. Each SIMT VF function corresponds to a specific case version.

| Case   | Implementation Characteristics                                           | SIMT VF Function Called               | Optimization Feature              |
| ------ | -------------------------------------------------- | -------------------------- | --------------------- |
| Case 0 | Directly compute output coordinates according to the transpose formula. GM contiguous read, non-contiguous write   | simt_transpose_naive     | Direct index transpose version      |
| Case 1 | Transfer tiles through UB and swap read/write directions. GM reads and writes are closer to contiguous access | simt_transpose_coalesced | UB intermediary + global memory access coalescing |

#### Thread Block Layout

This example uses SIMD and SIMT hybrid programming. The kernel function calls `__simt_vf__` functions via `asc_vf_call`. To facilitate performance comparison across versions, this example only supports square matrices, and the matrix width and height must be integer multiples of 32. All cases use an identical thread block layout:

- This example flattens 2D tile coordinates into 1D `blockIdx.x` and flattens the 32x32 elements within a tile into 1D `threadIdx.x`.
- Each tile is 32x32, and one block processes one tile. The block count is `bn = grid_x * grid_y`, where `grid_x = matrix_width / TILE_DIM` and `grid_y = matrix_height / TILE_DIM`.

Based on the above partitioning, `blockIdx.x` represents the flattened 1D tile index. By dividing and taking modulo with `grid_width`, the 2D tile coordinates `(block_row, block_col)` for the current block can be restored. `threadIdx.x` represents the flattened 1D element index within the tile. By dividing and taking modulo with `TILE_DIM`, the local coordinates `(tile_row, tile_col)` of the element processed by the thread within the tile can be restored.

```cpp
int block_row = blockIdx.x / grid_width;
int block_col = blockIdx.x % grid_width;

int tile_row = threadIdx.x / TILE_DIM;
int tile_col = threadIdx.x % TILE_DIM;
```

The following figure provides a more intuitive mapping illustration. The left side shows the 1024x1024 matrix divided into 32x32 tiles. The right side shows one tile that a block needs to process, with a size of 32x32.

<img src="./figures/blockMapping.png" width="60%">

Taking the green element in the figure as an example, its input coordinates in GM can be directly derived using the built-in variables in the kernel.

- The tile coordinates are (2,1), corresponding to:
  - block_row = 2
  - block_col = 1
  - blockIdx.x = block_row x grid_width + block_col = 2 x 32 + 1 = 65
- The local coordinates of the element within the tile are (2,29), corresponding to:
  - threadIdx.x = tile_row x TILE_DIM + tile_col = 2 x 32 + 29 = 93

Combined with the index computation in the code:

```cpp
int input_row = block_row * TILE_DIM + tile_row;
int input_col = block_col * TILE_DIM + tile_col;
int index_in = input_col + width * input_row;
```

Substituting the variable values in this example:

- block_row = 65 / 32 = 2
- block_col = 65 % 32 = 1
- tile_row = 93 / 32 = 2
- tile_col = 93 % 32 = 29
- input_row = block_row x TILE_DIM + tile_row = 2 x 32 + 2 = 66
- input_col = block_col x TILE_DIM + tile_col = 1 x 32 + 29 = 61

Therefore, the input element coordinates processed by this thread in GM are:

- input[input_row,input_col] = input[66,61]

Substituting the 1D address formula:

- index_in = input_col + width x input_row = 61 + 1024 x 66

### Performance Metric Description

| Metric                | Description                                                                                      |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)|Total task latency, including scheduling time to the accelerator, execution time on the accelerator, and response completion time.|
| aiv_time|Theoretical task execution time on the AI Vector Core, in μs.|
| aiv_vec_time(μs) | vec-type instruction (vector operation instruction) latency, in μs. |
| aiv_vec_ratio | Ratio of vec-type instruction (vector operation instruction) cycles to total cycles. |
| aiv_scalar_time(μs) | scalar-type instruction (scalar operation instruction) latency, in μs. |
| aiv_scalar_ratio | Ratio of scalar-type instruction (scalar operation instruction) cycles to total cycles. |
| aiv_mte2_time(μs) | mte2-type instruction (GM->UB transfer instruction) latency, in μs. |
| aiv_mte2_ratio | Ratio of mte2-type instruction (GM->UB transfer instruction) cycles to total cycles. |
| aiv_mte3_time(μs) | mte3-type instruction (UB->GM transfer instruction) latency, in μs. |
| aiv_mte3_ratio | Ratio of mte3-type instruction (UB->GM transfer instruction) cycles to total cycles. |

### Case 0: Direct Index Transpose Version

**Example Objective**: Implement basic matrix transpose functionality as a latency comparison baseline for subsequent optimized versions.

**Core Implementation**:

- Each block processes one 32x32 tile.
- Each SIMT thread processes 1 element within the tile.
- The thread first reads the input element from GM using the original coordinates, then computes the transposed output position, and writes the data directly to the transposed GM address.
- GM read direction is contiguous; GM write-back direction is non-contiguous.

The following figure shows the data flow of Case 0, with red highlighting the elements processed by one Warp when reading from and writing to GM. Threads in the same Warp read one row of elements from the input tile in GM and write them back to one column of the output tile in GM. When reading the GM input, adjacent threads access contiguous element addresses, which is contiguous reading. When writing back to the output, adjacent threads are scattered across different rows of the output matrix, which is non-contiguous writing. Therefore, the core issue of this version is that the transposed write-back addresses are no longer contiguous, which typically significantly affects overall throughput.

<img src="./figures/case0.png" width="60%">

**Key Code**:

```cpp
int index_in = input_col + width * input_row;
int index_out = input_row + height * input_col;

output[index_out] = input[index_in];
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 63.731 | 62.810 | 58.143 | 0.926 | 2.439 | 0.039 | 0.059 | 0.001 | 0.02 | 0.0 |

**Analysis**:

- Case 0's Task Duration is 63.731μs. As the direct index transpose version, it serves as the comparison baseline for subsequent optimized versions.
- In this version, GM reads are still contiguous, but GM write-back is cross-row and non-contiguous. Write requests from the same Warp are difficult to coalesce efficiently, so the overall latency is primarily limited by the write-back memory access pattern.

---

### Case 1: UB Intermediary + Global Memory Access Coalescing Transpose Version

**Optimization Objective**: Adjust the transpose write-back method through UB intermediary to make GM reads and writes closer to contiguous access, reducing end-to-end latency.

**Core Implementation**:

- Transfer the input data tile from GM to UB.
- Each thread reads from UB. One Warp reads one column of elements from the tile in UB.
- Write the read values back to the transposed positions in GM. One Warp writes one column of elements read from UB to one row of the output tile in GM.

The following figure shows the data flow of Case 1, with red and yellow highlighting the elements processed by one Warp's threads when reading from and writing to GM. When reading the GM input, the entire tile is transferred to UB following the GM layout. When writing to the GM output, one Warp's threads read one column of elements from UB and write them back to their corresponding transposed positions.

<img src="./figures/case1.png" width="60%">

Unlike Case 0, where threads "directly write input elements to the transposed GM position" causing adjacent threads to be scattered across different rows of the output matrix, Case 1 first transfers the input tile to UB following the GM layout via `asc_copy_gm2ub_align`, then SIMT threads read from UB in the transposed direction and write back to GM. Therefore, the core benefit of this version is: transferring the originally non-contiguous GM write-back to transposed reads on the UB side, achieving a "contiguous read, contiguous write" access pattern on the GM side, with significantly lower overall latency than Case 0.

**Key Code**:

```cpp
uint32_t tile_row = threadIdx.x / TILE_DIM;
uint32_t tile_col = threadIdx.x % TILE_DIM;

output_tile[tile_col + tile_row * height] = input_tile[tile_col * TILE_DIM + tile_row];
```

**Optimization Methods**:

- Use UB as a tile intermediary area, converting the non-contiguous GM writes in Case 0 to transposed reads on the UB side.
- Adjust the block coordinates of the output tile so that when writing back to GM, the same Warp writes more closely in row-contiguous fashion.
- Use `asc_sync_notify()` / `asc_sync_wait()` to ensure that the GM-to-UB transfer is complete before launching SIMT VF processing.

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 30.602 | 29.6 | 16.942 | 0.572 | 3.404 | 0.115 | 6.912 | 0.234 | 0.022 | 0.001 |

**Analysis**:

- Compared with the direct index transpose version in Case 0, Case 1's Task Duration decreases from 63.731μs to 30.602μs, a latency reduction of approximately 52.0%.
- In terms of Task Duration, Case 1 achieves approximately 2.08x the performance of Case 0, indicating that improving GM write-back contiguity through UB intermediary results in significant end-to-end latency improvement.
- Case 1 reduces the costly non-contiguous GM write-back overhead from Case 0, but the code still includes tile transfer from GM to UB, pipeline synchronization, and transposed-direction UB reads by SIMT threads. Therefore, the optimized Task Duration will not be equivalent to the ideal latency of purely contiguous GM reads and writes.

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:

- Through the memory access coalescing optimization from Case 0 to Case 1, the example Task Duration decreases from 63.731μs to 30.602μs, a latency reduction of approximately 52.0%.
- Case 1 achieves approximately 2.08x performance improvement relative to Case 0, indicating that improving GM write-back contiguity through UB intermediary yields significant end-to-end latency benefits.

| Case version | Task Duration(μs) | End-to-End Performance Relative to Case 0 | Optimization Point                           |
| ------------ | ----------------- | -------------------- | -------------------------------- |
| Case 0       | 63.731            | **1x**         | Direct index transpose, GM contiguous read, non-contiguous write |
| Case 1       | 30.602            | **2.08x**      | UB intermediary, global memory access coalescing             |

## Tuning Recommendations

1. **Prioritize GM memory access contiguity**: Matrix transpose has very low computation volume. End-to-end latency is primarily affected by read/write memory access patterns.
2. **Use UB intermediary to improve write-back patterns**: When direct transpose causes non-contiguous GM writes, transfer the non-contiguous access to the UB side in exchange for contiguous reads and writes on the GM side.
3. **Pay attention to transfer and pipeline synchronization overhead**: UB intermediary requires transferring from GM to UB and ensuring tile data is available to SIMT threads through pipeline synchronization. When optimizing, balance the GM memory access benefits against UB transposed read and pipeline synchronization overhead.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=0                       # Select the execution scenario, options: 0-1
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./demo                               # Run the example
  ```

- Build option description

  | Option             | Values      | Description              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `0`-`1` | Example type, default is 0 |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msprof` tool to obtain detailed performance data:

```bash
msprof ./demo   # Analyze case performance
```

A PROF_-prefixed folder is generated in the current directory. The `mindstudio_profiler_output` directory contains the performance data summary for the Host and each Device. For performance data analysis, it is recommended to view the files in this directory.

```bash
PROF_xxxx_XXXXXX
├── device_{id}
└── host
└── mindstudio_profiler_log
└── mindstudio_profiler_output    # Performance data summary for Host and each Device
    ├── msprof_*.json
    ├── xx_*.csv
    └── README.txt
```

View the specific performance analysis results:

```bash
# View Task Duration and various metrics
cat ./PROF_*/mindstudio_profiler_output/op_summary_*.csv
```
