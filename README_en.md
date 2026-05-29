<div align="center">

# Ascend C

<h4>Multi-level API built on C/C++, providing complete chip programming capabilities for极致performance</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](https://gitcode.com/cann/asc-devkit/blob/master/LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](https://gitcode.com/cann/asc-devkit/blob/master/CONTRIBUTING.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)

</div>

## 🔥Latest News
[2026/03] v9.0.0-beta.2 Key Features
### 🚀 Key Features
- Ascend 950PR supports SIMD programming mode, providing 200+ [API interfaces](./impl/basic_api/dav_3510) with cross-generation compatibility, enabling smooth operator migration from Atlas A2 series and Atlas A3 series products.
- Ascend 950PR adds Reg-based programming, providing 90+ [Reg programming interfaces](./impl/basic_api/reg_compute/dav_3510) including Reg data movement, basic arithmetic, reduction computation, and synchronization control.
- Atlas A2 series, Atlas A3 series, and Ascend 950PR support [language extension layer pure C interfaces](./include/c_api), providing array-style memory allocation and pointer-based computation interfaces for native pure C programming experience.
- Ascend 950PR supports SIMD and SIMT hybrid programming, providing approximately 700 [SIMT API interfaces](./include/simt_api), including warp, atomic, basic mathematical computation, type conversion, and other fundamental interfaces.
- Ascend 950PR supports communication high-level API CCU communication interfaces, providing [Allreduce, Allgather, Reducescatter, AlltoAll and other mainstream communication primitives](./impl/adv_api/detail/hccl/impl/platform_v310) based on CCU; Matmul high-level API adds support for [MXFP4/8 low-bit data type matrix operations](./impl/adv_api/detail/matmul/mx_matmul_impl.h), achieving half memory usage and doubled computing throughput.
- Ascend 950PR adds and compatibly supports approximately 260 samples, including [SIMT samples](./examples/03_simt_api), [SIMD samples](./examples/01_simd_cpp_api) (framework, basic API, high-level API, best practices, and so on), and [sample directory structure adjustments](https://gitcode.com/cann/asc-devkit/pull/1223) to improve readability.
- Fusion compilation and <<<>>> invocation support [CPU mode](https://gitcode.com/cann/asc-tools/pull/138) and [SIM simulation mode](https://gitcode.com/cann/asc-devkit/blob/master/cmake/asc/asc_modules/CMakeASCInformation.cmake).
### 📖 Documentation
- Added 90+ [Reg programming interface API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMD-API/基础API/Reg矢量计算/Reg矢量计算.md) documentation. Reg vector computation API is developed for RegBase architecture, allowing users to directly operate registers involved in Vector computation on the chip for greater flexibility and better performance.
- Added SIMT [quick start](docs/guide/入门教程/快速入门/基于SIMT编程/Gather算子快速入门.md), [programming model](docs/guide/编程指南/编程模型/AI-Core-SIMT编程/抽象硬件架构.md), and [operator implementation](docs/guide/算子实践参考/SIMT算子实现/基础知识.md) introductions.
- Added SIMD and SIMT [hybrid programming model](docs/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程.md), [operator implementation](docs/guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识.md), and [performance optimization](docs/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率.md) introductions.
- Added [SIMT API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/SIMT-API.md) documentation section.
- Added [compatibility migration guide](docs/guide/跨代迁移兼容性指南/Ascend-C-API兼容策略.md) (220x architecture version migration to 351x architecture version).
- In Ascend community, Ascend C operator development added [visualization zone](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html), presenting Cube and Vector operator execution processes through videos.

For detailed information about all historical versions and updates, see [CHANGELOG.md](./CHANGELOG.md)

## 🚀 Overview

[Ascend C](https://www.hiascend.com/cann/ascend-c) is a dedicated operator program development language for Ascend AI processors launched by CANN (Compute Architecture for Neural Networks), with native support for C and C++ standard specifications. As a programming language designed for multiple scenarios, Ascend C not only aims to **provide complete chip programming capabilities for ultimate performance**, but also enables flexible selection of the most suitable API based on project requirements, team skills, and performance goals through multi-level programming API design, achieving the best balance between development efficiency and runtime performance.

### Design Goals

Ascend C's design goals can be summarized as **"high performance, completeness, ease of programming, debuggability, and compatibility"**. Through minimal extensions to C/C++ language standards, it supports both pointer-based C language development habits and Tensor-based C++ programming paradigm, seamlessly integrating with existing ecosystems while supporting efficient Ascend operator development and ensuring consistent development experience.

We adhere to the following core principles:
- **No silver bullet**: Different scenarios have varying requirements for performance and development efficiency; a single interface cannot optimally adapt to all scenarios;
- **Progressive learning**: Beginners can start with ease-of-use interfaces for quick algorithm verification; experts can drill down for fine-tuning, leveraging complex interface features to fully exploit hardware potential.

### API Levels
Ascend C provides three types of interfaces, all achieving complete low-level programming capabilities:

| API Level | Language | Features | Target Users | Main Uses |
|----------|----------|----------|----------|----------|
| **Tpipe/Tque Framework Programming API** | **C++** | Tensor-based programming<br>Unified memory and synchronization management through Tpipe/Tque framework | Operator library developers | Framework-based automatic synchronization and memory management for improved programming usability |
| **Basic API** | **C++** | Tensor-based programming, providing **C++ basic complete programming capabilities**<br>Tensor allocation through MakeTensor/LocalMemoryAllocator, self-managed synchronization | Operator library developers | Self-managed synchronization and memory<br>Matching C++ Tensor development habits for ultimate performance |
| **Language Extension Layer<br>SIMD&SIMT API** | **C** | Pointer-based programming, providing **C basic complete programming capabilities**<br>Memory allocation through array [], self-managed synchronization | Operator library developers | Self-managed synchronization and memory<br>Matching C language development habits for ultimate performance |


In addition, Ascend C provides high-level API and operator template library to improve operator development efficiency.

| API Level | Target Users | Main Uses |
|----------|----------|----------|
| **Operator Template Library (CATLASS/ATVOSS and so on)** | Algorithm developers | Custom extensions based on typical operator implementations for specific scenario high-performance requirements |
| **High-level API** | Algorithm developers | Reusing common single-core algorithms for quick algorithm verification |


The overall logical architecture diagram is shown below:

<img src="docs/figures/architecture.png" alt="Architecture Diagram" width="850px" height="580px">

- **Language Extension Layer C API**: Pure C interfaces, supporting array memory allocation and pointer-based computation interfaces, providing industry-consistent C language programming experience and complete chip programming capabilities. Atlas A2/A3 supports SIMD pure C interfaces; Ascend 950PR/Ascend 950DT will support industry-similar SIMT programming capabilities and SIMD/SIMT hybrid programming capabilities;
- **Basic API**: Single-instruction abstracted C++ class library API, generally Tensor-based programming; gradually improving Tensor programming capabilities based on Layout;
- **High-level API**: Abstraction and encapsulation based on single-core for common algorithms, providing implementations of public algorithms;
- **Operator Template Library**: Complete implementation references for operators based on templates, simplifying Tiling development, supporting user-defined extensions;
- **Python Frontend PyAsc**: PyAsc provides complete low-level chip programming capabilities based on Python frontend, and will gradually improve Tensor programming capabilities based on Layout, adding SIMT programming and other capabilities for developing high-performance operators based on Python interfaces;

### How to Choose Multi-level API for Operator Development
- **C/C++ language-based development**: See [Ascend C Multi-level API Selection Guide](./docs/en/asc_how_to_choose_api.md) for details
- **Python language-based development, supporting complete programming capabilities for ultimate performance**: Recommend Ascend C Python frontend [PyAsc](https://gitcode.com/cann/pyasc)
- **Python language-based development, quick development verification, ease of use priority**: Recommend [PyPTO](https://gitcode.com/cann/pypto)


## 🔍 Directory Structure
This repository mainly contains Ascend C programming API and necessary cmake build scripts, which are core modules required for operator development. The directory structure is as follows:

```
├── cmake                               # Ascend C build source code
├── docs                                # Project documentation introduction
├── examples                            # Ascend C API sample projects
├── impl                                # Ascend C API implementation source code
│   ├── adv_api                         # Ascend C high-level API implementation source code
│   ├── aicpu_api                       # Ascend C AI CPU API implementation source code
│   ├── basic_api                       # Ascend C basic API implementation source code
│   ├── c_api                           # Ascend C language extension layer C API implementation source code
│   ├── experimental                    # Ascend C TENSOR API implementation source code
│   ├── simt_api                        # Ascend C SIMT API implementation source code
│   └── utils                           # Ascend C utility class implementation source code
├── include                             # Ascend C API declaration source code
│   ├── adv_api                         # Ascend C high-level API declaration source code
│   ├── aicpu_api                       # Ascend C AI CPU API declaration source code
│   ├── basic_api                       # Ascend C basic API declaration source code
│   ├── c_api                           # Ascend C language extension layer C API declaration source code
│   ├── experimental                    # Ascend C TENSOR API declaration source code
│   ├── simt_api                        # Ascend C SIMT API declaration source code
│   └── utils                           # Ascend C utility class declaration source code
├── scripts                             # Packaging related scripts
├── tests                               # Ascend C API UT cases
└── tools                               # Ascend C tool source code
```

## ⚡️ Quick Start

To quickly experience project build and operator sample execution, access the following documentation for simple tutorials.

- [Build and Compilation](docs/en/quick_start.md): Introduces environment setup, build execution, local verification, and other operations.
- [Sample Execution](examples/README_en.md): Provides operator development samples and introduces end-to-end sample execution methods.

## 🧰 clangd/IDE Support

- Install clangd (recommended 15+, Ubuntu OS example) and VSCode plugin clangd

  ```bash
  sudo apt install -y clangd-15
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

- Configure `.clangd` in project root directory (example) Complete `.clangd` file is provided in this directory, where CANN header file directory needs to be replaced with actual installation location, default is `/usr/local/Ascend` in `.clangd`.

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
      - "ignored_attributes"
      - "unknown_type_name"
      - "undeclared_var_use"
      - "invalid_token_after_toplevel_declarator"
      - "missing_type_specifier"
      - "typename_nested_not_found"
      - "redefinition"
  ```

- Restart clangd (VSCode: Command Palette -> "Clangd: Restart language server")

- 💡 For any suggestions or improvements regarding ASC language syntax highlighting and code navigation support, community developers are welcome to provide feedback!

## 📖 Related Resources

- **Programming Guide**
  | Document | Description |
  |---------|--------|
  |[Ascend C Programming Guide](https://hiascend.com/document/redirect/CannCommunityOpdevAscendC)|Write operator programs using Ascend C based on Ascend AI hardware, develop custom operators.|
  |[Ascend C Practice Reference](https://hiascend.com/document/redirect/CannCommunityAscendCBestPractice) | Introduces how to further optimize operator performance based on already developed Ascend C operators. |
  |[Ascend C API List](https://hiascend.com/document/redirect/CannCommunityAscendCApi)| Ascend C SIMD&SIMT API, including language extension layer C API, C++ class library basic API and high-level API|
  |[Ascend C Samples](./examples)| Ascend C API key feature introduction samples, including AICore SIMD&SIMT, AICPU, and so on|
  |[Ascend C Programming Guide (Harmony)](https://gitcode.com/cann/cann-recipes-harmony-infer/blob/master/docs/ascendc_develop_guide.md)|Write operator programs using Ascend C based on Kylin AI hardware, develop custom operators.|

- **Contribution Guide**
  | Document | Description |
  |---------|--------|
  |[CANN Community Contribution Guide](https://gitcode.com/cann/community)| CANN community Issue, PR and other general processing procedures|
  |[ASC-DevKit Contribution Guide](./CONTRIBUTING_en.md) | Ascend C API, documentation and samples contribution guide|

- **Others**
  | Document | Description |
  |---------|--------|
  |[Ascend C Meetup Materials](https://gitcode.com/cann/community/tree/master/events/meetup/slides/sig-ascendc)|Ascend C external presentation PPT materials, including 950 new features, and so on|
  |[Ascend C Wiki](https://gitcode.com/cann/asc-devkit/wiki)|Ascend C technical promotion articles, and so on|
  |[CANN-Learning-Hub](https://gitcode.com/cann/cann-learning-hub/tree/master/tutorials/ascendc_operator_development)|Ascend C operator development online full-process tutorial|
  |[Ascend C Ops Samples](https://gitcode.com/cann/cann-samples)|Ascend C operator how to gradually achieve high-performance sample repository|

## 📌 Related Plans

- [Ascend C Development Roadmap (2026 Q2)](https://gitcode.com/cann/asc-devkit/issues/316);

## 📝 Related Information

- [Contribution Guide](CONTRIBUTING_en.md)
- [Security Statement](SECURITY_en.md)
- [License](LICENSE)
