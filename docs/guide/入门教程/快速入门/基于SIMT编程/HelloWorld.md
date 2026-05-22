# HelloWorld<a name="ZH-CN_TOPIC_0000002523206878"></a>

本示例展示了如何使用Ascend C SIMT编写一个简单的"Hello World"程序，包括核函数（设备侧实现的入口函数）的实现、调用流程以及编译运行的完整步骤。通过本示例，您可以快速了解Ascend C SIMT编程的基本开发流程。完整样例请参考[hello\_world](https://gitcode.com/cann/asc-devkit/tree/9.1.0-beta.1/examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt/README.md)。

代码文件hello\_world.asc包括核函数实现和主函数实现。

-   核函数实现：该核函数的核心逻辑是输出"Hello World!!!"字符串。
-   主函数实现：在主函数中，进行初始化环境、申请资源、通过<<<\>\>\>调用核函数以及释放资源等操作。完整的代码流程和逻辑可以通过代码注释查看。

```cpp
// Host侧应用程序需要包含的头文件
#include "acl/acl.h"
// Kernel侧需要包含的头文件
#include "asc_simt.h"
#include "asc_printf.h"
__global__ void  hello_world()
{
    if (threadIdx.x < 3) {
        printf("[blockIdx (%lu/%lu)][threadIdx (%lu/%lu)]: Hello World!\n", blockIdx.x, gridDim.x, threadIdx.x, blockDim.x);
    }
}
int32_t main(int argc, char const *argv[])
{
    // 初始化
    aclInit(nullptr);
    // 运行管理资源申请
    int32_t deviceId = 0;
    aclrtSetDevice(deviceId);
    aclrtStream stream = nullptr;
    aclrtCreateStream(&stream);
    // 设置参与计算的线程块、线程个数（可根据实际需求设置）
    constexpr uint32_t block_num = 2;
    constexpr uint32_t thread_num = 32;
   // 设置需要分配的动态内存大小，本样例不需要动态内存
    constexpr uint32_t dyn_ubuf_size = 0;
    hello_world<<<block_num, thread_num, dyn_ubuf_size, stream>>>();
    aclrtSynchronizeStream(stream);
    // 资源释放和去初始化
    aclrtDestroyStream(stream);
    aclrtResetDevice(deviceId);
    aclFinalize();
    return 0;
}
```

完成代码实现后，可以通过两种方式，对上述代码进行编译：

>[!NOTE]说明 
>- 该样例仅支持如下型号：
>    - Atlas 350 加速卡
>- 编译命令中的--npu-arch用于指定NPU的架构版本，dav-后为架构版本号，请替换为您实际使用的架构版本号。各AI处理器型号对应的架构版本号请通过[AI处理器型号和\_\_NPU\_ARCH\_\_的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#table65291052154114)进行查询。
>-   编译命令中的--enable-simt用于指定SIMT编程场景。


- **使用bisheng命令行进行编译**

    ```
    bisheng hello_world.asc --npu-arch=dav-3510 -o demo --enable-simt 
    ./demo
    ```

-   **使用CMake进行编译**

    CMake编译配置如下：

    ```
    cmake_minimum_required(VERSION 3.16)
    # find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
    find_package(ASC REQUIRED)
    # 指定项目支持的语言包括ASC和CXX，ASC表示支持使用毕昇编译器对Ascend C编程语言进行编译
    project(kernel_samples LANGUAGES ASC CXX)
    add_executable(demo
        hello_world.asc
    )
    # 通过编译选项设置NPU架构
    target_compile_options(demo PRIVATE   
       $<$<COMPILE_LANGUAGE:ASC>:dav-3510 --enable-simt>
    )
    ```

    编译和运行步骤如下：

    ```
    mkdir -p build && cd build; 
    cmake ..;make -j;
    ./demo
    ```

运行结果如下，本样例共调度8个核，打印了核号和"Hello World!!!"等信息。

```
[blockIdx (0/2)][threadIdx (2/32)]: Hello World!
[blockIdx (0/2)][threadIdx (1/32)]: Hello World!
[blockIdx (0/2)][threadIdx (0/32)]: Hello World!
[blockIdx (1/2)][threadIdx (2/32)]: Hello World!
[blockIdx (1/2)][threadIdx (1/32)]: Hello World!
[blockIdx (1/2)][threadIdx (0/32)]: Hello World!
```
