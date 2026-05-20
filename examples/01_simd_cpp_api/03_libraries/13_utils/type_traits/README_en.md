# type_traits Type Traits Direct Call Sample
## Overview
This sample implements type_traits type traits use cases using the Ascend C programming language. Type traits interfaces include is_void, remove_const, add_const, and so on. It uses the <<<>>> kernel call operator to complete the basic flow of running and verifying operator kernel functions on the NPU side, providing corresponding end-to-end implementations.

## Supported Products
- Ascend 950PR/Ascend 950DT
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure
```
├── type_traits
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read and write functions
│   ├── type_traits.asc         // Ascend C operator implementation and call sample
│   └── README.md               // Sample introduction
```

## Function Description
- type_traits type traits interfaces include is_void, is_integral, is_floating_point, is_array, is_pointer, is_reference, is_const, remove_const, remove_volatile, remove_cv, remove_reference, remove_pointer, add_const, add_volatile, add_cv, add_pointer, add_lvalue_reference, and add_rvalue_reference. These interfaces can be used for type checking, addition, removal, and conversion during program compilation.

- Operator Implementation:
  - Kernel Implementation
    Ascend C calls each interface and prints the interface return value and ascendc_assert judgment.

  - Call Implementation
    Use the kernel call operator <<<>>> to call the kernel function.

## Build and Run

Execute the following steps in the root directory of this sample to build and run the operator.

- Configure Environment Variables
  Please select the corresponding command to configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  - Default path, root user installed CANN software package
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, non-root user installed CANN software package
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, installed CANN software package
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Run the Example

  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # Default npu mode
  ./demo                        # Execute the compiled executable program to run the sample
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