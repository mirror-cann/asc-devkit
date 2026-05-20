# Reduce Example

## Overview

This example introduces how to call the WholeReduceSum and BlockReduceSum high-level APIs to implement the reduce operator, which accumulates data elements in contiguous memory and returns the accumulation result.

## Supported Products

- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── reduce
│   ├── scripts
│   │   ├── gen_data.py         // Script to generate input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read and write functions
│   └── reduce.asc              // Ascend C operator implementation & invocation example
```

## Operator Description

- Operator Function:
  The ReduceCustom operator accumulates data elements in contiguous memory and returns the accumulation result. The corresponding mathematical expression is:

  ```
  z = sum(x)
  ```

- Operator Specifications:
  <table>
  <tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center"> reduce </td></tr>

  <tr><td rowspan="3" align="center">Operator Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">4096</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Operator Output</td></tr>
  <tr><td align="center">y</td><td align="center">32</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_custom</td></tr>
  </table>

- Operator Implementation:
  This example implements the ReduceCustom operator with a fixed shape of input x[4096] and output y[32]. The valid range for x length is (0,4096] for float and (0,16384] for half.

    The example accumulates a contiguous input segment to obtain the sum of elements in this contiguous buffer.

      - 1. When the length is less than 256B, ComputeKey = 1, and WholeReduceSum can obtain the result in one operation.

      - 2. When the length is in the range of (256B, 2KB] for float input or (256B, 4KB] for half input, ComputeKey = 2. Since BlockReduceSum executes faster than WholeReduceSum for the same input length, a combination of one BlockReduceSum and one WholeReduceSum is used to achieve higher computational efficiency.

      - 3. When the length is in the range of (2KB, 16KB] for float input or (4KB, 32KB] for half input, ComputeKey = 3. Since one WholeReduceSum has higher accumulation efficiency than using two BlockReduceSum operations, two WholeReduceSum operations are used (instead of two BlockReduceSum plus one WholeReduceSum) to obtain the accumulation sum of this buffer.

      - 4. When the float input length is 10000, ComputeKey = 4, corresponding to the processing method in WholeReduceSumImpl. In Counter mode, the WholeReduceSum instruction is used to process each row in the two-dimensional data in a loop to obtain the reduction result for each row.

      - 5. When the float input length is 20000, ComputeKey = 5, corresponding to the processing method in BinaryReduceSumImpl. In Counter mode, the operation data is first divided into two parts, the Add instruction is used to add the two parts together, and this process repeats. Finally, one WholeReduceSum instruction obtains the reduction result. This operation mode has better performance compared to the single WholeReduceSum instruction mode when the data volume is large and there are many loop iterations.
      Note that Counter mode is used in the code.

  - Kernel Implementation
    The computation logic is: the operation elements of the vector computation interface provided by Ascend C are all LocalTensor. Input data must first be moved to on-chip storage, then the Reduce high-level API interface is used to complete the reduce computation and obtain the final result, which is then moved to external storage.

    The implementation process of the ReduceCustom operator is divided into 3 basic tasks: CopyIn, Compute, and CopyOut. The CopyIn task moves the input Tensor xGm from Global Memory to xLocal. The Compute task performs the reduce computation on xLocal, and the computation method is determined by the ComputeKey parameter, which is determined by the input length. The computation result is stored in zLocal. The CopyOut task moves the output data from zLocal to the output Tensor zGm on Global Memory.

  - Invocation Implementation
    Use the kernel call operator <<<>>> to invoke the kernel function.

## Build and Run

Execute the following steps in the root directory of this example to build and run the operator.

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
  mkdir -p build && cd build;   # Create and enter build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # Default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Execute the compiled program to run the example
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