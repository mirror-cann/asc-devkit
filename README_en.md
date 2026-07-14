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
[2026/05] Key features of v9.1.0-beta.2
### 🚀 Key Features
- The Ascend C framework basic API supports NPU Check ([PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557) [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)), enhancing operator runtime verification capability.
- SIMD VF supports printf and reg dump printing ([PR#1605](https://gitcode.com/cann/asc-devkit/pull/1605)), providing debug printing and register data dump capability.
- A5 supports DumpTensor for L1 Tensor data ([PR#2175](https://gitcode.com/cann/asc-devkit/pull/2175)), extending L1 layer data debugging support.
- The compilation project CMakeModule supports CMAKE<LANG> compilation options ([PR#2055](https://gitcode.com/cann/asc-devkit/pull/2055)); adds the optype_collector tool, supporting optype duplicate name checking ([PR#285](https://gitcode.com/cann/asc-tools/pull/285)).
- The basic API supports functional behavior in ctrl (saturation overflow management) ([PR#2077](https://gitcode.com/cann/asc-devkit/pull/2077)).
- SIMT programming adds ld/st interfaces ([PR#2058](https://gitcode.com/cann/asc-devkit/pull/2058)) and AddrSpace class interfaces ([PR#1597](https://gitcode.com/cann/asc-devkit/pull/1597)), enriching SIMT memory access programming capability.
### 🎯 Sample Updates
- Best practice sample development: matmul+gelu fusion, datacopy optimization, bank conflict optimization, group_matmul quantized group matrix multiplication, and simt&simd high-performance programming ([PR#1814](https://gitcode.com/cann/asc-devkit/pull/1814) [PR#2137](https://gitcode.com/cann/asc-devkit/pull/2137) [PR#2141](https://gitcode.com/cann/asc-devkit/pull/2141) [PR#2166](https://gitcode.com/cann/asc-devkit/pull/2166) [PR#2363](https://gitcode.com/cann/asc-devkit/pull/2363)).
- Ascend 950 new feature supplements and compatibility sample rectification: loopmode data movement, interleave vector computation, datacopy_gm2l1, loadmx (Load2DMX), mmad_mx, data_copy_pad, and so on ([PR#2336](https://gitcode.com/cann/asc-devkit/pull/2336) [PR#1899](https://gitcode.com/cann/asc-devkit/pull/1899) [PR#2124](https://gitcode.com/cann/asc-devkit/pull/2124)).
- RegBase adds basic samples: basic arithmetic, data type conversion, reduction, comparison, indexing, and other samples ([PR#1459](https://gitcode.com/cann/asc-devkit/pull/1459) [PR#1575](https://gitcode.com/cann/asc-devkit/pull/1575) [PR#2024](https://gitcode.com/cann/asc-devkit/pull/2024)).
- Adds SIMD VF print samples and dump samples ([PR#2558](https://gitcode.com/cann/asc-devkit/pull/2558)).
- SIMT adds DCache access optimization samples ([PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)), transpose-based memory coalescing and bank conflict samples ([PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)), and best practice samples for improving data movement efficiency through type alignment ([PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)).
- SIMT adds functional feature samples: PyTorch custom operator registration ([PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)), compilation-related samples (dynamic, static, separate compilation, and so on) ([PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)), profiling samples ([PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)), memory barrier feature samples ([PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)), Warp class feature samples ([PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)), simulator samples ([PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)), and kernel log samples ([PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)).
- The SIMT getting-started sample is modified to gather ([PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)).
- Adds Tensor API getting-started and best practice samples: Matmul getting started, data movement in/out, data movement out with on-path quantization, and MX FP4 best practices ([PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)).
### 📖 Documentation
- Adds matrix computation overview and computation fractal introduction documentation ([PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)).
- Optimizes vector computation API documentation, supplementing instruction constraints, and so on ([PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)).
- Sets up a VitePress documentation site, providing AscendC documentation preview functionality ([PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)).
- Adds SIMD and SIMT hybrid programming performance optimization overview ([PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)).

For detailed information about all historical versions and updates, see [CHANGELOG.md](./CHANGELOG_en.md).

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

<img src="docs/zh/guide/figures/architecture_ascendc.png" alt="Architecture Diagram"  width="1000px">

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
