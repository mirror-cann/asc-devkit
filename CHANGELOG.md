# CHANGELOG

## v9.1.0-beta.1
发布日期：2026/04/30
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.1.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.1.0-beta.1) | [v9.1.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.1.0-beta.1) |

### 🚀 关键特性
- Aclrtc支持global函数为模板参数（[PR#1276](https://gitcode.com/cann/asc-devkit/pull/1276)），融合编译支持msobjdump解析（[PR#235](https://gitcode.com/cann/asc-tools/pull/235)），提升编译工程灵活性和调试能力。
- Tensor API分支代码合入主线（[PR#1770](https://gitcode.com/cann/asc-devkit/pull/1770)），正式提供Tensor编程支持。
- C-API新增支持在UB和寄存器之间搬运int4x2_t类型的数据（[PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)），扩展低比特数据搬运能力。
- SIMT API新增DCCI接口（[PR#1510](https://gitcode.com/cann/asc-devkit/pull/1510)）、浮点比较接口（[PR#1693](https://gitcode.com/cann/asc-devkit/pull/1693)）及浮点运算指令（[PR#1774](https://gitcode.com/cann/asc-devkit/pull/1774)），丰富SIMT编程能力。

### 🎯 样例更新
- 存量样例整改：涉及基础、高阶的cube、vector、数据搬运等API样例整改（[PR#1200](https://gitcode.com/cann/asc-devkit/pull/1200) [PR#1429](https://gitcode.com/cann/asc-devkit/pull/1429) [PR#1454](https://gitcode.com/cann/asc-devkit/pull/1454) [PR#1311](https://gitcode.com/cann/asc-devkit/pull/1311) [PR#1563](https://gitcode.com/cann/asc-devkit/pull/1563)）。
- 新增静态Tensor最佳实践：add高性能样例（[PR#1262](https://gitcode.com/cann/asc-devkit/pull/1262)）。
- 新增低比特cube最佳实践：matmul的高阶、基础API样例，MXFP4基础API样例（[PR#1535](https://gitcode.com/cann/asc-devkit/pull/1535) [PR#1648](https://gitcode.com/cann/asc-devkit/pull/1648) [PR#1788](https://gitcode.com/cann/asc-devkit/pull/1788)）。
- 新增自定义算子工程多vendors编译样例（[PR#1466](https://gitcode.com/cann/asc-devkit/pull/1466)）。
- 新增atomic类特性样例（[PR#1688](https://gitcode.com/cann/asc-devkit/pull/1688)）。

### 📖 资料文档
- 新增C-API API手册内容：补充UB与寄存器的搬运接口资料（[PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)）。
- 优化入门教程及SIMT编程模型（[PR#1487](https://gitcode.com/cann/asc-devkit/pull/1487)）。
- 增加CANN包安装及配套说明（[PR#1451](https://gitcode.com/cann/asc-devkit/pull/1451) [PR#1514](https://gitcode.com/cann/asc-devkit/pull/1514)）。
- 新增AscendC入门教程、编程指南、兼容性迁移指南、算子实践参考文档（[PR#1438](https://gitcode.com/cann/asc-devkit/pull/1438)）。

---

## v9.0.0-beta.2
发布日期：2026/03/31
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.0.0-beta.2](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.2) | [v9.0.0-beta.2](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.2) |

### 🚀 关键特性
- Ascend 950PR支持SIMD编程模式，提供200+ [API接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/dav_c310)跨代兼容能力，可实现Atlas A2系列产品和Atlas A3系列产品算子平滑迁移。
- Ascend 950PR新增基于Reg的编程方式，提供Reg数据搬运、基础算术、规约计算、同步控制等90+ [Reg编程接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/reg_compute/dav_c310)。
- Atlas A2系列产品、Atlas A3系列产品、Ascend 950PR支持[语言扩展层纯C接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/c_api)，支持数组式内存分配与指针型计算接口，提供原生纯 C 编程体验。
- Ascend 950PR支持SIMD与SIMT混合编程，提供约700个[SIMT API接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/simt_api)，包含warp、atomic、基本数学计算、类型转换等基础接口。
- Ascend 950PR支持通信高阶API的CCU通信接口，提供基于CCU的[Allreduce，Allgather，Reducescatter，AlltoAll等主流通信原语](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/adv_api/detail/hccl/impl/platform_v310)；Matmul高阶API新增支持[MXFP4/8低比特数据类型的矩阵运算](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/impl/adv_api/detail/matmul/mx_matmul_impl.h)，实现内存占用减半、算力吞吐倍增。
- Ascend 950PR新增及兼容支持样例共计约260个，包含SIMT样例、SIMD样例（框架类、基础API、高阶API、最佳实践等），并按照编程模型和样例类别对[样例目录结构进行调整](https://gitcode.com/cann/asc-devkit/pull/1223)，提升样例目录结构的易读性。
- 融合编译与<<<>>>调用方式支持[CPU模式](https://gitcode.com/cann/asc-tools/pull/138)以及[SIM仿真模式](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/cmake/asc/asc_modules/CMakeASCInformation.cmake)。
### 📖 资料文档
- 新增90+ [Reg编程接口API](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/docs/api/context/Reg%E7%9F%A2%E9%87%8F%E8%AE%A1%E7%AE%97.md)资料，Reg矢量计算API是面向RegBase架构开发的API，用户可通过该API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。
- 新增SIMT[快速入门](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0022.html)、[编程模型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10064.html)和[算子实现](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlasascendc_api_07_10293.html)介绍。
- 新增SIMD与SIMT[混合编程模型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10052.html)、[算子实现](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10039.html)、[性能优化](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_best_practices_10_10029.html)介绍。
- 新增[SIMT API](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0427.html)资料章节。
- 新增[兼容性迁移指南](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_compatibility_10_00001.html)（220x架构版本迁移到351x架构版本）。
- 昇腾社区中，Ascend C算子开发新增[可视化专区](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html)，通过视频呈现Cube和Vector算子的执行过程。
---

## v9.0.0-beta.1
发布日期：2026/02/25
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.0.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.1) | [v9.0.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.1) |

### 🚀 关键特性

- 完成[Gitee样例仓](https://gitee.com/ascend/samples/tree/master/operator/ascendc)AscendC样例及[AscendC高阶API仓](https://gitee.com/ascend/ascendc-api-adv/tree/master/examples)样例到[asc-devkit仓](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.1/examples)的迁移，并统一使用<<<>>>调用方式；
- 融合编译与<<<>>>调用方式支持[CPU模式](https://gitcode.com/cann/asc-tools/pull/138)以及[SIM仿真模式](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.1/cmake/asc/asc_modules/CMakeASCInformation.cmake)。
- Ascend C项目新增对Kirin X90和Kirin 9030处理器的支持。鸿蒙开发者基于Ascend C的实践案例可参考：[cann-recipes-harmony-infer](https://gitcode.com/cann/cann-recipes-harmony-infer)。
---

## v8.5.0
发布日期：2026/01/23
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 8.5.0](https://www.hiascend.com/developer/download/community/result?module=cann&cann=8.5.0) | [v8.5.0](https://gitcode.com/cann/asc-devkit/tags/v8.5.0) |

### 🚀 关键特性

基于Atlas A3 训练系列产品/Atlas A3推理系列产品、Atlas A2训练系列产品/Atlas A2推理系列产品全面开源开放，包含以下新特性：
- 实现分仓分包，支持分包独立安装部署，包括Ascend C算子开发所需的核心仓asc-devkit、调试工具仓[asc-tools](https://gitcode.com/cann/asc-tools)、Vector算子模板库仓[atvc](https://gitcode.com/cann/atvc)和[atvoss](https://gitcode.com/cann/atvoss)、Python前端仓[pyasc](https://gitcode.com/cann/pyasc)。
- 编程API能力扩展
  - 新增语言扩展层C API，提供与业界相似的编程体验。
  - 基础API新增LocalMemAllocator内存分配接口。
- 全面支持异构编译与<<<>>> 直调，通过文件后缀名“.asc”或编译选项“-x asc”使能异构编译。
- 算子编译CMake接口标准化，提供Cmake module接口，支持不同编译场景。
- 支持CPU&NPU孪生调试的能力一致性，一套代码同时支持CPU和NPU调试。
- 编程指南全面优化。
- 新增算子样例。
