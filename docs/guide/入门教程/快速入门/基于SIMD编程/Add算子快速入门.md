# Add算子快速入门<a name="ZH-CN_TOPIC_0000002500781060"></a>

本示例为入门级演示，基于Ascend C SIMD实现Add算子，引导您快速完成实践，内容涵盖Device端核函数实现、Host端调用以及编译运行的完整流程，助您建立整体认知。开始前，请参考[环境准备](../../环境准备.md)安装所需的CANN软件包。

以下分别介绍基于C语言API和C++ Tensor的两种典型实现方式。

- **Add算子功能介绍**：

  Add算子的数学表达式为：

  ![](../../../figures/zh-cn_formulaimage_0000002501241896.png)

  计算逻辑为逐元素完成`z = x + y`。

- **Device端代码实现**：

  后缀名为`*.asc`的代码文件可同时包含Host端与Device端代码，Device端部分示例如下：
  - **基于C语言API实现Memory矢量计算示例**
      ```cpp
      __vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
      {
          asc_init();

          constexpr uint32_t block_length = TOTAL_LENGTH / NUM_BLOCKS;

          __gm__ float* x_gm = x + block_idx * block_length;
          __gm__ float* y_gm = y + block_idx * block_length;
          __gm__ float* z_gm = z + block_idx * block_length;

          __ubuf__ float x_local[block_length];
          __ubuf__ float y_local[block_length];
          __ubuf__ float z_local[block_length];

          asc_copy_gm2ub((__ubuf__ void*)x_local, (__gm__ void*)x_gm, block_length * sizeof(float));
          asc_copy_gm2ub((__ubuf__ void*)y_local, (__gm__ void*)y_gm, block_length * sizeof(float));
          asc_sync();

          asc_add(z_local, x_local, y_local, block_length);
          asc_sync();

          asc_copy_ub2gm((__gm__ void*)z_gm, (__ubuf__ void*)z_local, block_length * sizeof(float));
          asc_sync();
      }
      ```
      > [!NOTE] 说明
      > - 本Memory矢量计算示例支持以下型号：
      >     - Atlas A3训练系列产品/Atlas A3推理系列产品
      >     - Atlas A2训练系列产品/Atlas A2推理系列产品
      > - SIMD算子的Kernel函数需要额外修饰符，[`__vector__`](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)修饰符表明该算子仅在向量计算单元上执行。

  - **基于C++ Tensor实现Memory矢量计算示例**

      ```cpp
      template <uint32_t blockLength>
      __vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
      {
          AscendC::InitSocState();

          AscendC::GlobalTensor<float> xGm, yGm, zGm;
          xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);
          yGm.SetGlobalBuffer(y + block_idx * blockLength, blockLength);
          zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);

          AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
          AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
          AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
          AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

          AscendC::DataCopy(xLocal, xGm, blockLength);
          AscendC::DataCopy(yLocal, yGm, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();

          AscendC::Add(zLocal, xLocal, yLocal, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();

          AscendC::DataCopy(zGm, zLocal, blockLength);
          AscendC::PipeBarrier<PIPE_ALL>();
      }
      ```
      > [!NOTE] 说明
      > - 该样例支持以下型号：
      >     - Ascend 950PR/Ascend 950DT
      >     - Atlas A3训练系列产品/Atlas A3推理系列产品
      >     - Atlas A2训练系列产品/Atlas A2推理系列产品
      > - SIMD算子的Kernel函数需要额外修饰符，[`__vector__`](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md)修饰符表明该算子仅在向量计算单元上执行。

- **Host端代码实现**：

  Host端通过`<<<>>>`语法糖调用Device端核函数，示例代码如下：
  ```cpp
    int32_t main(int argc, char const *argv[])
    {
        ...
        constexpr uint32_t numBlocks = 8;
        ...
        // Launch kernel <<<NumBlocks, Block, Dynamic memory, Stream>>>
        // numBlocks : Number of Blocks
        // 0 : No dynamic memory required in this sample
        // nullptr : Use default stream

        // Example One：Call add kernel which is implemented by SIMD C API
        add_custom<<<numBlocks, 0>>>(xDevice, yDevice, zDevice);

        // Example Two：Call add kernel which is implemented by SIMD C++ Basic API
        // add_custom<blockLength><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);

        aclrtSynchronizeDevice();
        ...
    }
  ```

- **算子编译与运行**：

  CMake配置文件示例：
  ```
  cmake_minimum_required(VERSION 3.16)

  find_package(ASC REQUIRED)

  project(kernel_samples LANGUAGES ASC CXX)

  add_executable(c_api_add_example
      c_api_add.asc
  )

  # ======================================================================================
  # NPU 编译选项配置
  #
  # 说明：
  #   - 需根据实际部署的 NPU 硬件架构选择对应的 `npu-arch` 参数。
  # ======================================================================================
  target_compile_options(c_api_add_example PRIVATE
      $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
  )
  ```
  编译与执行示例：
  ```
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake ..;make -j;             # 编译工程
  ./c_api_add_example           # 运行样例
  ```
  > [!NOTE] 说明
  > - 编译选项`--npu-arch`用于指定NPU架构版本，`dav-`后面的数字为架构版本号，请替换为您实际使用的版本。各AI处理器型号与架构版本的对应关系请查阅[AI处理器型号和 \_\_NPU_ARCH\_\_ 的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#table65291052154114)。

此外，基于C/C++不同层级的编程接口和不同的矢量计算类型，Add算子有多种实现方式，具体可参考下表：

| API层级 | 矢量计算类型 | Add算子实例 | 说明 |
|--------------|-----------|--------------|--------------|
| SIMD C API | 基于指针的Memory矢量计算 | [Memory矢量计算Add算子示例（同上述C API实现样例）](https://gitcode.com/cann/asc-devkit/blob/master/examples/02_simd_c_api/00_introduction/01_add/c_api_async_add/README.md) | 贴合C语言开发习惯，易于上手 |
| SIMD C API | 基于指针和Reg计算的Reg矢量计算 | [Reg矢量计算Add算子示例](https://gitcode.com/cann/asc-devkit/blob/master/examples/02_simd_c_api/00_introduction/04_reg_base_add_compute/c_api_simd_add/README.md) | 贴合C语言开发习惯，性能上限更高 |
| 基础API | 基于Tensor的Memory矢量计算 | [Memory矢量计算Add算子示例（同上述C++ Tensor实现样例）](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/00_introduction/01_vector/add/README.md) | 匹配Tensor编程习惯，易于上手 |
| 基础API | 基于Tensor的Reg矢量计算 | [Reg矢量计算Add算子示例](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/00_introduction/04_vector_reg/add/README.md) | 匹配Tensor编程习惯，性能上限更高 |

> [!NOTE] 说明
> Ascend 950PR/Ascend 950DT新一代架构在传统[UB](../../../技术附录/概念原理和术语/术语表.md)缓存体系的基础上，开放了寄存器（Register）可编程能力，单个寄存器大小为256B。基于寄存器的矢量计算称为Reg矢量计算，而基于传统UB的矢量计算称为Memory矢量计算。

若要深入理解Ascend C的SIMD与SIMT编程模型，请参阅[Ascend C编程模型概述](../../../编程指南/编程模型/编程模型概述.md)。
