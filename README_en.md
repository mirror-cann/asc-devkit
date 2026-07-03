<div align="center">

# Ascend C

<h4>Build multi-level APIs based on C/C++, provide complete chip programming capabilities, and support achieving ultimate performance</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](https://gitcode.com/cann/asc-devkit/blob/master/LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](https://gitcode.com/cann/asc-devkit/blob/master/CONTRIBUTING_en.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)
[![VS Marketplace](https://vsmarketplacebadges.dev/version/HICANN.ascend-c-toolkit.svg)](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit)
[![Installs](https://vsmarketplacebadges.dev/installs-short/HICANN.ascend-c-toolkit.svg)](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit)

</div>

## 🔥Latest News
[2026/03] Key features of v9.0.0-beta.2
### 🚀 Key Features
- Ascend 950PR supports SIMD programming mode and provides over 200 [API interfaces](./impl/basic_api/dav_3510) with cross-generation compatibility capabilities, enabling smooth operator migration between Atlas A2 series products and Atlas A3 series products.
- Ascend 950PR adds Reg-based programming and provides over 90 [Reg programming interfaces](./impl/basic_api/reg_compute/dav_3510), including Reg datamove, basic arithmetic, reduction calculation, and synchronization control.
- Atlas A2 series products, Atlas A3 series products, and Ascend 950PR support [language extension layer pure C interfaces](./include/c_api), supporting array-style memory allocation and pointer-based calculation interfaces, providing a native pure C programming experience.
- Ascend 950PR supports SIMD and SIMT hybrid programming and provides approximately 700 [SIMT API interfaces](./include/simt_api), including warp, atomic, basic mathematical calculation, type conversion, and other fundamental interfaces.
- Ascend 950PR supports CCU communication interfaces for communication high-level APIs and provides mainstream communication primitives based on CCU, including [Allreduce, Allgather, Reducescatter, AlltoAll, and others](./impl/adv_api/detail/hccl/impl/platform_v310). Matmul high-level API adds support for [MXFP4/8 low-bit data type matrix operations](./impl/adv_api/detail/matmul/mx_matmul_impl.h), achieving half memory usage and doubled computing throughput.
- Ascend 950PR adds and supports approximately 260 sample cases in total, including [SIMT samples](./examples/03_simt_api), [SIMD samples](./examples/01_simd_cpp_api) (framework, basic API, high-level API, best practices, and others), and adjusts the [sample directory structure](https://gitcode.com/cann/asc-devkit/pull/1223) according to programming model and sample category to improve readability.
- Fusion compilation and <<<>>> invocation support [CPU mode](https://gitcode.com/cann/asc-tools/pull/138) and [SIM simulation mode](https://gitcode.com/cann/asc-devkit/blob/master/cmake/asc/asc_modules/CMakeASCInformation.cmake).
### 📖 Documentation
- Adds over 90 [Reg programming interface API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMD-API/基础API/Reg矢量计算/Reg矢量计算.md) documentation. Reg vector calculation API is developed for the RegBase architecture. Users can directly operate registers involved in Vector calculation on the chip through this API to achieve greater flexibility and better performance.
- Adds SIMT [quick start](docs/guide/入门教程/快速入门/基于SIMT编程/Gather算子快速入门.md), [programming model](docs/guide/编程指南/编程模型/AI-Core-SIMT编程/抽象硬件架构.md), and [operator implementation](docs/guide/算子实践参考/SIMT算子实现/基础知识.md) introduction.
- Adds SIMD and SIMT [hybrid programming model](docs/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程/概述.md), [operator implementation](docs/guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识.md), and [performance optimization](docs/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率.md) introduction.
- Adds [SIMT API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/SIMT-API.md) documentation chapter.
- Adds [compatibility migration guide](docs/guide/跨代迁移兼容性指南/Ascend-C-API兼容策略.md) (migration from 220x architecture version to 351x architecture version).
- In the Ascend community, Ascend C operator development adds a [visualization section](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html), presenting the execution process of Cube and Vector operators through videos.

For detailed information about all historical versions and updates, see [CHANGELOG.md](./CHANGELOG_en.md)

## 🚀 Overview

[Ascend C](https://www.hiascend.com/cann/ascend-c) is a specialized operator programming language for Ascend AI processors launched by CANN (Compute Architecture for Neural Networks), providing native support for C and C++ standard specifications. As a programming language designed for multiple scenarios, Ascend C not only focuses on **opening complete chip programming capabilities to support achieving ultimate performance**, but also enables flexible selection of the most appropriate API through multi-level programming API design based on project requirements, team skills, and performance goals, achieving the best balance between development efficiency and runtime performance.

### Design Goals

The design goals of Ascend C can be summarized as **"high performance, completeness, ease of programming, debuggability, and compatibility"**. Through minimal extension to C/C++ language standards, it supports both pointer-based C language development habits and Tensor-based C++ programming paradigms, achieving seamless integration with existing ecosystems while supporting efficient Ascend operator development and ensuring consistency in development experience.

We uphold the following core philosophies:
- **No silver bullet**: Different scenarios have different requirements for performance and development efficiency. A single interface cannot optimally adapt to all scenarios.
- **Progressive learning**: Beginners can start with ease-of-use interfaces to quickly verify algorithms, while experts can drill down and fine-tune, leveraging complex interface features to fully exploit hardware potential.

### API Levels
Ascend C provides three types of interfaces, all capable of achieving complete low-level programming capabilities:

| API Level | Language | Features | Target Users | Primary Use |
|----------|----------|----------|----------|----------|
| **Tpipe/Tque Framework Programming API** |  **C++** |Based on **Tensor** programming<br>Unified management of memory and synchronization through Tpipe/Tque framework| Operator library developers| Based on framework automatic synchronization and memory management<br>Improve programming ease of use|
| **Basic API** | **C++** |Based on **Tensor** programming, providing **C++ basic complete programming capabilities**<br>Allocate Tensor through MakeTensor/LocalMemoryAllocator, manage synchronization independently| Operator library developers|Manage synchronization and memory independently<br>Match C++ Tensor development habits, support achieving ultimate performance|
| **Language Extension Layer<br>SIMD&SIMT API** |**C**|Based on **pointer** programming, providing **C basic complete programming capabilities**<br>Allocate memory through array[], manage synchronization independently|Operator library developers |Manage synchronization and memory independently<br>Match C language development habits, support achieving ultimate performance|


Additionally, Ascend C provides high-level APIs and operator template libraries to improve operator development efficiency.

| API Level |  Target Users | Primary Use |
|----------|----------|----------|
| **Operator Template Library (CATLASS/ATVOSS and others)** |  Algorithm developers | Perform custom extensions based on typical operator implementations to meet high-performance requirements in specific scenarios | 
| **High-level API** |Algorithm developers | Reuse general single-core algorithms to quickly complete algorithm verification |


The overall logical architecture diagram is shown below:

<img src="docs/guide/figures/architecture_ascendc.png" alt="Architecture Diagram"  width="1000px">

- **Language Extension Layer C API**: Pure C interfaces, supporting array memory allocation and pointer-based calculation interfaces, providing a C programming experience consistent with industry standards and opening complete chip programming capabilities. Atlas A2/A3 supports SIMD pure C interfaces; Ascend 950PR/Ascend 950DT will support SIMT programming capabilities similar to industry standards and SIMD/SIMT hybrid programming capabilities.
- **Basic API**: Single-instruction abstracted C++ class library API, generally based on Tensor programming; gradually improving Tensor programming capabilities based on Layout.
- **High-level API**: Abstraction and encapsulation of common algorithms based on single cores, providing implementations of public algorithms.
- **Operator Template Library**: Complete implementation references for operators based on templates, simplifying Tiling development and supporting user custom extensions.
- **Python Frontend PyAsc**: PyAsc provides complete low-level chip programming capabilities based on Python frontend, and will gradually improve Tensor programming capabilities based on Layout, adding SIMT programming and other capabilities to achieve high-performance operator development based on Python interfaces.

### How to Choose Multi-level APIs for Operator Development
- **Based on C/C++ language development**: For details, see [Ascend C Multi-level API Selection Guide](./docs/en/asc_how_to_choose_api.md)
- **Based on Python language development, supporting complete programming capabilities to achieve ultimate performance**: Recommend Ascend C Python frontend [PyAsc](https://gitcode.com/cann/pyasc)
- **Based on Python language development, quick development verification, ease of use priority**: Recommend [PyPTO](https://gitcode.com/cann/pypto)


## 🔍 Directory Structure
This repository mainly contains Ascend C programming APIs and necessary cmake build scripts, which are the core modules required for operator development. The directory structure is as follows:

```
├── cmake                               # Ascend C build source code
├── docs                                # Project documentation
├── examples                            # Ascend C API sample projects
├── impl                                # Ascend C API implementation source code
│   ├── adv_api                         # Ascend C high-level API implementation source code
│   ├── aicpu_api                       # Ascend C AI CPU API implementation source code
│   ├── basic_api                       # Ascend C basic API implementation source code
│   ├── c_api                           # Ascend C language extension layer C API implementation source code
│   ├── simt_api                        # Ascend C SIMT API implementation source code
│   ├── tensor_api                      # Ascend C TENSOR API implementation source code
│   └── utils                           # Ascend C utility implementation source code
├── include                             # Ascend C API declaration source code
│   ├── adv_api                         # Ascend C high-level API declaration source code
│   ├── aicpu_api                       # Ascend C AI CPU API declaration source code
│   ├── basic_api                       # Ascend C basic API declaration source code
│   ├── c_api                           # Ascend C language extension layer C API declaration source code
│   ├── simt_api                        # Ascend C SIMT API declaration source code
│   ├── tensor_api                      # Ascend C TENSOR API declaration source code
│   └── utils                           # Ascend C utility declaration source code
├── scripts                             # Packaging-related scripts
├── tests                               # Ascend C API UT test cases
└── tools                               # Ascend C tool source code
```

## ⚡️ Quick Start

If you want to quickly experience project build and operator sample execution, access the following documentation for simple tutorials.

- [Build and Compile](docs/en/quick_start.md): Introduces environment setup, compilation execution, local verification, and other operations.
- [Sample Execution](examples/README_en.md): Provides operator development samples and introduces end-to-end sample execution methods.

## 🧰 clangd/IDE Support

- Install clangd (recommend 15+, using Ubuntu operating system as an example) and VSCode plugin clangd

  ```bash
  sudo apt install -y clangd-15
  sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-15 100
  ```

- Configure local VSCode `settings.json` (example)

  ```json
  {
    "clangd.path": "/usr/bin/clangd",
    "clangd.arguments": [
        "--background-index=0",
        "--clang-tidy=0"
    ],
    "C_Cpp.intelliSenseEngine": "disabled"
  }
  ```

- Configure `.clangd` in the project root directory (example). The complete `.clangd` file is provided in this directory. CANN header file directories need to match actual installation locations. The default in `.clangd` is `/usr/local/Ascend`.

  If CANN is installed in a non-default path, or if you need to switch the NPU architecture macro, source CANN `set_env.sh` first, then generate a local config from `.clangd.in`:

  ```bash
  source /path/to/cann/set_env.sh
  python3 scripts/setup_clangd.py --npu-arch 2201 --output .clangd.local
  ```

  `ASCEND_HOME_PATH` is set by `set_env.sh`, and the script uses this environment variable to generate the real CANN path. If it is not set, the script prompts you to source `set_env.sh` and retry. The generated file defaults to `.clangd.local` and does not overwrite the built-in `.clangd`. To make project-level clangd config take effect, copy it to `.clangd` as needed and restart clangd.

  ```yaml
  CompileFlags:
    Add:
      - "-std=c++17"
      - "-stdlib=libstdc++"
      - "-D__NPU_ARCH__=2201"
      - "-DASCENDC_CPU_DEBUG=1"
      ...

  ---
  If:
    PathMatch: ".*\\.(asc|aicpu)$"
  CompileFlags:
    CompilationDatabase: None
    Add:
      - "-x"
      - "c++"
  Diagnostics:
    Suppress:
      - "attributes_not_allowed"
      - "decomp_decl_template"
      - "ignored-attributes"
      - "unknown_typename"
      - "undeclared_var_use"
      - "invalid_token_after_toplevel_declarant"
      - "missing_type_specifier"
      - "typename_nested_not_found"
      - "redefinition"
  ```

- Restart clangd (VS Code: Command Palette -> "Clangd: Restart language server")

- We recommend installing the VS Code extension [Ascend C Toolkit](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit), which supports editing, building, running, debugging, exception detection, and performance tuning for Ascend C operator projects in VS Code. When this workspace is opened, VS Code prompts users to install the recommended extensions from `.vscode/extensions.json`.

- Recent Ascend C Toolkit updates focus on environment parsing for Atlas A2 and Atlas A3 series products, standard custom operator creation, SoC settings, simulation analysis, NPU debugging, settings migration, and stability. For full usage instructions and feedback channels, see the [CANN discussion guide](https://gitcode.com/org/cann/discussions/54).

- 💡 For any suggestions or improvements regarding ASC language syntax highlighting and code navigation support, community developers are welcome to provide feedback!

## 📖 Related Resources

- **Programming Guide**
  | Document  |  Description   |
  |---------|--------|
  |[Ascend C Programming Guide](https://hiascend.com/document/redirect/CannCommunityOpdevAscendC)|Write operator programs using Ascend C based on Ascend AI hardware and develop custom operators.|
  |[Ascend C Best Practice](https://hiascend.com/document/redirect/CannCommunityAscendCBestPractice) | Based on completed Ascend C operators, introduces how to further optimize operator performance. |
  |[Ascend C API List](https://hiascend.com/document/redirect/CannCommunityAscendCApi)| Ascend C SIMD&SIMT API, including language extension layer C API, C++ class library basic API, and high-level API|
  |[Ascend C Samples](./examples)| Ascend C API key feature introduction samples, including AICore SIMD&SIMT, AICPU, and others|
  |[Ascend C Programming Guide (Harmony)](https://gitcode.com/cann/cann-recipes-harmony-infer/blob/master/docs/ascendc_develop_guide.md)|Write operator programs using Ascend C based on Kylin AI hardware and develop custom operators.|

- **Contribution Guide**
  | Document  |  Description   |
  |---------|--------|
  |[CANN Community Contribution Guide](https://gitcode.com/cann/community)| CANN community Issue, PR, and other general processing procedures|
  |[ASC-DevKit Contribution Guide](./CONTRIBUTING_en.md) | Ascend C API, documentation, and sample contribution guide|

- **Others**
  | Document  |  Description   |
  |---------|--------|
  |[Ascend C Meetup Materials](https://gitcode.com/cann/community/tree/master/events/meetup/slides/sig-ascendc)|Ascend C external presentation PPT materials, including 950 new features and others|
  |[Ascend C Wiki](https://gitcode.com/cann/asc-devkit/wiki)|Ascend C technical promotion articles and others|
  |[CANN-Learning-Hub](https://gitcode.com/cann/cann-learning-hub/tree/master/tutorials/ascendc_operator_development)|Ascend C operator development online full-process tutorial|
  |[Ascend C Ops Samples](https://gitcode.com/cann/cann-samples)|Ascend C operator how to gradually achieve high-performance sample repository|

## 📌 Related Plans

- [Ascend C Development Roadmap (2026 Q2)](https://gitcode.com/cann/asc-devkit/issues/316);

## 📝 Related Information

- [Contribution Guide](CONTRIBUTING_en.md)
- [Security Statement](SECURITY_en.md)
- [License](LICENSE)
