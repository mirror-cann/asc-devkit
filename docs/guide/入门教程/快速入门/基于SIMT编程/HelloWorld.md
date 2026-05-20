# Hello World <a name="ZH-CN_TOPIC_0000002523206878"></a>

本入门示例基于Ascend C SIMT实现Hello World算子，带你快速上手实践，涵盖Device端核函数实现、Host端调用以及编译运行的完整流程，帮助开发者建立整体认知。

开始前请参考[环境准备](../../环境准备.md)安装所需的CANN软件包，完整样例请参考[hello\_world](https://gitcode.com/cann/asc-devkit/tree/master/examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt/README.md)。

 - **Hello World功能介绍**：
 
    在NPU上打印`Hello World!`。

 - **Device 端代码实现**：

    后缀名为`*.asc`的代码文件包含Host端与Device端代码，其Device端部分示例如下：
    ```cpp
    __global__ void hello_world()
    {
        if (threadIdx.x < 3) {
            printf("[blockIdx (%lu/%lu)][threadIdx (%lu/%lu)]: Hello World!\n", blockIdx.x, gridDim.x, threadIdx.x, blockDim.x);
        }
    }
    ```

 - **Host 端代码实现**：

    Host端通过<<<>>>语法糖调用Device端代码。
    ```cpp
    int32_t main(int argc, char const *argv[])
    {
        ...
        // 4. Configure kernel launch parameters
        constexpr uint32_t block_num     = 2;   // Number of thread blocks (Grid size)
        constexpr uint32_t thread_num    = 32;  // Number of threads per block (Block size)
        constexpr uint32_t dyn_ubuf_size = 0;   // No dynamic memory required in this sample

        // 5. Launch kernel <<<Grid, Block, Dynamic memory, Stream>>>
        hello_world<<<block_num, thread_num, dyn_ubuf_size, stream>>>();
        // Wait for kernel execution to complete
        ...
    }
    ```

 - **算子编译与运行**：
    ```bash
    bisheng hello_world.asc --npu-arch=dav-3510 -o demo --enable-simt 
    ./demo
    ```
    > [!NOTE] 说明
    > - 该样例支持以下型号：
    >     - Ascend 950PR / Ascend 950DT
    > - 编译选项 `--npu-arch` 用于指定NPU架构版本，`dav-` 后为架构版本号，请替换为您实际使用的版本。各 AI 处理器型号对应的架构版本号请通过[AI 处理器型号和 \_\_NPU\_ARCH\_\_ 的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#table65291052154114) 查询。
    > - 编译选项`--enable-simt` 用于启用SIMT编程场景。
    
    运行结果如下，本样例共调度8个核，打印了核号和"Hello World!!!"等信息。

如需进一步了解Ascend C的SIMD与SIMT编程模型，请参阅[Ascend C 编程模型概述](../../../编程指南/编程模型/编程模型概述.md)。
