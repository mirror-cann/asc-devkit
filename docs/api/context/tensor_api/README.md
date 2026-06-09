# Tensor API

Tensor API 是基于Ascend C的C++模板抽象库的张量编程接口，专为定义和操作层次化多维数据布局而设计。Tensor API提供Layout和Tensor对象，将数据类型、形状、存储空间和内存布局紧凑地封装在一起，并为用户自动处理复杂的索引计算，让开发者能以直观、逻辑化的方式访问数据，而无需手动计算底层地址。

Tensor API将张量Tensor看作是带有布局语义的数据视图，并由内存引擎（Engine）和数据布局（Layout）为张量的两个正交组成部分。Engine 持有硬件指针并负责底层内存访问，Layout 携带形状、步幅和布局格式，在编译期将多维坐标自动换算为线性偏移。开发者不再需要手动计算，可直接通过坐标方式访问数据，分形布局（NZ/ZN/ND 等）的复杂映射被封装在 Layout 内部。并可通过切片操作通过返回保留完整Shape和Stride信息的子张量Tensor，使数据操作可组合、可传递，在将复杂的硬件细节封装在底层的同时，通过模板参数暴露足够的控制点，让高级用户能够对硬件行为进行精细调优。基于Layout所描述的数据组织方式，Atom（原子操作）​ 进一步封装了底层硬件指令，它将操作类型和特征参数组合在一起，成为执行最小单元的计算或数据搬运操作的基础,并统一由Copy/Mmad作为调用入口和分发逻辑。

Tensor API当前仅支持Ascend 950PR/Ascend 950DT。

## Tensor API架构

**图 1**  Tensor API架构图

![](figures/tensor_api_arch.png)

Tensor API 采用了分层设计，各层分别承担数据表达、操作封装和指令调用三类不同职责。
Tensor层：聚合Engine和Layout，提供坐标访问、切片等用户接口。
Atom层: 将操作类型和特征参数组合，映射到昇腾处理器的硬件指令。
Arch层: 封装硬件指令的底层接口，对参数进行处理后调用硬件指令。

第一层是 Tensor 层。该层负责描述“数据是什么”以及“数据应如何被访问”。其核心对象是 Tensor，Tensor通过聚合Engine与Layout，将底层存储地址、多维逻辑形状、步幅以及布局格式统一为一个可组合的数据视图。对用户而言，Tensor 层提供的是数据访问接口，包括坐标访问、子Tensor构造、切片、获取Shape/Stride等。

第二层是 Atom 层。Atom 层作为Tensor层与Arch层之间的桥梁，负责描述“对这些数据执行什么操作”以及“该操作携带哪些特征参数”。CopyAtom和MmadAtom分别实现了搬运和计算的原子操作封装。Atom并不直接实现具体硬件指令，而是将操作类别、默认参数traits、可配置参数以及参数展开逻辑组织在一起，形成一类可调用、可配置、可分发的操作对象。并通过with等机制，提供了对特征参数的配置能力。

第三层是 Arch 层。该层负责“在当前硬件架构、数据位置和布局条件下，具体应当调用哪条实现路径”的操作。Arch层一方面封装面向具体架构的底层指令接口，另一方面通过routing机制根据物理位置、Layout分形、数据类型以及 版本号进行静态分发。以 Copy 和 Mmad 为例，用户只需要调用统一入口，最终由Arch层判断本次操作应走GM到UB、L1到L0A或其他特定路径，并进一步落到对应架构目录下的具体实现文件中。

## 接口调用

Tensor API的接口调用一般遵循以下四个步骤：

1. **构造内存指针**：通过MakeMemPtr的模板参数标记数据所在的物理存储位置（GM / L1 / L0A / L0B / L0C / UB / BiasTable Buffer / Fixpipe Buffer）；
2. **构造数据布局**：通过MakeFrameLayout（标准分形排布，如 ND / NZ / ZN / DN）或MakeLayout（自由形状+步长）描述张量的形状、步长和排布模式；
3. **构造张量**：通过MakeTensor(ptr, layout)将内存指针与布局绑定，生成带有完整类型信息的属性张量；
4. **调用操作接口**：将张量传入 Copy（数据搬运）或Mmad（矩阵计算），编译器根据张量的存储位置、布局模式和数据类型自动选择底层硬件指令实现。

## 参考资源

### 数据结构和列表

- [Layout](./data_structure/layout_structure/Layout和层次化表述法.md)：张量的内存排布描述，由Shape（形状）和Stride（步长）组成。支持层次化表达，可描述多重分形格式。
- [Shape](./data_structure/layout_structure/Shape.md)：张量的维度大小描述。
- [Stride](./data_structure/layout_structure/Stride.md)：张量的维度步长描述。
- [Coord](./data_structure/layout_structure/Coord.md)：张量的多维坐标描述。
- [Tensor](./data_structure/tensor_structure/Tensor.md)：包含指向张量位置的指针和用于访问其元素的Layout。

完整接口目录请参考[Tensor API手册资料目录导览](tensor_api.md)。

### 测试和样例

本仓库提供[测试用例](../../../../tests/api/tensor_api) 以及[示例代码1](../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_tensor_api)，[示例代码2](../../../../examples/01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/mmad_tensor_api),[示例代码3](../../../../examples/01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/copy_in_tensor_api),[示例代码4](../../../../examples/01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/copy_out_tensor_api),[示例代码5](../../../../examples/01_simd_cpp_api/04_best_practices/01_matrix_compute_practices/matmul_mxfp4_tensor_api_high_performance)方便快速了解功能用法并进行验证。
