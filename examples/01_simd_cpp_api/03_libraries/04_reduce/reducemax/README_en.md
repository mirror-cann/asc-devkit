# ReduceMax Example

## Overview

This example demonstrates the ReduceMax high-level API in a reduction scenario to compute the maximum value of each column in a matrix.

> **Interface Note:** In addition to the `ReduceMax` interface used in this example, Ascend C provides the following reduction interfaces. You can switch between them by replacing the function name:
> - **ReduceMin**: Computes the minimum value. Replace `AscendC::ReduceMax` with `AscendC::ReduceMin` and `GetReduceMaxMaxMinTmpSize` with `GetReduceMinMaxMinTmpSize` in `reducemax.asc`.
> - **ReduceMean**: Computes the average value. Replace `AscendC::ReduceMax` with `AscendC::ReduceMean` and `GetReduceMaxMaxMinTmpSize` with `GetReduceMeanMaxMinTmpSize` in `reducemax.asc`.
> - **ReduceSum**: Computes the sum. Replace `AscendC::ReduceMax` with `AscendC::ReduceSum` and `GetReduceMaxMaxMinTmpSize` with `GetReduceSumMaxMinTmpSize` in `reducemax.asc`.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── reducemax
│   ├── scripts
│   │   └── gen_data.py             // Script to generate input data and ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read and write functions
│   └── reducemax.asc               // Ascend C example implementation & invocation example
```

## Example Description

- Example Function:
  This example computes the maximum value of each column in the input matrix.

- Example Specifications:
<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> reducemax </td></tr>

<tr><td rowspan="3" align="center">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[32, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="2" align="center">Example Output</td></tr>
<tr><td align="center">y</td><td align="center">[1, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_custom</td></tr>
</table>

- Example Implementation:
  This example computes the maximum value of each column in the input [32, 136] matrix and outputs [1, 136].

  - Kernel Implementation:
    The computation logic is: input data must first be moved to on-chip storage, then the ReduceMax high-level API interface is used to complete the computation, and finally the results are moved out.

  - Tiling Implementation:
    The tiling implementation process for this example is as follows: use the GetReduceMaxMaxMinTmpSize interface to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

  - Invocation Implementation:
    Use the kernel call operator <<<>>> to invoke the kernel function.

## Build and Run

Execute the following steps in the root directory of this example to build and run the example.

- Configure Environment Variables
  Select the appropriate command to configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md) of the CANN development kit on your current environment.
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
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # Default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2/A3 series, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison passed.
  ```bash
  test pass!
  ```