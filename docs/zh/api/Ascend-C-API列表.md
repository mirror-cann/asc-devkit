# Ascend C API列表

Ascend C提供了一套层次化的API体系，涵盖了从底层C扩展到高阶C++类库的完整能力。它支持开发者以标准C/C++语法为基础，在AI Core\(SIMD/SIMT\)及AI CPU等多种编程模型下，灵活实现精细化的内存管理与高效的矢量/矩阵运算。

## API分类总览

下表展示了Ascend C API的总体分类，帮助开发者根据编程模型和功能需求快速定位所需API。

| API一级分类 | API二级分类 | 分类说明 |
| --- | --- | --- |
| [SIMD API](SIMD-API/基础API/基础API列表.md) | [基础API](SIMD-API/基础API/基础API.md) | 实现对硬件能力的抽象，开放芯片的能力，保证完备性和兼容性。标注为ISASI（Instruction Set Architecture Special Interface，硬件体系结构相关的接口）类别的API，不能保证跨硬件版本兼容。 |
| [SIMD API](SIMD-API/C-API/C-API.md) | [C API](SIMD-API/C-API/C-API.md) | 纯C接口，开放芯片完备编程能力，支持数组分配内存，一般基于指针编程，提供与业界一致的C语言编程体验。 |
| [SIMD API](SIMD-API/高阶API/高阶API列表.md) | [高阶API](SIMD-API/高阶API/高阶API.md) | 实现一些常用的计算算法，用于提高编程开发效率，通常会调用多种基础API实现。高阶API包括数学库、Matmul、Softmax等API。高阶API可以保证兼容性。 |
| [SIMT API](SIMT-API/概述.md) | - | 对标业界，提供单指令多线程API。以单条指令多个线程的形式来实现并行计算。SIMT编程主要用于向量计算，特别适合处理离散访问、复杂控制逻辑等场景。SIMT API支持两种编程模型：SIMT编程、SIMD与SIMT混合编程，具体支持的API请分别参见[SIMT编程API列表](SIMT-API/SIMT编程简介/API列表.md)、[SIMD与SIMT混合编程API列表](SIMT-API/SIMD与SIMT混合编程简介/API列表-148.md)。 |
| [AI CPU API](AI-CPU-API/AI-CPU-API列表.md) | - | 通常作为上述API的补充，主要承担非矩阵类、逻辑比较复杂的分支密集型计算。 |
| [Utils API](Utils-API/Utils-API列表.md) | - | 丰富的通用工具类，涵盖标准库（目前仅支持SIMD）、平台信息获取、运行时编译及日志输出等功能，支持开发者高效实现算子开发与性能优化。 |
