# Add Sample

## Overview

This sample demonstrates the basic usage of Ascend C vector addition.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── add
│   ├── CMakeLists.txt      // Compilation project file
│   ├── add.asc             // Ascend C sample implementation & invocation sample
│   └── README.md           // Sample documentation
```

## Sample Description

- Sample Function:
  The Add sample implements the function of adding two data items and returning the addition result. The corresponding mathematical expression is:
  ```
  z = x + y
  ```

- Sample Specifications:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Sample Implementation:
  - Kernel Implementation
    This sample uses 8 cores to complete the calculation, with each core processing 2048 elements. The calculation offset is:
    ```
    block_idx * blockLength
    ```

    The implementation process of the Add sample is divided into 3 steps:

    **Step 1: Move Data to UB (Unified Buffer)**

    Move inputs x and y from GM (Global Memory) to xLocal and yLocal on UB (Unified Buffer).

    **Step 2: Execute Vector Addition**

    Execute addition operation on xLocal and yLocal, and store the calculation result in zLocal on UB (Unified Buffer).

    **Step 3: Move Result to GM (Global Memory)**

    Move output data from zLocal to output z on GM (Global Memory).

- Invocation Implementation
  Use the kernel invocation operator <<<>>> to call the kernel function.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and execute the sample.

- Configure Environment Variables
  Configure environment variables according to the [installation method](../../../../../docs/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When the installation directory is not specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Compile the project, default npu mode
  ./demo                           # Execute the sample
  ```

  When using CPU Debug or NPU Simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU Debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU Simulation mode
  ```

  > **Attention:** You need to clear the cmake cache before switching compilation modes. Execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Results
  The execution result is as follows, indicating that the accuracy comparison succeeds.
  ```bash
  test pass!
  ```