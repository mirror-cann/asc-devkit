# MatrixTranspose Performance Tuning Example

## Overview

This example uses matrix transpose to demonstrate memory access optimization strategies in the Ascend C SIMT programming model. The example includes 1 matrix copy baseline version and 3 progressively optimized transpose kernel versions, starting from direct-index transpose, then introducing UB staging, global memory access coalescing, and UB Bank conflict reduction through padding, demonstrating the tuning path for matrix transpose in the SIMT programming model.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
├── matrix_transpose_practice
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // Build project file
│   ├── matrix_transpose.asc   // SIMT matrix transpose example implementation
│   └── README.md
```

## Example Description

- Computation Formula:

  $$
  output(x, y) = input(y, x)
  $$

  - input is the input matrix with shape [H,W] and data type float
  - output is the output matrix with shape [W,H] and data type float
- Example Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">copy_kernel / transpose_naive_kernel / transpose_coalesced_kernel / transpose_avoid_bank_conflicts_kernel</td></tr>
  </table>

## Example Implementation

### Case Implementation Description

This example implements different memory access strategies through 4 independent kernels, each corresponding to a specific case version.

| Case   | Implementation Characteristics                                   | Kernel Function Used                    | Optimization Features                         |
| ------ | ---------------------------------------------------------------- | --------------------------------------- | --------------------------------------------- |
| Case 0 | Read/write at the same coordinates, no transpose, contiguous GM read and write | copy_kernel                           | Matrix copy version (baseline)                |
| Case 1 | Directly compute output coordinates according to the transpose formula | transpose_naive_kernel                | Direct-index transpose version                |
| Case 2 | Stage tiles in UB and swap read/write directions for more contiguous GM access | transpose_coalesced_kernel            | UB staging + global memory coalescing         |
| Case 3 | Add padding in UB tiles to reduce bank conflicts                 | transpose_avoid_bank_conflicts_kernel | UB staging + global memory coalescing + UB Bank conflict avoidance |

#### Thread Block Layout

To facilitate comparison of performance differences across versions, this example only supports square matrices where the width and height are integer multiples of 32. All cases use identical thread block layouts:

- For 2D regular computations such as matrix transpose, the large matrix is decomposed into multiple local tile sub-blocks, with each block processing one local region, facilitating index computation and thread collaboration.
- Each tile is 32x32 in size, and one block processes one tile. The grid is configured as `(matrix_width/32, matrix_height/32, 1)`.
- The block is fixed at `(32, 32, 1)`, with a total of 32x32=1024 threads per block, and each thread processes only 1 element within the tile.
- The `threadIdx.x` direction has 32 threads corresponding to the column index within the tile; the `threadIdx.y` direction has 32 threads corresponding to the row index within the tile.

Based on the above partitioning, `blockIdx` locates the tile assigned to the current block, and `threadIdx` locates the element processed by the current thread within the tile. Adding the tile coordinates and the local coordinates within the tile yields the global coordinates `x_index` and `y_index` of the element in the original matrix; expanding in row-major layout then yields the linear index `index` in GM.

```cpp
int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
int index = x_index + width * y_index;
```

The figure below provides a more intuitive mapping illustration. The left side shows a 1024x1024 matrix divided into 32x32 tiles. The right side shows one tile that a block needs to process, with a size of 32x32.

<img src="./figures/blockMapping.png" width="60%">

Taking the green element in the figure as an example, its input coordinates in GM can be derived directly using built-in variables in the kernel.

- The tile coordinates of the green element are (2,1), corresponding to:
  - blockIdx.y = 2
  - blockIdx.x = 1
- The local coordinates of the green element within the tile are (2,29), corresponding to:
  - threadIdx.y = 2
  - threadIdx.x = 29

Substituting the variable values in this example:

- x_index = blockIdx.x x TILE_DIM + threadIdx.x = 1 x 32 + 29 = 61
- y_index = blockIdx.y x TILE_DIM + threadIdx.y = 2 x 32 + 2 = 66

Therefore, the input element coordinates processed by this thread in GM are:

- input[y_index,x_index] = input[66,61]

Substituting into the 1D address formula:

- index = x_index + width x y_index = 61 + 1024 x 66

### Performance Metrics Description

| Metric                | Description                                                                              |
| --------------------- | ---------------------------------------------------------------------------------------- |
| Task Duration(us)     | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time |
| aiv_time(us)          | Theoretical execution time of the task on the AI Vector Core, in microseconds            |
| aiv_total_cycles      | Total execution cycles on each AI Vector Core compute unit after the task is assigned    |
| aiv_vec_time(us)      | Duration of vec-type instructions (vector computation instructions), in microseconds      |
| aiv_vec_ratio         | Ratio of vec-type instruction cycles to total cycles                                     |
| aiv_scalar_time(us)   | Duration of scalar-type instructions (scalar computation instructions), in microseconds   |
| aiv_scalar_ratio      | Ratio of scalar-type instruction cycles to total cycles                                  |

Except for Task Duration, all other metrics in this example show the average values across all blocks.

### Case 0: Matrix Copy Version

**Example Goal**: Establish a latency baseline for the matrix copy scenario to provide a performance reference for subsequent transpose versions

**Core Implementation**:

- Each block processes one 32x32 tile
- Each thread processes 1 element within the tile
- The block locates the current tile based on `blockIdx`, and the thread locates an element within the tile based on `threadIdx`
- The thread reads `input[index]` from GM and directly writes back to `output[index]`

There is no coordinate swapping and no UB involvement in the entire process. This version does not involve matrix transpose. Adjacent threads in the same Warp have consistent GM read/write directions, and the memory access pattern is contiguous. This serves as the latency baseline for subsequent transpose versions.

**Key Code**:

```cpp
int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
int index = x_index + width * y_index;

output[index] = input[index];
```

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      24.777      |    1.054    |     1739.820     |      0.889      |     0.847     |        0.153        |      0.141      |

**Analysis**:

- The Task Duration of Case 0 is 24.777us, serving as the latency baseline for contiguous GM read/write scenarios
- Subsequent transpose versions need to complete coordinate swapping while staying as close to this baseline as possible

---

### Case 1: Direct-Index Transpose Version

**Optimization Goal**: Implement the most straightforward matrix transpose and observe the latency change caused by writing directly to transposed addresses

**Core Implementation**:

- Each block processes one 32x32 tile
- Each thread processes 1 element within the tile
- The thread first reads the input element from GM using the original coordinates, then computes the transposed output position and writes the data directly to the transposed GM address
- GM read direction is contiguous, GM write direction is non-contiguous

The figure below shows the data flow of Case 1, where the highlighted elements show the elements processed by one Warp when reading from and writing to GM. Threads in the same Warp read one row of elements from the input tile in GM and write back to one column of the output tile in GM. When reading GM input, adjacent threads access contiguous element addresses, which is contiguous read. When writing back to output, adjacent threads are scattered to different rows of the output matrix, which is non-contiguous write. Therefore, the core issue of this version is that the transposed write-back addresses are no longer contiguous, which typically significantly affects overall throughput.

<img src="./figures/case1.png" width="60%">

**Key Code**:

```cpp
int index_in = x_index + width * y_index;
int index_out = y_index + height * x_index;

output[index_out] = input[index_in];
```

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      60.477      |    3.516    |     5801.925     |      3.357      |     0.955     |        0.147        |      0.041      |

**Analysis**:

- Compared to the Case 0 copy baseline, Task Duration increases from 24.777us to 60.477us, approximately 2.44x the copy version
- The computation in direct-index transpose is minimal, but the transposed GM write-back becomes cross-row, non-contiguous access, so the end-to-end latency is significantly higher than the copy baseline
- GM reads in this version remain contiguous, but GM write-back addresses are non-contiguous, making it difficult to efficiently merge write requests from the same Warp, which is the main cause of the increased Task Duration

---

### Case 2: UB Staging + Global Memory Coalescing Transpose Version

**Optimization Goal**: Adjust the transpose write-back method through UB staging to make GM read/write more contiguous, reducing end-to-end latency

**Core Implementation**:

- Each thread reads 1 element from the tile in GM; one Warp reads one row of elements from a tile
- Write elements to the tile in UB using original coordinates; one Warp writes the read row to one row of the tile in UB
- After synchronization, each thread fetches data from UB; one Warp reads one column of elements from the tile in UB
- Write the fetched values back to the transposed positions in GM; one Warp writes the column of elements read from UB to one row of the output tile in GM

The figure below shows the data flow of Case 2, where the highlighted elements show the elements processed by one Warp when reading from and writing to GM. When reading GM input, the entire tile is copied to UB according to the GM layout. When writing to GM output, threads in one Warp read one column of elements from UB and write them back to their corresponding transposed positions.

<img src="./figures/case2.png" width="60%">

Unlike Case 1, where threads "directly write input elements to the transposed GM positions" causing adjacent threads to scatter across different rows of the output matrix, Case 2 first places data in UB, transferring the non-contiguous global write access to non-contiguous reads within UB. Therefore, the core benefit of this version is: although it adds one UB read/write and one synchronization, it achieves a "contiguous read, contiguous write" access pattern on the GM side, and the overall latency is typically significantly lower than Case 1.

**Key Code**:

```cpp
tile[threadIdx.y][threadIdx.x] = input[index_in];
asc_syncthreads();

x_index = blockIdx.y * TILE_DIM + threadIdx.x;
y_index = blockIdx.x * TILE_DIM + threadIdx.y;
int index_out = x_index + y_index * height;

output[index_out] = tile[threadIdx.x][threadIdx.y];
```

**Optimization Methods**:

- Use UB as a tile staging area, transferring the non-contiguous GM writes in Case 1 to UB-side access
- Swap the block coordinates of the output tile so that write-back to GM is closer to contiguous row-wise writing for the same Warp
- Use `asc_syncthreads()` to ensure the entire tile is written to UB before performing transpose-direction reads

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      35.945      |    1.814    |     2993.315     |      1.646      |     0.910     |        0.156        |      0.083      |

**Analysis**:

- Compared to the naive transpose in Case 1, Task Duration decreases from 60.477us to 35.945us, a reduction of approximately 40.6%, with overall performance improvement of approximately 1.68x
- Case 2 transfers the non-contiguous GM writes in Case 1 to UB-side access through UB staging, making both GM read and write more contiguous, so Task Duration decreases significantly
- Compared to the Case 0 copy baseline, Case 2 Task Duration is still approximately 45.1% higher. This gap mainly comes from the additional UB read/write, synchronization, and transpose-direction UB access overhead

---

### Case 3: UB Staging + Global Memory Coalescing + UB Bank Conflict Avoidance Transpose Version

**Optimization Goal**: Based on the global memory coalescing version, reduce bank conflicts during the transpose read phase through UB padding

**Core Implementation**:

- The GM-to-UB phase is identical to Case 2
- The only difference is the UB layout, changed from 32x32 to 32x33
- After synchronization, the UB-to-GM write-back is identical to Case 2
- This version does not change the algorithm path, nor the block and thread partitioning; it only adjusts the physical layout in UB

The following uses the UB partitioning rules of Ascend 950PR/Ascend 950DT as an example to explain how Bank Conflicts arise in this example and the theoretical conflict intensity differences between Case 2 and Case 3.

The bank partitioning in UB is shown in the figure below. The total UB size is 256KB, which can be viewed as two rows, each 128KB. The first 128KB corresponds to bank0 through bank7, and the second 128KB corresponds to bank8 through bank15. Bank0 and bank8 belong to the same group, bank1 and bank9 belong to the same group, and so on.

<img src="./figures/bank结构示意图.png">

For SIMT, the key concern is whether concurrent threads in the same Warp concentrate access on a few banks under the same UB access instruction.

- **Read-Write Conflict**: A read operation and a write operation simultaneously attempt to access the same bank.
- **Write-Write Conflict**: Multiple write operations simultaneously attempt to access the same bank group.
- **Read-Read Conflict**: Two read operations simultaneously attempt to access the same bank, or more than two read operations simultaneously attempt to access the same bank group.

Since the tiles in this example are small:

- `32x32x4B = 4096B`
- `32x33x4B = 4224B`

Much smaller than 128KB, so one tile typically falls within the first 128KB region. In this analysis, it can be approximated as only using `bank0~bank7`.

In Case 2, the first 10 rows of the UB tile array are stored in row-major order as shown below. For illustration purposes, only the first 10 rows are shown; the remaining rows follow the same pattern. Each row of 32 float elements spans exactly 4 banks, with the first element of each row marked in blue. In Case 2, threads in one Warp read one column of elements from the tile and write them back to GM output. When accessing UB, 32 threads concentrate on two banks, meaning one bank has 16 threads accessing simultaneously, generating a large number of read conflicts.

<img src="./figures/case2bank.png">

In Case 3, one column of padding is added to the tile array in UB, changing from 32 elements per row to 33 elements per row. The layout in UB is shown below. Since each row of 33 elements spans 5 banks, elements in the same column are distributed across different banks. In Case 3, when accessing UB, the access from 32 threads is distributed across 8 banks, meaning one bank has 4 threads accessing simultaneously, greatly reducing the conflict scale.

<img src="./figures/case3bank.png">

**Key Code**:

```cpp
__ubuf__ float tile[TILE_DIM][TILE_DIM + 1];

tile[threadIdx.y][threadIdx.x] = input[index_in];
asc_syncthreads();

x_index = blockIdx.y * TILE_DIM + threadIdx.x;
y_index = blockIdx.x * TILE_DIM + threadIdx.y;
int index_out = x_index + y_index * height;

output[index_out] = tile[threadIdx.x][threadIdx.y];
```

**Optimization Methods**:

- Add +1 padding to the UB tile, changing the per-row stride from 32 floats to 33 floats
- Change the bank distribution of same-column elements in UB to reduce the probability of concentrated access on a few banks during the transpose read phase of the same Warp

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      26.725      |    1.224    |     2018.943     |      1.059      |     0.869     |        0.152        |      0.121      |

**Analysis**:

- Compared to the transpose_coalesced_kernel in Case 2, Task Duration decreases from 35.945us to 26.725us, a reduction of approximately 25.7%, with overall performance improvement of approximately 1.35x
- Case 3 builds on Case 2 by reducing UB bank conflicts during the transpose read phase through padding, so the end-to-end Task Duration continues to decrease
- Compared to the naive transpose in Case 1, Case 3 Task Duration decreases by approximately 55.8%, with overall performance improvement of approximately 2.26x, demonstrating that "GM memory coalescing + UB bank conflict reduction" optimizations stack effectively on end-to-end latency
- Compared to the Case 0 copy baseline, Case 3 Task Duration is only approximately 7.9% higher, already close to the contiguous GM read/write baseline. The remaining gap mainly comes from UB staging, synchronization, and the small number of bank conflicts that still exist during UB read/write

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:

- From Case 1 direct-index transpose to Case 3 fully optimized version, Task Duration decreases from 60.477us to 26.725us, a reduction of approximately 55.8%, with overall performance improvement of approximately 2.26x
- Case 3 is only approximately 7.9% higher than the Case 0 copy baseline, indicating that through GM memory coalescing and UB Bank conflict optimization, matrix transpose is already close to the contiguous GM read/write baseline

| Case version | Task Duration(us) | Task Duration Relative to Case 0 | Optimization Points                          |
| ------------ | ----------------- | -------------------------------- | -------------------------------------------- |
| Case 0       | 24.777            | **1x**                     | Matrix copy baseline, contiguous GM read/write |
| Case 1       | 60.477            | **2.44x latency**           | Direct-index transpose, contiguous GM read, non-contiguous write |
| Case 2       | 35.945            | **1.45x latency**           | UB staging, global memory coalescing         |
| Case 3       | 26.725            | **1.08x latency**           | UB staging, global memory coalescing, UB Bank conflict avoidance |

## Tuning Recommendations

1. **Establish a copy baseline first**: When analyzing the performance of memory-access operators, measure the copy scenario latency first, then compare the actual operator performance against it.
2. **Prioritize GM memory access contiguity**: Matrix transpose computation is minimal; end-to-end latency is primarily affected by the read/write memory access pattern.
3. **Use UB staging to improve write-back patterns**: When direct transpose causes non-contiguous GM writes, transfer non-contiguous access to the UB side in exchange for contiguous GM read/write.
4. **Continue analyzing UB Bank conflicts**: After GM memory coalescing, UB bank conflicts during the transpose read phase may become the next bottleneck. Adjust the UB physical layout through padding or similar methods.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  SCENARIO_NUM=3                       # Select the execution scenario, options 0-3
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./matrix_transpose                   # Run the example
  ```

- Build Options Description

  | Option             | Values      | Description              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `0`-`3` | Example type, default is 3 |

  The following output indicates that the accuracy verification is successful.


  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msprof` tool to collect detailed performance data:

```bash
msprof op ./matrix_transpose   # Analyze case performance
```

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

```text
├──dump                       # Raw performance data, no user attention needed
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```
