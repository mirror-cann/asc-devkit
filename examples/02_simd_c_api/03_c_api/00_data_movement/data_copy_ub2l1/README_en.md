# data_copy_ub2l1 Example

## Overview

This example uses the Ascend C C API to move matrix data from UB (Unified Buffer) to L1 (L1 Buffer), then moves data from L1 Buffer to L0A Buffer and L0B Buffer, performs Mmad matrix multiplication, and uses Fixpipe to move the L0C Buffer result to GM (Global Memory). It applies to Ascend 950PR/Ascend 950DT (`dav-3510`) and can run in NPU execution or NPU simulation mode.

## Supported Products and CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                // Generates input and golden data
│   │   └── verify_result.py           // Compares output with golden data
│   ├── CMakeLists.txt                 // CMake build file
│   ├── data_utils.h                   // Data read/write helper functions
│   ├── data_copy_ub2l1.asc            // C API implementation and launcher
│   ├── README.md                      // Chinese example documentation
│   └── README_en.md                   // English example documentation
```

## Scenario Description

Use the `SCENARIO_NUM` build parameter to select an input scenario. Both scenarios use the matrix multiplication shape [M, K, N] = [128, 128, 128], and the kernel name is `data_copy_ub2l1`.

**Table 1: SCENARIO_NUM Values**

| SCENARIO_NUM | Input format | Input type | Output type | UB-to-L1 processing |
|---|---|---|---|---|
| 1 | Nz | half | float | Contiguous copy |
| 2 | ND | half | float | Rearrange data into Nz format in UB, then copy contiguously |

`SCENARIO_NUM` is passed by CMake as a compile-time macro. The kernel selects the scenario with `if constexpr`. After changing the scenario, recompile the project.

**Scenario 1: Nz input, half input type**

- Input: A [128, 128] in `half` Nz format; B [128, 128] in `half` Nz format
- Output: C [128, 128] in `float` ND format
- Implementation: The AIV side first moves A and B from GM to UB with `asc_copy_gm2ub`. After `asc_sync_notify` and `asc_sync_wait` establish the MTE2-to-MTE3 dependency, `asc_copy_ub2l1` copies the Nz-format data contiguously to L1

**Scenario 2: ND input, half input type**

- Input: A [128, 128] in `half` ND format; B [128, 128] in `half` ND format
- Output: C [128, 128] in `float` ND format
- Implementation: The AIV side first moves A and B from GM to UB. After MTE2-to-V synchronization, `copy_ub_nd_to_nz` calls `asc_copy_ub2ub` for each C0 column block to rearrange ND-format data into Nz-format data. A and B use separate UB temporary buffers, avoiding read/write overlap between the two rearrangements and the following UB-to-L1 transfers. After V-to-MTE3 synchronization, `asc_copy_ub2l1` copies the Nz-format data contiguously to L1

### Data Flow and Synchronization

1. The AIV side calls `asc_copy_gm2ub` to move data from GM to UB with MTE2.
2. In scenario 1, the AIV side calls `asc_sync_notify` and `asc_sync_wait` to establish MTE2-to-MTE3 synchronization, then calls `asc_copy_ub2l1` to copy Nz-format data contiguously from UB to L1. In scenario 2, it first calls `asc_sync_notify` and `asc_sync_wait` to establish MTE2-to-V synchronization, then calls `copy_ub_nd_to_nz`, which calls `asc_copy_ub2ub` for each C0 column block to rearrange ND-format data in UB into Nz-format data. It then establishes V-to-MTE3 synchronization and calls `asc_copy_ub2l1` to copy the data contiguously from UB to L1.
3. The AIV side calls `asc_sync_block_arrive` to notify the AIC side, which waits for L1 data with `asc_sync_block_wait`.
4. The AIC side calls `asc_copy_l12l0a` and `asc_copy_l12l0b_transpose` to move Nz-format data from L1 Buffer to L0A Buffer and L0B Buffer, then calls `asc_sync_notify` and `asc_sync_wait` to establish MTE1-to-M synchronization before calling `asc_mmad` for matrix multiplication.
5. The AIC side calls `asc_sync_notify` and `asc_sync_wait` to establish M-to-FIX synchronization, then calls `asc_set_l0c2gm_nz2nd` to configure the conversion from Nz format to ND format and uses the Fixpipe API `asc_copy_l0c2gm` to move the L0C Buffer result to GM.

The required pipeline dependencies are established during processing. `asc_sync_pipe(PIPE_ALL)` is called at the end of the kernel to ensure that all pipelines complete.

## Build and Run

Run the following steps in the example root directory.

- Configure environment variables

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:**`${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

- Run the example

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate input and golden data
  ./demo                           # Run the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify the result
  ```

  Set `SCENARIO_NUM` and `-scenarioNum` to the same value: 1 or 2.

  To use NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim` to the CMake command:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
  ```

  > **Notice:**Clear the CMake cache before switching run modes or scenarios. Run `rm CMakeCache.txt` in the build directory and rerun CMake.

- Build options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number |

- Expected result

  A successful accuracy comparison prints:

  ```bash
  test pass!
  ```
