# AddBroadCast Operator Direct Call Example

## Overview

This example introduces the kernel function direct call method for the Add operator, with input Broadcast added for multi-core and tiling scenarios.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── add_broadcast
│   ├── scripts
│   │   ├── gen_data.py          // Input data and ground truth data generation script
│   │   └── verify_result.py     // Verification script for checking output data against ground truth data
│   ├── CMakeLists.txt           // Build project file
│   ├── data_utils.h             // Data read and write functions
│   └── add_broadcast.asc        // Ascend C operator implementation & invocation example
```

## Operator Description

- Operator Function:
  The Add operator implements the addition of two data values and returns the addition result. The corresponding mathematical expression is:
  ```
  z = x + y
  ```
- Operator Specifications:
  <table>
  <tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 1) / (16, 1) / (20, 1) / (20, 1)
  </td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">axis = 0: (1, 1024) / (1, 1022) / (1, 1024) / (1, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">z</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_broadcast_custom</td></tr>
  </table>

- Operator Implementation:

  The computation logic is: The vector computation interface provided by Ascend C operates on elements in LocalTensor, so input data must first be moved to on-chip memory, then the computation interface is used to complete the addition of two input parameters to obtain the final result, which is then moved to external memory.

    The Add operator implementation process consists of 3 basic tasks: CopyIn, Compute, and CopyOut.
    The CopyIn task moves the input Tensors xGm and yGm from Global Memory to Local Memory, storing them in xLocal and yLocal respectively;
    The Compute task performs Broadcast on xLocal and yLocal and stores them in temporary memory, then executes the addition operation, storing the computation result in zLocal;
    The CopyOut task moves the output data from zLocal to the output Tensor zGm in Global Memory.

  - Tiling Implementation
    TilingData parameter design: TilingData parameters are essentially parameters related to parallel data partitioning.

    This example attempts to distribute input data evenly across a given number of cores for computation. If even distribution is not possible, the number of full cores and tail cores is calculated, with full cores and tail cores processing different amounts of data. Define UB_BLOCK_NUM as the maximum available UB data block, corresponding to a UB size of UB_BLOCK_NUM * 32 Bytes. Tiling partitioning is performed with UB_BLOCK_NUM as the granularity.

    This example operator uses 20 tiling parameters: xLen, yLen, and dataType are the total length and data type of x and y input data; coef is the dimension expansion factor for the input that needs Broadcast; axis is the axis for input data Broadcast; blockLength, tileNum, tileLength, and lastTileLength are the computation amount per core, number of tiling partitions, and full block and tail block lengths in the core even distribution scenario; former(tail)Num, former(tail)Length, former(tail)TileNum, former(tail)TileLength, and former(tail)LastTileLength are the partitioning data related to full cores and tail cores in the uneven core distribution scenario, corresponding to those in the even core distribution scenario; isEvenCore is used to determine whether the current scenario is an even core distribution scenario.

  - Invocation Implementation
    Use the kernel call operator <<<>>> to invoke the kernel function.

## Build and Run

Execute the following steps in the root directory of this example to build and run the operator.

- Configure Environment Variables
  Select the appropriate environment variable configuration command based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on your current environment.
  - Default path, CANN software package installed by root user
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, CANN software package installed by non-root user
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, CANN software package installed
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Run the Example

  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # Default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Execute the compiled executable program to run the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output result correctness, confirm algorithm logic is correct
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