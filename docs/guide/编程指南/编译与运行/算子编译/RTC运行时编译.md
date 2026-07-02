# RTC<a name="ZH-CN_TOPIC_0000002462620725"></a>

RTC是Ascend C运行时编译库，通过[aclrtc](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_00162.html)接口，在程序运行时，将中间代码动态编译成目标机器码，提升程序运行性能。

运行时编译库提供以下核心接口：
-   aclrtcCreateProg：根据输入参数（字符串形式表达的Ascend C源代码等）创建aclrtcProg程序实例。
-   aclrtcCompileProg：编译给定的程序，支持用户自定义编译选项，比如指定NPU架构版本号：--npu-arch=dav-2201。支持的编译选项可以参考[《毕昇编译器用户指南》](https://www.hiascend.com/document/redirect/CannCommunityBiSheng)。
-   aclrtcGetBinDataSize：获取编译后的Device侧二进制数据的大小。
-   aclrtcGetBinData：获取编译后的Device侧二进制数据。
-   aclrtcDestroyProg：在编译和执行过程结束后，销毁给定的程序。

编译完成后需要调用如下接口完成（仅列出核心接口）Kernel加载与执行。完整流程和详细接口说明请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)中的“Kernel加载与执行”章节。
1.  通过aclrtBinaryLoadFromData接口解析由aclrtcGetBinData接口获取的算子二进制数据。
2.  获取核函数句柄并根据核函数句柄操作其参数列表，相关接口包括aclrtBinaryGetFunction（获取核函数句柄）、aclrtKernelArgsInit（初始化参数列表）、aclrtKernelArgsAppend（追加拷贝用户设置的参数值如xDevice,  yDevice, zDevice）等。
3.  调用aclrtLaunchKernelWithConfig接口，启动对应算子的计算任务。


如下是一个使用aclrtc接口编译并运行Add自定义算子的完整样例：


```c++
#include <vector>
#include "acl/acl.h"
#include "acl/acl_rt_compile.h" // 使用aclrtc接口需要包含的头文件

#define ASCENDC_CHECK(expr) do {               \
    aclError ret = (expr);                     \
    if (ret != ACL_SUCCESS) {                  \
        fprintf(stderr,                        \
            "Ascend Error: %s:%d code=%d %s\n",\
            __FILE__, __LINE__,                \
            ret, aclrtRecentErrMsg(ret));      \
    }                                          \
} while(0)

const char *src = R""""(
  #include "debug/asc_printf.h"
  extern "C" __global__ __aicore__ hello_world()
  {
      printf("hello world!!!\n")
  }
)"""";

int main(int argc, char *argv[])
{
    // aclrtc流程，src为用户Device侧源码，通过aclrtcCreateProg来创建编译程序
    aclrtcProg prog;
    ASCENDC_CHECK(aclrtcCreateProg(&prog, src, "hello_world.asc", 0, nullptr, nullptr));

    // aclrtc流程，传入毕昇编译器的编译选项，调用aclrtcCompileProg进行编译
    const char *options[] = {
        "--npu-arch=dav-2201",
    };
    int numOptions = sizeof(options) / sizeof(options[0]);
    ASCENDC_CHECK(aclrtcCompileProg(prog, numOptions, options));

    // aclrtc流程，获取Device侧二进制内容和大小
    size_t binDataSizeRet;
    ASCENDC_CHECK(aclrtcGetBinDataSize(prog, &binDataSizeRet));
    std::vector<char> deviceELF(binDataSizeRet);
    ASCENDC_CHECK(aclrtcGetBinData(prog, deviceELF.data()));

    // --------------------------------------------------- aclrt part --------------------------------------------------
    ASCENDC_CHECK(aclrtSetDevice(0));
    aclrtStream stream = nullptr;
    ASCENDC_CHECK(aclrtCreateStream(&stream));
    
    aclrtBinHandle binHandle = nullptr;
    aclrtBinaryLoadOptions loadOption;
    loadOption.numOpt = 1;
    aclrtBinaryLoadOption option;
    option.type = ACL_RT_BINARY_LOAD_OPT_LAZY_MAGIC;
    option.value.magic = ACL_RT_BINARY_MAGIC_ELF_VECTOR_CORE;   // 设置magic值，表示算子在Vector Core上执行
    loadOption.options = &option;
    ASCENDC_CHECK(aclrtBinaryLoadFromData(deviceELF.data(), binDataSizeRet, &loadOption, &binHandle));
  
    aclrtFuncHandle funcHandle = nullptr;
    const char *funcName = "add_custom";
    ASCENDC_CHECK(aclrtBinaryGetFunction(binHandle, funcName, &funcHandle));

    // 核函数入口
    uint32_t numBlocks = 8;
    ASCENDC_CHECK(aclrtLaunchKernelWithConfig(funcHandle, numBlocks, stream, nullptr, argsHandle, nullptr));
    ASCENDC_CHECK(aclrtSynchronizeStream(stream));
    ASCENDC_CHECK(aclrtBinaryUnLoad(binHandle));
    ASCENDC_CHECK(aclrtDestroyStream(stream));
    ASCENDC_CHECK(aclrtResetDevice(deviceId));
    ASCENDC_CHECK(aclFinalize());

    // 编译和运行均已结束，销毁程序
    ASCENDC_CHECK(aclrtcDestroyProg(&prog));
    return 0;
}
```

编译命令如下，编译时需要设置-I\$\{ASCEND_HOME_PATH\}/include，用于找到aclrtc相关头文件，并设置-L\$\{ASCEND_HOME_PATH\}/lib64链接alc_rtc动态库。

```
g++ add_custom.cpp -I${ASCEND_HOME_PATH}/include -L${ASCEND_HOME_PATH}/lib64 -lascendcl -lacl_rtc -o main
```
