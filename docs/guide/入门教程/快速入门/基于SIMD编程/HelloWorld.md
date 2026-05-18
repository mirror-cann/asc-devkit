# HelloWorld<a name="ZH-CN_TOPIC_0000002500621202"></a>

本示例展示了如何使用Ascend C编写一个简单的"Hello World"程序，包括核函数（设备侧实现的入口函数）的实现、调用流程以及编译运行的完整步骤。通过本示例，您可以快速了解Ascend C的基本开发流程。完整样例请参考[LINK](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/00_introduction/00_quickstart/hello_world)。

代码文件 `hello_world.asc` 包括核函数实现和主函数实现。

-   核函数实现：该核函数的核心逻辑是输出"Hello World!!!"字符串。
-   主函数实现：在主函数中，完成环境初始化、申请资源、通过 `<<<>>>` 调用核函数以及释放资源等操作。完整的代码流程和逻辑可以通过代码注释查看。

```cpp
  // Host侧应用程序需要包含的头文件
  #include "acl/acl.h"
  // Kernel侧 printf需要包含的头文件
  #include "debug/asc_printf.h"

  __global__ __vector__ void hello_world()
  {
    printf("Hello World!!!\n");
  }

  int main(int argc, char const* argv[])
  {
    aclrtSetDevice(0); // 运行管理资源申请。
    // 1. 使用内核调用符<<<numBlock, dynUBufSize, stream>>>调用核函数。
    // 2. numBlock：8表示参与计算的核数为8（核数可根据实际需求设置）。
    // 3. dynUBufSize: 0表示不使用Unified Buffer的动态内存。
    // 4. stream: nullptr表示使用默认stream。
    hello_world<<<8, 0, nullptr>>>();
    aclrtSynchronizeDevice(); // 等待核函数执行完成
    return 0;
  }
```

完成代码实现后，可以通过以下两种方式对上述代码进行编译：

>[!NOTE] 说明
>- 该样例仅支持如下型号：
>    - Ascend 950PR/Ascend 950DT
>    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
>    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
>- 编译命令中的--npu-arch用于指定NPU的架构版本，dav-后为架构版本号，请替换为您实际使用的架构版本号（bisheng命令行编译）或通过CMAKE_ASC_ARCHITECTURES传入（CMake编译）。各AI处理器型号对应的架构版本号请通过[AI处理器型号和\_\_NPU\_ARCH\_\_的对应关系](../../../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#table65291052154114)进行查询。

-   **使用bisheng命令行进行编译**

    ```
    bisheng hello_world.asc --npu-arch=dav-2201 -o demo
    ./demo
    ```

-   **使用CMake进行编译**

    CMake编译配置如下：

    ```
    cmake_minimum_required(VERSION 3.16)
    # CMAKE_ASC_ARCHITECTURES是用来决定NPU架构的编译参数，可选值为：dav-2201, dav-3510
    set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510")
    # find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
    find_package(ASC REQUIRED)
    # 指定项目支持的语言包括ASC，ASC表示支持使用毕昇编译器对Ascend C编程语言进行编译
    project(kernel_samples LANGUAGES ASC)
    add_executable(demo
        hello_world.asc
    )
    # 通过编译选项设置NPU架构
    target_compile_options(demo PRIVATE
        --npu-arch=${CMAKE_ASC_ARCHITECTURES}
    )
    ```

    编译和运行步骤如下：

    ```
    mkdir -p build && cd build;
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
    ./demo
    ```

运行结果如下。本样例共调度8个核，打印了核号和"Hello World!!!"等信息。

```
  [AIV Block 0/8] Hello World!!!
  [AIV Block 1/8] Hello World!!!
  [AIV Block 2/8] Hello World!!!
  [AIV Block 3/8] Hello World!!!
  [AIV Block 4/8] Hello World!!!
  [AIV Block 5/8] Hello World!!!
  [AIV Block 6/8] Hello World!!!
  [AIV Block 7/8] Hello World!!!
```
