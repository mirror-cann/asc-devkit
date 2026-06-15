<div align="center">

# Ascend C

<h4>基于C/C++构建多层级API，开放芯片完备编程能力，支撑实现极致性能</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](https://gitcode.com/cann/asc-devkit/tree/master/examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](https://gitcode.com/cann/asc-devkit/blob/master/LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](https://gitcode.com/cann/asc-devkit/blob/master/CONTRIBUTING.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)

</div>

## 🔥Latest News
[2026/03] v9.0.0-beta.2 版本关键特性
### 🚀 关键特性
- Ascend 950PR支持SIMD编程模式，提供200+ [API接口](./impl/basic_api/dav_3510)跨代兼容能力，可实现Atlas A2系列产品和Atlas A3系列产品算子平滑迁移。
- Ascend 950PR新增基于Reg的编程方式，提供Reg数据搬运、基础算术、规约计算、同步控制等90+ [Reg编程接口](./impl/basic_api/reg_compute/dav_3510)。
- Atlas A2系列产品、Atlas A3系列产品、Ascend 950PR支持[语言扩展层纯C接口](./include/c_api)，支持数组式内存分配与指针型计算接口，提供原生纯 C 编程体验。
- Ascend 950PR支持SIMD与SIMT混合编程，提供约700个[SIMT API接口](./include/simt_api)，包含warp、atomic、基本数学计算、类型转换等基础接口。
- Ascend 950PR支持通信高阶API的CCU通信接口，提供基于CCU的[Allreduce，Allgather，Reducescatter，AlltoAll等主流通信原语](./impl/adv_api/detail/hccl/impl/platform_v310)；Matmul高阶API新增支持[MXFP4/8低比特数据类型的矩阵运算](./impl/adv_api/detail/matmul/mx_matmul_impl.h)，实现内存占用减半、算力吞吐倍增。
- Ascend 950PR新增及兼容支持样例共计约260个，包含[SIMT样例](./examples/03_simt_api)、[SIMD样例](./examples/01_simd_cpp_api)（框架类、基础API、高阶API、最佳实践等），并按照编程模型和样例类别对[样例目录结构进行调整](https://gitcode.com/cann/asc-devkit/pull/1223)，提升样例目录结构的易读性。
- 融合编译与<<<>>>调用方式支持[CPU模式](https://gitcode.com/cann/asc-tools/pull/138)以及[SIM仿真模式](https://gitcode.com/cann/asc-devkit/blob/master/cmake/asc/asc_modules/CMakeASCInformation.cmake)。
### 📖 资料文档
- 新增90+ [Reg编程接口API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMD-API/基础API/Reg矢量计算/Reg矢量计算.md)资料，Reg矢量计算API是面向RegBase架构开发的API，用户可通过该API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。
- 新增SIMT[快速入门](docs/guide/入门教程/快速入门/基于SIMT编程/Gather算子快速入门.md)、[编程模型](docs/guide/编程指南/编程模型/AI-Core-SIMT编程/抽象硬件架构.md)和[算子实现](docs/guide/算子实践参考/SIMT算子实现/基础知识.md)介绍。
- 新增SIMD与SIMT[混合编程模型](docs/guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程.md)、[算子实现](docs/guide/算子实践参考/SIMD与SIMT混合算子实现/基础知识.md)、[性能优化](docs/guide/算子实践参考/SIMD与SIMT混合算子性能优化/内存访问/使用Unified-Buffer提升内存访问效率.md)介绍。
- 新增[SIMT API](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/SIMT-API.md)资料章节。
- 新增[兼容性迁移指南](docs/guide/跨代迁移兼容性指南/Ascend-C-API兼容策略.md)（220x架构版本迁移到351x架构版本）。
- 昇腾社区中，Ascend C算子开发新增[可视化专区](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html)，通过视频呈现Cube和Vector算子的执行过程。

有关所有历史版本及更新的详细信息，请参阅[CHANGELOG.md](./CHANGELOG.md)

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

<img src="docs/guide/figures/architecture_ascendc.png" alt="架构图"  width="1000px">

- **语言扩展层C API**：纯C接口，支持数组分配内存、基于指针的计算接口，提供与业界一致的C语言编程体验，并开放芯片完备编程能力。Atlas A2/A3支持SIMD的纯C接口；Ascend 950PR/Ascend 950DT将支持与业界类似的SIMT编程能力、SIMD/SIMT混合编程能力；
- **基础API**：单指令抽象的C++类库API，一般基于Tensor编程；逐步基于Layout完善Tensor编程能力；
- **高阶API**：基于单核对常见算法进行抽象和封装，提供公共算法的实现；
- **算子模板库**：基于模板提供算子的完整实现参考，简化Tiling开发，支持用户自定义扩展；
- **Python前端PyAsc**：PyAsc基于Python前端，提供芯片底层完备编程能力，并将逐步基于Layout完善Tensor编程能力，新增SIMT编程等能力，实现基于Python接口开发高性能算子；

### 如何选择多层级API进行算子开发
- **基于C/C++语言开发**：详细请参考[Ascend C多级API选择指南](./docs/asc_how_to_choose_api.md)
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
│   ├── tensor_api                      # Ascend C TENSOR API实现源代码
│   ├── simt_api                        # Ascend C SIMT API实现源代码
│   └── utils                           # Ascend C 工具类实现源代码
├── include                             # Ascend C API接口声明源代码
│   ├── adv_api                         # Ascend C 高阶API声明源代码
│   ├── aicpu_api                       # Ascend C AI CPU API声明源代码
│   ├── basic_api                       # Ascend C 基础API声明源代码
│   ├── c_api                           # Ascend C 语言扩展层C API声明源代码
│   ├── tensor_api                      # Ascend C TENSOR API声明源代码
│   ├── simt_api                        # Ascend C SIMT API声明源代码
│   └── utils                           # Ascend C 工具类声明源代码
├── scripts                             # 打包相关脚本
├── tests                               # Ascend C API的UT用例
└── tools                               # Ascend C 工具源代码
```

## ⚡️快速入门

若您希望快速体验项目的构建和算子样例的执行，请访问如下文档获取简易教程。

- [编译构建](docs/quick_start.md)：介绍搭建环境、编译执行、本地验证等操作。
- [样例执行](examples/README.md)：提供算子开发样例，介绍端到端执行样例的方式。

## 🧰clangd/IDE 支持

- 安装 clangd（推荐 15+，以Ubuntu操作系统为例）以及VSCode插件clangd

  ```bash
  sudo apt install -y clangd-15
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

- 在项目根目录下配置 `.clangd`（示例）完整 `.clangd`文件在本目录下给出，其中涉及 CANN 头文件目录需自行替换实际安装位置，`.clangd`中默认为`/usr/local/Ascend`.

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

- 重启clangd（VSCode: Command Palette -> "Clangd: Restart language server"）

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
