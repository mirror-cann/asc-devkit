# pattern_transformation Compatibility Sample

## Overview

This sample demonstrates the fractal transformation logic for the L1 Buffer to L0A Buffer path, based on the basic mmad sample, isolating different hardware implementations through compile-time macros.

- In Atlas A2/A3 series products, the data layout in L0A Buffer is Zz fractal. When transferring from L1 Buffer to L0A Buffer, Nz to Zz fractal transformation is required.
- In Ascend 950PR/Ascend 950DT, the data layout in L0A Buffer is changed to Nz fractal, which is consistent with L1 Buffer, and fractal transformation is no longer required.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 training series products/Atlas A3 inference series products
- Atlas A2 training series products/Atlas A2 inference series products

## Directory Structure

```
├── pattern_transformation
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and golden data
│   │   └── verify_result.py    // Script for verifying output data matches golden data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   └── pattern_transformation.asc         // AscendC operator implementation & call sample
```

## Sample Specifications

<table>
<caption>Sample Specification Table</caption>
<tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="3" align="center">Sample Input</td></tr>
<tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Sample Output</td><td align="center">z</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">pattern_transformation</td></tr>
</table>

## Sample Implementation

<table>
<tr><th align="left">Step</th><th align="left">Operation</th><th align="left">Function</th><th align="left">Atlas A2/A3 Fractal Transformation</th><th align="left">Ascend 950PR/950DT Fractal Transformation</th></tr>
<tr><td align="left">1</td><td align="left">CopyIn</td><td align="left">Copy matrices A and B from GM to L1 Buffer</td><td align="left">ND -> Nz</td><td align="left">ND -> Nz</td></tr>
<tr><td align="left">2</td><td align="left">DataLoadA</td><td align="left">Copy matrix A from L1 Buffer to L0A Buffer</td><td align="left">Nz -> Zz (LoadData2DParams)</td><td align="left">Nz -> Nz (LoadData2DParamsV2)</td></tr>
<tr><td align="left">3</td><td align="left">DataLoadB</td><td align="left">Copy matrix B from L1 Buffer to L0B Buffer</td><td align="left">Nz -> Zn (LoadData2DParams)</td><td align="left">Nz -> Zn (LoadData2DParamsV2)</td></tr>
<tr><td align="left">4</td><td align="left">Compute</td><td align="left">Execute matrix multiplication Mmad computation</td><td align="left">N/A</td><td align="left">N/A</td></tr>
<tr><td align="left">5</td><td align="left">CopyOut</td><td align="left">Copy computation results from L0C Buffer to GM</td><td align="left">Nz -> ND</td><td align="left">Nz -> ND</td></tr>
</table>

## Build and Run

Execute the following steps in the root directory of this sample to build and run the sample.

- Configure environment variables

  Please select the corresponding command to configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.

  - Default path, root user installed CANN package

    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, non-root user installed CANN package

    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, installed CANN package

    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Sample execution

  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Default NPU mode
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output results are correct
  ```

  When using CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, you need to clean the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation option description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2/A3 series, dav-3510 corresponds to Ascend 950PR/950DT |

- Execution result

  The following execution result indicates successful precision comparison:

  ```bash
  test pass!
  ```