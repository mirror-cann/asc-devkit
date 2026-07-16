# asc_copy_gm2l1 Example

## Overview

This example shows how to use the Ascend C API to move input data from GM (Global Memory) to the L1 Buffer. It supports NZ, ND, and DN input formats and vector-quantization parameter movement. After the input is moved, the example performs L1-to-L0 movement, matrix multiplication, and L0C-to-GM result writeback.

This example supports Ascend 950PR/Ascend 950DT (`dav-3510`) only. It supports NPU execution and NPU simulation; CPU debug mode is not provided.

## Supported Products and CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── asc_copy_gm2l1
│   ├── figures                        // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                // Generates input and golden data
│   │   └── verify_result.py           // Verifies that output matches the golden data
│   ├── data_utils.h                   // Data input/output helpers
│   ├── CMakeLists.txt                 // CMake build file
│   ├── asc_copy_gm2l1.asc             // C API implementation and launcher
│   ├── README.md                       // Chinese example documentation
│   └── README_en.md                    // English example documentation
```

## Detailed Scenario Description

Use the `SCENARIO_NUM` build parameter to select an input scenario. The meaning of each value is shown below. All scenarios use the same matrix multiplication shape: [M, K, N] = [128, 128, 256]. The kernel is named `asc_copy_gm2l1`.

<table>
<caption style="font-weight: normal;">
         <span style="font-weight: bold; font-size: 1.2em;">📌 Table 1: SCENARIO_NUM values</span></caption>
<tr><td rowspan="1" align="center">SCENARIO_NUM</td><td align="center">Input format</td><td align="center">Input type</td><td align="center">Output type</td><td align="center">Vector quantization enabled</td></tr>
<tr><td align="center">1</td><td align="center">NZ</td><td align="center">half</td><td align="center">float</td><td align="center">No</td></tr>
<tr><td align="center">2</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">No</td></tr>
<tr><td align="center">3</td><td align="center">DN</td><td align="center">half</td><td align="center">float</td><td align="center">No</td></tr>
<tr><td align="center">4</td><td align="center">ND</td><td align="center">half</td><td align="center">int8_t</td><td align="center">Yes</td></tr>
</table>

`SCENARIO_NUM` is passed by CMake as a compile-time macro. The kernel selects the corresponding scenario with `if constexpr`; rerun CMake and rebuild after changing the scenario.

All device-side movement and compute use asynchronous C API calls. `asc_sync_notify` and `asc_sync_wait` establish only the required `MTE2→MTE1→M→FIX` dependencies; scenario 4 also uses `MTE2→FIX` to make the quantization parameters ready, then uses `asc_sync_pipe(PIPE_FIX)` after the L1-to-Fixpipe Buffer transfer before the final Fixpipe writeback. No `PIPE_ALL` synchronization is used.

**Scenario 1: NZ input, half input type**

- Input: A [128, 128] in `half` NZ format; B [128, 256] in `half` NZ format
- Output: C [128, 256] in `float` ND format
- Implementation: Use `asc_copy_gm2l1` to move the NZ input data from GM to L1
- Description: The inputs are already in NZ format, so no format conversion is required
<p align="center">
  <img src="figures/asc_copy_gm2l1_nz2nz.png" width="800">
</p>

**Scenario 2: ND input, half input type**

- Input: A [128, 128] in `half` ND format; B [128, 256] in `half` ND format
- Output: C [128, 256] in `float` ND format
- Implementation: Configure ND2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_nd2nz` to move ND input data from GM to L1 and convert it to NZ
- Description: The ND inputs are converted to NZ during the movement
<p align="center">
  <img src="figures/asc_copy_gm2l1_nd2nz.png" width="800">
</p>

**Scenario 3: DN input, half input type (Ascend 950PR/Ascend 950DT only)**

- Input: A [128, 128] in `half` DN format; B [128, 256] in `half` DN format
- Output: C [128, 256] in `float` ND format
- Implementation: Configure DN2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_dn2nz` to move DN input data from GM to L1 and convert it to NZ
- Description: The DN inputs are converted to NZ during the movement
<p align="center">
  <img src="figures/asc_copy_gm2l1_dn2nz.png" width="800">
</p>

**Scenario 4: ND input, half input type, vector quantization enabled**

- Input: A [128, 128] in `half` ND format; B [128, 256] in `half` ND format; quantization parameters [256] in `uint64_t`
- Output: C [128, 256] in `int8_t` ND format
- Implementation: Configure ND2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_nd2nz` to move input data from GM to L1 and perform matrix multiplications; use `asc_copy_gm2l1` and `asc_copy_l12fb` to move quantization parameters from GM through L1 to Fixpipe Buffer; configure their address with `asc_set_l0c2gm_config`, then use `asc_copy_l0c2gm` with `VQF322B8_PRE` for vector-quantized output
- Description: The `float` matrix multiplication result is vector-quantized to `int8_t`; each C matrix column uses one quantization parameter. `asc_set_l0c2gm_config` configures the Fixpipe vector-quantization parameter address through `quant_pre`, which uses 128B address units. The parameter file must therefore be rounded up to 128B, with any padding zero-filled. In this example, the `[256] uint64_t` parameters occupy 2048B and already meet this requirement. When the parameters are moved from L1 to Fixpipe Buffer, `asc_copy_l12fb` specifies `len_burst` in 64B units; this buffer therefore uses 32 bursts.

## Build and Run

Run the following steps in the example root directory to build and execute the example.

- Configure environment variables

  Configure environment variables according to the CANN development kit [installation instructions](../../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If it is not specified, the default is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the example executable
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify the result
  ```

  Set both `SCENARIO_NUM` and `-scenarioNum` to a value from 1 through 4 to run the corresponding scenario.

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the CMake cache before switching run modes. Run `rm CMakeCache.txt` in the build directory and re-run CMake.

- Build options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | Scenario number |

- Expected result

  For scenarios 1-3, the floating-point result must satisfy the accepted tolerance. For scenario 4, the int8 result must exactly match the golden data. In either case, successful verification prints:

  ```bash
  test pass!
  ```
