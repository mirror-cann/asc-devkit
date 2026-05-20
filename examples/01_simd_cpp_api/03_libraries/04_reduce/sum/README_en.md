# Sum Example

## Overview

This example demonstrates the Sum high-level API, which computes the sum of elements along the last dimension of the input data. If the input is a one-dimensional vector, the sum of all elements in the vector is computed. If the input is a matrix, the sum of elements in each row along the last dimension is computed.

> **Interface Note:** In addition to the `Sum` interface used in this example, Ascend C provides the `Mean` interface for computing the average value. You can switch between them by replacing the function name:
> - **Mean**: Computes the mean. Replace `AscendC::Sum` with `AscendC::Mean` and `GetSumMaxMinTmpSize` with `GetMeanMaxMinTmpSize` in `sum.asc`.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── scripts
│   └── gen_data.py                   // Script to generate test data and ground truth data
├── CMakeLists.txt                    // Build project file
├── data_utils.h                      // Data read and write utility functions
└── sum.asc                           // Ascend C example instance & main program
```

## Example Specifications

- Example Function:
  This example computes the sum of elements in each row of a matrix.

<table>
<caption>Table 1: Example Specifications</caption>
<tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="center"> sum </td></tr>

<tr><td rowspan="3" align="left">Example Input</td></tr>
<tr><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
<tr><td align="left">src</td><td align="left">[3, 32]</td><td align="left">float</td><td align="left">ND</td></tr>

<tr><td rowspan="2" align="left">Example Output</td></tr>
<tr><td align="left">dst</td><td align="left">[8, 1]</td><td align="left">float</td><td align="left">ND</td></tr>
<tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="center">sum_custom</td></tr>
</table>

- Example Implementation:
  This example implements a fixed shape with input x[3, 32] and output y[8, 1], where y contains 3 valid values. After summing the elements of each row in the input x, the first 3 elements of the output y are valid data, and the last 5 elements are padding data.
  - Kernel Implementation:
    The computation logic is: this example moves the input data to on-chip storage, then uses the Sum high-level API interface to complete the summation, and finally moves the results out.

  - Tiling Implementation:
    The tiling implementation process for this example is as follows: use the GetSumMaxMinTmpSize interface to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

## Build and Run

Execute the following steps in the root directory of this example to build and run the example.

- Configure Environment Variables
  Select the appropriate command to configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on your current environment.
  - Default path, root user installed CANN software package
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, non-root user installed CANN software package
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, CANN software package installed
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Run the Example

  ```bash
  mkdir -p build && cd build;      # Create and enter build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Execute the compiled program to run the example
  ```

  When using CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, you need to clear the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Build Options
  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison passed.
  ```bash
  test pass!
  ```