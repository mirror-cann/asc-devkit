# DataCopy ub2l1 Example

## Overview

This example implements data transfer from UB (Unified Buffer) to L1 (L1 Buffer) based on DataCopy in a Mmad matrix multiplication scenario.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                   // Input data and ground truth generation script
│   ├── CMakeLists.txt                    // Build configuration file
│   ├── data_utils.h                      // Data read/write functions
│   ├── data_copy_ub2l1.asc               // Ascend C example implementation & invocation example
│   └── README.md                         // Example description document
```

## Example Description

- Example functionality:
  Transfers data from UB (Unified Buffer) to L1 (L1 Buffer), then performs Mmad matrix multiplication computation, and finally transfers the result to GM (Global Memory) via Fixpipe. Refer to [Inline Basic Data Transfer](../../../../../docs/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/矩阵数据搬入至L1-Buffer/UBToL1连续数据搬运（DataCopy）.md) for API documentation.
- Example specifications:
  <table>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_ub2l1</td></tr>
  </table>
- Example implementation:
  - Transfers data from GM (Global Memory) to UB (Unified Buffer).
  - Transfers data from UB (Unified Buffer) to L1 (L1 Buffer).
  - Calls the basic API LoadData to transfer data from L1 (L1 Buffer) to A2 (L0A Buffer) and B2 (L0B Buffer).
  - Calls the basic API Mmad to perform matrix multiplication computation.
  - Calls the basic API Fixpipe to transfer data from L0C Buffer to GM (Global Memory).
- Invocation implementation
  Uses the kernel invocation syntax <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run it.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project, default npu mode
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
