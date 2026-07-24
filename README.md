<div align="center">

# Ascend C

<h4>基于C/C++构建多层级API，开放芯片完备编程能力，支撑实现极致性能</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](CONTRIBUTING.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)
[![VS Marketplace](https://vsmarketplacebadges.dev/version/HICANN.ascend-c-toolkit.svg)](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit)
[![Installs](https://vsmarketplacebadges.dev/installs-short/HICANN.ascend-c-toolkit.svg)](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit)

</div>

## 🔥Latest News
[2026/05] v9.1.0-beta.2 版本关键特性
### 🚀 关键特性
- Ascend C框架基础API支持NPU Check（[PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557) [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)），增强算子运行时校验能力。
- SIMD VF内支持printf和reg dump打印（[PR#1605](https://gitcode.com/cann/asc-devkit/pull/1605)），提供调试打印和寄存器数据dump能力。
- A5支持L1 Tensor数据的DumpTensor（[PR#2175](https://gitcode.com/cann/asc-devkit/pull/2175)），扩展L1层数据调试支持。
- 编译工程CMakeModule支持CMAKE<LANG>编译选项（[PR#2055](https://gitcode.com/cann/asc-devkit/pull/2055)）；新增optype_collector工具，支持检查optype重名（[PR#285](https://gitcode.com/cann/asc-tools/pull/285)）。
- 基础API支持ctrl中的功能行为（饱和溢出管理）（[PR#2077](https://gitcode.com/cann/asc-devkit/pull/2077)）。
- SIMT编程新增ld/st接口（[PR#2058](https://gitcode.com/cann/asc-devkit/pull/2058)）和AddrSpace类接口（[PR#1597](https://gitcode.com/cann/asc-devkit/pull/1597)），丰富SIMT内存访问编程能力。
### 🎯 样例更新
- 最佳实践样例开发：matmul+gelu融合、datacopy优化、bank冲突优化、group_matmul量化组矩阵乘、simt&simd高性能编程（[PR#1814](https://gitcode.com/cann/asc-devkit/pull/1814) [PR#2137](https://gitcode.com/cann/asc-devkit/pull/2137) [PR#2141](https://gitcode.com/cann/asc-devkit/pull/2141) [PR#2166](https://gitcode.com/cann/asc-devkit/pull/2166) [PR#2363](https://gitcode.com/cann/asc-devkit/pull/2363)）。
- Ascend 950新特性补充及兼容性样例整改：loopmode数据搬运、interleave矢量计算、datacopy_gm2l1、loadmx（Load2DMX）、mmad_mx、data_copy_pad等（[PR#2336](https://gitcode.com/cann/asc-devkit/pull/2336) [PR#1899](https://gitcode.com/cann/asc-devkit/pull/1899) [PR#2124](https://gitcode.com/cann/asc-devkit/pull/2124)）。
- RegBase新增基础样例：基础算术、数据类型转换、归约、比较、索引等样例（[PR#1459](https://gitcode.com/cann/asc-devkit/pull/1459) [PR#1575](https://gitcode.com/cann/asc-devkit/pull/1575) [PR#2024](https://gitcode.com/cann/asc-devkit/pull/2024)）。
- 新增SIMD VF print样例和dump样例（[PR#2558](https://gitcode.com/cann/asc-devkit/pull/2558)）。
- SIMT新增DCache访问优化样例（[PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)）、基于transpose的仿存合并和bank冲突样例（[PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)）、最佳实践样例：通过类型对齐提升搬运效率（[PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)）。
- SIMT新增功能特性样例：pytorch注册自定义算子（[PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)）、编译相关样例（动态、静态、分离编译等）（[PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)）、profiling样例（[PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)）、内存屏障特性样例（[PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)）、Warp类特性样例（[PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)）、simulator样例（[PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)）、kernel log样例（[PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)）。
- SIMT入门样例修改为gather（[PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)）。
- 新增Tensor API入门及最佳实践样例：Matmul入门、数据搬入搬出、搬出随路量化、MX FP4最佳实践（[PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)）。
### 📖 资料文档
- 新增矩阵计算概述和计算分形介绍的文档（[PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)）。
- 优化矢量计算API文档，补充指令约束等（[PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)）。
- 搭建VitePress文档站点，提供AscendC资料预览功能（[PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)）。
- 增加SIMD与SIMT混合编程性能优化概述（[PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)）。

有关所有历史版本及更新的详细信息，请参阅[CHANGELOG.md](./CHANGELOG.md)。

## 🚀概述

[Ascend C](https://www.hiascend.com/cann/ascend-c)是CANN（Compute Architecture for Neural Networks）推出的昇腾AI处理器专用的算子程序开发语言，原生支持C和C++标准规范。作为一门面向多场景的编程语言，Ascend C不仅致力于**开放芯片完备编程能力支撑实现极致性能**，同时通过多层级编程API设计，让您能够根据项目需求、团队技能与性能目标，灵活选择最合适的API，在开发效率与运行性能之间取得最佳平衡。

### 设计目标

Ascend C的设计目标可概括为 **“高性能、完备性、易编程、可调试和兼容性”**。其通过对C/C++语言标准进行最小化扩展，既支持基于指针的C语言开发习惯，也支持基于Tensor的C++编程范式，在支撑昇腾算子高效开发的同时，实现与现有生态的无缝衔接，保障开发体验的一致性。

我们秉持以下核心理念：
- **没有银弹**：不同场景对性能、开发效率的要求各异，单一接口无法最优适配所有场景；
- **渐进式学习**：新手可从易用性接口入手快速验证算法；专家则可向下钻取、精细调优，借助复杂接口特性充分挖掘硬件潜能。

### API层级
Ascend C提供三类接口，均可实现底层的完备编程能力：

| API层级 | 语言  | 特点 | 目标用户 | 主要用途 |
|----------|----------|----------|----------|----------|
| **Tpipe/Tque框架编程API** |  **C++** |基于**Tensor**编程<br>通过Tpipe/Tque框架统一管理内存与同步| 算子库开发者| 基于框架自动管理同步与内存，<br>提升编程易用性|
| **基础API** | **C++** |基于**Tensor**编程，提供**C++基础完备编程能力**<br>通过MakeTensor/LocalMemoryAllocator分配Tensor，自主管理同步| 算子库开发者|自主管理同步与内存<br>匹配C++Tensor开发习惯，支撑实现极致性能|
| **语言扩展层<br>SIMD&SIMT API** |**C**|基于**指针**编程，提供**C基础完备编程能力**<br>通过数组[]分配内存，自主管理同步|算子库开发者 |自主管理同步与内存<br>匹配C语言开发习惯，支撑实现极致性能|


此外，Ascend C提供高阶API和算子模板库以便提升算子开发效率。

| API层级 |  目标用户 | 主要用途 |
|----------|----------|----------|
| **算子模板库 (CATLASS/ATVOSS等)** |  算法开发人员 | 基于典型算子实现进行自定义扩展，满足特定场景高性能需求 |
| **高阶API** |算法开发人员 | 复用通用单核算法，快速完成算法验证 |


其总体逻辑架构图如下所示：

<img src="docs/zh/guide/figures/architecture_ascendc.png" alt="架构图"  width="1000px">

- **语言扩展层C API**：纯C接口，支持数组分配内存、基于指针的计算接口，提供与业界一致的C语言编程体验，并开放芯片完备编程能力。Atlas A2/A3支持SIMD的纯C接口；Ascend 950PR/Ascend 950DT将支持与业界类似的SIMT编程能力、SIMD/SIMT混合编程能力；
- **基础API**：单指令抽象的C++类库API，一般基于Tensor编程；逐步基于Layout完善Tensor编程能力；
- **高阶API**：基于单核对常见算法进行抽象和封装，提供公共算法的实现；
- **算子模板库**：基于模板提供算子的完整实现参考，简化Tiling开发，支持用户自定义扩展；
- **Python前端PyAsc**：PyAsc基于Python前端，提供芯片底层完备编程能力，并将逐步基于Layout完善Tensor编程能力，新增SIMT编程等能力，实现基于Python接口开发高性能算子；

### 如何选择多层级API进行算子开发
- **基于C/C++语言开发**：详细请参考[Ascend C多级API选择指南](./docs/zh/asc_how_to_choose_api.md)
- **基于Python语言开发，支撑完备编程能力，实现极致性能**：推荐选用Ascend C Python前端[PyAsc](https://gitcode.com/cann/pyasc)
- **基于Python语言开发，快速开发验证，易用性优先**：推荐选用 [PyPTO](https://gitcode.com/cann/pypto)


## 🔍目录结构说明
本仓主要包含Ascend C编程API和必要的cmake编译脚本，是算子开发所需的核心模块，其目录结构如下：

```
├── cmake                               # Ascend C 构建源代码
├── docs                                # 项目文档介绍
├── examples                            # Ascend C API样例工程
├── impl                                # Ascend C API接口实现源代码
│   ├── adv_api                         # Ascend C 高阶API实现源代码
│   ├── aicpu_api                       # Ascend C AI CPU API实现源代码
│   ├── basic_api                       # Ascend C 基础API实现源代码
│   ├── c_api                           # Ascend C 语言扩展层C API实现源代码
│   ├── simt_api                        # Ascend C SIMT API实现源代码
│   ├── tensor_api                      # Ascend C TENSOR API实现源代码
│   └── utils                           # Ascend C 工具类实现源代码
├── include                             # Ascend C API接口声明源代码
│   ├── adv_api                         # Ascend C 高阶API声明源代码
│   ├── aicpu_api                       # Ascend C AI CPU API声明源代码
│   ├── basic_api                       # Ascend C 基础API声明源代码
│   ├── c_api                           # Ascend C 语言扩展层C API声明源代码
│   ├── simt_api                        # Ascend C SIMT API声明源代码
│   ├── tensor_api                      # Ascend C TENSOR API声明源代码
│   └── utils                           # Ascend C 工具类声明源代码
├── scripts                             # 打包相关脚本
├── tests                               # Ascend C API的UT用例
└── tools                               # Ascend C 工具源代码
```

## ⚡️快速入门

若您希望快速体验项目的构建和算子样例的执行，请访问如下文档获取简易教程。

- [编译构建](docs/zh/quick_start.md)：介绍搭建环境、编译执行、本地验证等操作。
- [样例执行](examples/README.md)：提供算子开发样例，介绍端到端执行样例的方式。

## 🧰clangd/IDE 支持

- 安装 clangd（推荐 15+，以Ubuntu操作系统为例）以及VSCode插件clangd

  ```bash
  sudo apt install -y clangd-15
  sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-15 100
  ```

- 配置本地VSCode的`settings.json`（示例）

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

- 在项目根目录下配置 `.clangd`（示例）完整 `.clangd`文件在本目录下给出，其中涉及 CANN 头文件目录需对应实际安装位置，`.clangd`中默认为`/usr/local/Ascend`.

  如果 CANN 安装在非默认路径，或需要切换 NPU 架构宏，可先 source CANN `set_env.sh`，再通过脚本基于 `.clangd.in` 生成本地配置：

  ```bash
  source /path/to/cann/set_env.sh
  python3 scripts/setup_clangd.py --npu-arch 2201 --output .clangd.local
  ```

  `ASCEND_HOME_PATH` 由 `set_env.sh` 设置，脚本会使用该环境变量生成真实 CANN 路径；如果未设置，脚本会提示 source `set_env.sh` 后重试。生成文件默认为 `.clangd.local`，不会覆盖仓库内置 `.clangd`；如需让项目级 clangd 配置生效，可按需复制为 `.clangd` 后重启 clangd。

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
      - "invalid_token_after_toplevel_declarator"
      - "missing_type_specifier"
      - "typename_nested_not_found"
      - "redefinition"
  ```

- 重启clangd（VS Code: Command Palette -> "Clangd: Restart language server"）

- 推荐安装VS Code扩展[Ascend C Toolkit](https://marketplace.visualstudio.com/items?itemName=HICANN.ascend-c-toolkit)，用于在VS Code中完成Ascend C算子工程的编辑、编译、运行、调试、异常检测与性能调优等流程。打开本仓工作区时，VS Code会根据`.vscode/extensions.json`提示安装推荐扩展。

- Ascend C Toolkit近期更新重点增强Atlas A2系列产品和Atlas A3系列产品的环境解析、标准自定义算子创建、SoC设置、仿真分析、NPU调试、设置迁移与稳定性；完整使用说明和问题反馈请参考[CANN讨论区使用指导](https://gitcode.com/org/cann/discussions/54)。

- 💡 关于 ASC 语言语法高亮、代码跳转的支持，如有任何建议或改进意见，欢迎社区开发者积极反馈！

## 📖相关资源

- **编程指南**
  | 文档  |  说明   |
  |---------|--------|
  |[Ascend C 编程指南](https://hiascend.com/document/redirect/CannCommunityOpdevAscendC)|基于昇腾AI硬件，使用Ascend C编写算子程序，开发自定义算子。|
  |[Ascend C 实践参考](https://hiascend.com/document/redirect/CannCommunityAscendCBestPractice) | 基于已完成开发的Ascend C算子，介绍如何进一步优化算子性能。 |
  |[Ascend C API列表](https://hiascend.com/document/redirect/CannCommunityAscendCApi)| Ascend C SIMD&SIMT API，包括语言扩展层C API、C++类库基础API和高阶API|
  |[Ascend C 样例](./examples)| Ascend C API关键特性介绍样例，包括AICore SIMD&SIMT、AICPU等|
  |[Ascend C 编程指南（鸿蒙）](https://gitcode.com/cann/cann-recipes-harmony-infer/blob/master/docs/ascendc_develop_guide.md)|基于麒麟AI硬件，使用Ascend C编写算子程序，开发自定义算子。|

- **贡献指南**
  | 文档  |  说明   |
  |---------|--------|
  |[CANN 社区贡献指南](https://gitcode.com/cann/community)| CANN社区Issue、PR等通用处理流程|
  |[ASC-DevKit贡献指南](./CONTRIBUTING.md) | Ascend C API、资料与样例等贡献指南|

- **其他**
  | 文档  |  说明   |
  |---------|--------|
  |[Ascend C Meetup材料](https://gitcode.com/cann/community/tree/master/events/meetup/slides/sig-ascendc)|Ascend C对外宣传的PPT材料，包括950新增特性等|
  |[Ascend C Wiki](https://gitcode.com/cann/asc-devkit/wiki)|Ascend C技术宣传文章等|
  |[CANN-Learning-Hub](https://gitcode.com/cann/cann-learning-hub/tree/master/tutorials/ascendc_operator_development)|Ascend C算子开发在线全流程教程|
  |[Ascend C Ops Samples](https://gitcode.com/cann/cann-samples)|Ascend C 算子如何逐步实现高性能样例仓|

## 📌相关规划

- [Ascend C Development Roadmap (2026 Q2)](https://gitcode.com/cann/asc-devkit/issues/316)；

## 📝相关信息

- [贡献指南](CONTRIBUTING.md)
- [安全声明](SECURITY.md)
- [许可证](LICENSE)
