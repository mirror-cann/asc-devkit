# CAmodel Simulation Example Based on MatmulLeakyRelu

## Overview

This example uses MatmulLeakyRelu fused computation as the carrier to demonstrate the build, run, result verification, and performance data collection workflow of Ascend C programs in CAmodel simulation mode. Users can obtain simulation performance data through `msopprof simulator` to analyze performance bottlenecks and improve performance analysis efficiency.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── 08_simulator
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write helper functions
│   ├── README.md               // Example description
│   ├── scripts                 // Input data generation and result verification scripts
│   └── simulator.asc           // Ascend C example implementation and host-side invocation example
```

## Example Description

- Example features

  This example implements fused matrix multiplication and LeakyRelu activation function computation through the Matmul API. The computation formula is:

  ```text
  C = A * B
  C = C > 0 ? C : C * 0.001
  ```

  Where `LeakyRelu` means: when `C >= 0`, output `C`; when `C < 0`, output `C * 0.001`.

  In the example, `M = 512`, `K = 128`, `N = 128`. `scripts/gen_data.py` generates input data and golden data. After execution, the result is written to `output/output.bin`, and `scripts/verify_result.py` performs result verification.

- Example specifications

  <table border="2">
  <caption>Table 1: MatmulLeakyRelu Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">MatmulLeakyRelu</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">Example Input</td><td align="center">A</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Example Input</td><td align="center">B</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Example Output</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_leakyrelu_custom</td></tr>
  </table>

## Build and Run

Run the following steps in the root directory of this example to build and run the program.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  > To use the msOpProf tool, install CANN commercial/community edition. For details, refer to [msOpProf Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j
  python3 ../scripts/gen_data.py
  msopprof simulator --soc-version=Ascend910B1 ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  Select the corresponding `CMAKE_ASC_ARCHITECTURES` parameter based on the NPU hardware architecture being tested.

  | Option | Description |
  |--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | Set to `sim` to enable NPU simulation mode |
  | `CMAKE_ASC_ARCHITECTURES` | Specify the NPU architecture version. `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

  When accuracy comparison succeeds, the output is as follows:

  ```bash
  test pass!
  ```

## Simulation Tuning

Based on `./demo`, use `msopprof simulator` for simulation performance analysis to generate visualized instruction pipeline diagrams and other information. The command is:

```bash
msopprof simulator --soc-version=<soc_version> ./demo
```

> Obtain the AI processor model `<soc_version>` as follows:
>
>- Atlas A2 Training Series Products/Atlas A2 Inference Series Products
>   - For these product models: Run the `npu-smi info` command on the server with the Ascend AI processor installed to query and obtain the **Name** information. The actual configuration value is AscendName. For example, if the **Name** value is xxxyy, the actual configuration value is Ascendxxxyy.
>

> - Ascend 950PR/Ascend 950DT
> - Atlas A3 Training Series Products/Atlas A3 Inference Series Products
>   - For these product models, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server with the Ascend AI processor installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234.
>
> Where `id` is the device ID, which is the NPU ID obtained through the `npu-smi info -l` command; `chip_id` is the chip ID, which is the Chip ID obtained through the `npu-smi info -m` command.

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the current directory. The output structure is as follows:

```
OPPROF_{timestamp}_XXX/
├── dump                    // Raw performance data, no user attention needed
└── simulator
    ├── core*.veccore*/     // Simulation instruction pipeline diagram files for each vector core
    ├── trace.json          // Simulation pipeline diagram and hotspot function visualization file
    └── visualize_data.bin  // MindStudio Insight visualization file
```

After execution, view the instruction pipeline diagram through the following methods:

- **MindStudio Insight**: Open `visualize_data.bin` or `trace.json` for visual presentation.

  For more details, refer to the msOpProf tool usage: [MindStudio Tool Tuning (msOpProf) Quick Start](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md).
