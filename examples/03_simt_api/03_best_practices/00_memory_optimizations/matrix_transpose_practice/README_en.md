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
| Case 3 | Add padding in UB tiles to avoid bank conflicts                  | transpose_avoid_bank_conflicts_kernel | UB staging + global memory coalescing + UB Bank conflict avoidance |

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
| Task Duration(μs)     | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time |
| aiv_time(μs)          | Theoretical execution time of the task on the AI Vector Core, in microseconds            |
| aiv_total_cycles      | Total execution cycles on each AI Vector Core compute unit after the task is assigned    |
| aiv_vec_time(μs)      | Duration of vec-type instructions (vector computation instructions), in microseconds      |
| aiv_vec_ratio         | Ratio of vec-type instruction cycles to total cycles                                     |
| aiv_scalar_time(μs)   | Duration of scalar-type instructions (scalar computation instructions), in microseconds   |
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

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      21.751      |    0.919    |     1515.731     |      0.743      |     0.817     |        0.164        |      0.169      |

**Analysis**:

- The Task Duration of Case 0 is 21.751μs, serving as the latency baseline for contiguous GM read/write scenarios
- Subsequent transpose versions need to complete coordinate swapping while staying as close to this baseline as possible

---

### Case 1: Direct-Index Transpose Version

**Optimization Goal**: Implement the most straightforward matrix transpose and observe the latency change caused by writing directly to transposed addresses

**Core Implementation**:

- Each block processes one 32x32 tile
- Each thread processes 1 element within the tile
- The thread first reads the input element from GM using the original coordinates, then computes the transposed output position and writes the data directly to the transposed GM address
- GM read direction is contiguous, GM write direction is non-contiguous

The figure below shows the data flow of Case 1, where the highlighted elements show the elements processed by one Warp when reading from and writing to GM. Threads in the same Warp read one row of elements from the input tile in GM and write back to one column of the output tile in GM. When reading GM input, adjacent threads access contiguous element addresses, which is contiguous read. When writing back to output, adjacent threads access addresses scattered across different rows of the output matrix, which is non-contiguous write. Therefore, the core issue of this version is that the transposed write-back addresses are no longer contiguous, affecting overall throughput.

<img src="./figures/case1.png" width="60%">

**Key Code**:

```cpp
int index_in = x_index + width * y_index;
int index_out = y_index + height * x_index;

output[index_out] = input[index_in];
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      61.508      |    3.609    |     5954.297     |      3.441      |     0.954     |        0.156        |      0.043      |

**Analysis**:

- Compared to the Case 0 copy baseline, Task Duration increases from 21.751μs to 61.508μs, approximately 2.83x the copy version
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

Unlike Case 1, where threads "directly write input elements to the transposed GM positions" causing adjacent threads to scatter across different rows of the output matrix, Case 2 first places data in UB, transferring the non-contiguous global write access to non-contiguous reads within UB. Therefore, the core benefit of this version is: although it adds one UB read/write and one synchronization, it achieves a "contiguous read, contiguous write" access pattern on the GM side, and the overall latency is significantly lower than Case 1.

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

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      31.926      |    1.606    |     2650.112     |      1.442      |     0.900     |        0.152        |      0.092      |

**Analysis**:

- Compared to the naive transpose in Case 1, Task Duration decreases from 61.508μs to 31.926μs, a reduction of approximately 48.1%, with overall performance improvement of approximately 1.93x
- Case 2 transfers the non-contiguous GM writes in Case 1 to UB-side access through UB staging, making both GM read and write more contiguous, so Task Duration decreases significantly
- Compared to the Case 0 copy baseline, Case 2 Task Duration is still approximately 46.8% higher. Case 2 introduces additional UB read/write, synchronization, and transpose-direction UB access overhead compared with the copy version

---

### Case 3: UB Staging + Global Memory Coalescing + UB Bank Conflict Avoidance Transpose Version

**Optimization Goal**: Based on the global memory coalescing version, avoid bank conflicts during the transpose read phase through UB padding

**Core Implementation**:

- The GM-to-UB phase is identical to Case 2
- The only difference is the UB layout, changed from 32x32 to 32x34
- After synchronization, the UB-to-GM write-back is identical to Case 2
- This version does not change the algorithm path, nor the block and thread partitioning; it only adjusts the physical layout in UB

The following uses the UB partitioning rules of Ascend 950PR/Ascend 950DT as an example to explain how bank conflicts arise in this example and the theoretical conflict intensity differences between Case 2 and Case 3.

The bank partitioning in UB is shown in the figure below. The total UB size is 256KB, divided into 16 banks. Each bank contains 512 rows, each row is 32B, for a total of 16KB per bank. These 16 banks are further organized into 8 bank groups. Each bank group contains 2 banks: bank i and bank i+8 belong to bank group i, namely `bank_group_id = bank_id % 8`. In SIMT programming mode, each bank is further divided into 4 subbanks, and each subbank has a width of 8B.

<img src="./figures/bank结构示意图.png">

In SIMT scenarios, multiple threads in the same Warp may access UB concurrently within the same UB access instruction. When the accessed data belongs to subbank resources with the same index in the same bank group, the hardware cannot process all requests in one cycle, so the requests must queue, forming subbank conflicts and increasing access latency. If the accessed data belongs to the same 8B address range in the same row of the same subbank, the hardware merges these requests and no subbank conflict is formed.

**Low-bit interleaving is used for addresses:** As shown in the following figure, consecutive UB addresses are mapped to bank0 through bank15 at 32B granularity. The first 32B address segment maps to bank0, the second maps to bank1, and so on. The 16th segment maps to bank15, and the 17th segment maps back to the next row of bank0.

<img src="./figures/bank内存排布示意图.png">

The main subbank conflict types in SIMT programming are as follows:

- **Write-Write Conflict**: Multiple write operations simultaneously attempt to access subbanks with the same index in the same bank group.
- **Read-Read Conflict**: Multiple read operations simultaneously attempt to access subbanks with the same index in the same bank group.

In Case 2, the first 10 rows of the UB tile array are stored in row-major order as shown below. According to the low-bit address interleaving rule, row 1 of the tile array covers bank 0 through bank 3, row 2 covers bank 4 through bank 7, row 3 covers bank 8 through bank 11, and the remaining rows follow the same pattern. For illustration purposes, only the first 10 rows are shown. Each row of 32 float elements spans exactly 4 banks, with the first element of each row marked in blue. In Case 2, one Warp reads one column of the tile and writes it back to GM output. When accessing UB, the 32 threads concentrate on subbank 0 in two bank groups, which is a read-read conflict scenario.

<img src="./figures/case2bank.png">

In Case 3, two columns of padding are added to the tile array in UB, changing from 32 elements per row to 34 elements per row. The layout in UB is shown below. Since each row has 34 elements, the row stride becomes 17 subbanks, so elements in the same column are staggered across different subbanks. When Case 3 accesses UB, the accesses from 32 threads are distributed across each subbank of each bank group. In other words, each subbank is accessed by only one thread under the same access instruction, avoiding the read-read conflict described above.

<img src="./figures/case3bank.png">

**Key Code**:

```cpp
__ubuf__ float tile[TILE_DIM][TILE_DIM + 2];

tile[threadIdx.y][threadIdx.x] = input[index_in];
asc_syncthreads();

x_index = blockIdx.y * TILE_DIM + threadIdx.x;
y_index = blockIdx.x * TILE_DIM + threadIdx.y;
int index_out = x_index + y_index * height;

output[index_out] = tile[threadIdx.x][threadIdx.y];
```

**Optimization Methods**:

- Add +2 padding to the UB tile, changing the per-row stride from 32 floats to 34 floats
- Change the bank and subbank distribution of same-column elements in UB to avoid read-read conflicts during the transpose read phase of the same Warp

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.483      |    1.055    |     1740.038     |      0.883      |     0.847     |        0.151        |      0.138      |

**Analysis**:

- Compared to the transpose_coalesced_kernel in Case 2, Task Duration decreases from 31.926μs to 23.483μs, a reduction of approximately 26.4%, with overall performance improvement of approximately 1.36x
- Case 3 builds on Case 2 by avoiding UB bank conflicts during the transpose read phase through padding, so the end-to-end Task Duration continues to decrease
- Compared to the naive transpose in Case 1, Case 3 Task Duration decreases by approximately 61.8%, with overall performance improvement of approximately 2.62x, demonstrating that "GM memory coalescing + UB bank conflict avoidance" optimizations stack effectively on end-to-end latency
- Compared to the Case 0 copy baseline, Case 3 Task Duration is only approximately 8.0% higher, already close to the contiguous GM read/write baseline. Compared with the copy version, Case 3 still needs UB staging, synchronization, and UB transpose reads. The summary metrics alone do not further break down the remaining gap.

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:

- From Case 1 direct-index transpose to Case 3 fully optimized version, Task Duration decreases from 61.508μs to 23.483μs, a reduction of approximately 61.8%, with overall performance improvement of approximately 2.62x
- Case 3 is only approximately 8.0% higher than the Case 0 copy baseline, indicating that through GM memory coalescing and UB bank conflict avoidance, matrix transpose is already close to the contiguous GM read/write baseline

| Case version | Task Duration(μs) | Task Duration Relative to Case 0 | Optimization Points                          |
| ------------ | ----------------- | -------------------------------- | -------------------------------------------- |
| Case 0       | 21.751            | **1x**                     | Matrix copy baseline, contiguous GM read/write |
| Case 1       | 61.508            | **2.83x latency**           | Direct-index transpose, contiguous GM read, non-contiguous write |
| Case 2       | 31.926            | **1.47x latency**           | UB staging, global memory coalescing         |
| Case 3       | 23.483            | **1.08x latency**           | UB staging, global memory coalescing, UB Bank conflict avoidance |

## Tuning Recommendations

1. **Establish a copy baseline first**: When analyzing the performance of memory-access operators, measure the copy scenario latency first, then compare the actual operator performance against it.
2. **Prioritize GM memory access contiguity**: Matrix transpose computation is minimal; end-to-end latency is primarily affected by the read/write memory access pattern.
3. **Use UB staging to improve write-back patterns**: When direct transpose causes non-contiguous GM writes, transfer non-contiguous access to the UB side in exchange for contiguous GM read/write.
4. **Continue analyzing UB Bank conflicts**: After GM memory coalescing, UB bank conflicts during the transpose read phase may become the next bottleneck. Adjust the UB physical layout through padding, adjusting the row stride, or reordering data to avoid bank conflicts.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
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

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./matrix_transpose   # Analyze case performance
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
