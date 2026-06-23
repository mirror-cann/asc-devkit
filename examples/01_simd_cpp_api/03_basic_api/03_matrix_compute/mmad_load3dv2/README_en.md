# LoadData3DV2 Data Transfer Example in Matrix Multiplication

## Overview

This example introduces the usage scenarios and methods of the LoadData3DV2 instruction in matrix multiplication. LoadData3DV2 can transfer 2D A and B matrices from L1 to L0A/L0B, where A and B represent the left and right input matrices of matrix multiplication respectively.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mmad_load3dv2
│   ├── scripts
│   │   ├── gen_data.py                      // Script for generating input data and ground truth data
│   │   └── verify_result.py                 // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                       // Build project file
│   ├── data_utils.h                         // Data read/write functions
│   ├── load3d.py                            // LoadData3D helper script
│   ├── mmad_load3dv2.asc                    // Ascend C operator implementation & invocation example
│   └── README.md                            // Example documentation
```
## Operator Description

The LoadData3DV2 instruction (hereafter referred to as load3dv2) has transpose capabilities and supported data types that depend on the storage location of the destination address:

1. When the destination address is on L0A, supported data types are: uint8_t/int8_t/half/bfloat16_t/uint32_t/int32_t/float/int4b_t;

```cpp
When the destination address is on L0B, supported data types are: half/bfloat16_t/uint32_t/int32_t/float.
```
2. When the destination address is on L0A, enTranspose can determine whether to enable the transpose function;
    When the destination address is on L0B, the transpose function is enabled by default (even when enTranspose=false, the transpose function is still enabled).

Since this example does not currently support int4b_t input data type, it demonstrates the following five load3dv2 usage scenarios in matrix multiplication:

### Load3Dv2 Interface Scenario Table

| scenarioNum | Input Data Type | A Matrix Transpose | B Matrix Transpose |
| --- | --- | --- | --- |
| 1 | half | No transpose | No transpose |
| 2 | half | Transpose | No transpose |
| 3 | float | No transpose | No transpose |
| 4 | float | Transpose | No transpose |
| 5 | int8_t | No transpose | Transpose |

Note: When the input data type is B8 and the destination address is on L0B, the load3dv2 instruction is not supported. Therefore, when scenarioNum=5, SplitB calls the load2d instruction.

  In this example, the A matrix in scenarioNum=3 and 4 is consistent with scenarioNum=12 and 13 in the [load_data_l12l0](../load_data_l12l0/README.md) example respectively, and the B matrix is consistent with scenarioNum=13 in that example. Therefore, the specific parameter configuration and diagrams of the load3dv2 instruction can be found in the "6. L1 to L0 (LoadData)" section of that example's readme.

  Since the input data type differences have minimal impact on load3dv2 instruction parameter configuration, the remaining scenarios in this example can reference scenarioNum=3 and 4.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=4 M=30 K=40 N=70
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM -DM_SIZE=$M -DK_SIZE=$K -DN_SIZE=$N;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM -m=$M -k=$K -n=$N   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture |
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4` (default), `5` | Scenario number |
  | `M_SIZE` | `30` (default) | Number of elements on M axis |
  | `K_SIZE` | `40` (default) | Number of elements on K axis |
  | `N_SIZE` | `70` (default) | Number of elements on N axis |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
