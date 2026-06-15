# RTC<a name="ZH-CN_TOPIC_0000002462620725"></a>

RTC是Ascend C运行时编译库，通过[aclrtc](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/context/RTC.md)接口，在程序运行时，将中间代码动态编译成目标机器码，提升程序运行性能。

运行时编译库提供以下核心接口：
-   aclrtcCreateProg：根据输入参数（字符串形式表达的Ascend C源代码等）创建aclrtcProg程序实例。
-   aclrtcAddNameExpr（可选）：注册需要导出的核函数名表达式，支持模板参数（如 "Kernel::add_custom\<float\>"），非模板核函数可跳过。
-   aclrtcCompileProg：编译给定的程序，支持用户自定义编译选项，比如指定NPU架构版本号：--npu-arch=dav-2201。支持的编译选项可以参考[《毕昇编译器用户指南》](https://www.hiascend.com/document/redirect/CannCommunityBiSheng)。
-   aclrtcGetBinDataSize：获取编译后的Device侧二进制数据的大小。
-   aclrtcGetBinData：获取编译后的Device侧二进制数据。
-   aclrtcGetLoweredName（可选）：获取核函数编译后的mangled name，用于后续通过aclrtBinaryGetFunction查找核函数句柄，非模板核函数可跳过。
-   aclrtcDestroyProg：在编译和执行过程结束后，销毁给定的程序。

编译完成后需要调用如下接口完成（仅列出核心接口）Kernel加载与执行。完整流程和详细接口说明请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)中的“Kernel加载与执行”章节。
1.  通过aclrtBinaryLoadFromData接口解析由aclrtcGetBinData接口获取的算子二进制数据。
2.  获取核函数句柄并根据核函数句柄操作其参数列表，相关接口包括aclrtBinaryGetFunction（获取核函数句柄）、aclrtKernelArgsInit（初始化参数列表）、aclrtKernelArgsAppend（追加拷贝用户设置的参数值如xDevice,  yDevice, zDevice）等。
3.  调用aclrtLaunchKernelWithConfig接口，启动对应算子的计算任务。

如下样例演示了如何使用aclrtc接口编译并运行一个核函数，该核函数中调用了printf进行打印。完整样例请参考[LINK](https://gitcode.com/cann/asc-devkit/tree/9.1.0-beta.3/examples/01_simd_cpp_api/02_features/06_aclrtc/rtc_hello_world/README.md)。


```c++
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "acl/acl_rt_compile.h" // 使用aclrtc接口需要包含的头文件

#ifndef ACL_RTC_NPU_ARCH
#define ACL_RTC_NPU_ARCH "dav-2201"
#endif

#define ASCENDC_CHECK(expr) do {               \
    aclError ret = (expr);                     \
    if (ret != ACL_SUCCESS) {                  \
        fprintf(stderr,                        \
            "Ascend Error: %s:%d code=%d %s\n",\
            __FILE__, __LINE__,                \
            ret, aclGetRecentErrMsg());        \
        return ret;                            \
    }                                          \
} while(0)

const char *src = R""""(
    #include "debug/asc_printf.h"
    extern "C" __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
)"""";

int main(int argc, char *argv[])
{
    aclrtcProg prog;
    ASCENDC_CHECK(aclrtcCreateProg(&prog, src, "hello_world.asc", 0, nullptr, nullptr));

    // aclrtc流程，传入毕昇编译器的编译选项，调用aclrtcCompileProg进行编译
    const char *options[] = {
        "--npu-arch=" ACL_RTC_NPU_ARCH,
    };
    int numOptions = sizeof(options) / sizeof(options[0]);
    aclError ret = aclrtcCompileProg(prog, 1, options);
    if (ret != ACL_SUCCESS) { // 编译报错时打印错误信息
        size_t size = 0;
        (void)aclrtcGetCompileLogSize(prog, &size);
        char log[size] = {0};
        (void)aclrtcGetCompileLog(prog, log);
        printf("Compile Error Log : %s", log);
    }
    // aclrtc流程，获取Device侧二进制内容和大小
    size_t binDataSizeRet;
    ASCENDC_CHECK(aclrtcGetBinDataSize(prog, &binDataSizeRet));
    std::vector<char> deviceELF(binDataSizeRet);
    ASCENDC_CHECK(aclrtcGetBinData(prog, deviceELF.data()));

    // ----------------------------------------------- aclrt part ------------------------------------------------
    aclrtBinHandle binHandle = nullptr;
    aclrtBinaryLoadOptions loadOption;
    loadOption.numOpt = 1;
    aclrtBinaryLoadOption option;
    option.type = ACL_RT_BINARY_LOAD_OPT_LAZY_MAGIC;
    option.value.magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    loadOption.options = &option;
    ASCENDC_CHECK(aclrtSetDevice(0));
    ASCENDC_CHECK(aclrtBinaryLoadFromData(deviceELF.data(), binDataSizeRet, &loadOption, &binHandle));


    aclrtFuncHandle funcHandle = nullptr;
    const char *funcName = "hello_world";
    ASCENDC_CHECK(aclrtBinaryGetFunction(binHandle, funcName, &funcHandle));

    aclrtArgsHandle argsHandle = nullptr;
    ASCENDC_CHECK(aclrtKernelArgsInit(funcHandle, &argsHandle));
    ASCENDC_CHECK(aclrtKernelArgsFinalize(argsHandle));
    // 核函数执行
    uint32_t numBlocks = 8;
    ASCENDC_CHECK(aclrtLaunchKernelWithConfig(funcHandle, numBlocks, nullptr, nullptr, argsHandle, nullptr));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ASCENDC_CHECK(aclrtBinaryUnLoad(binHandle));
    ASCENDC_CHECK(aclrtResetDevice(0));

    // 编译和运行均已结束，销毁程序
    ASCENDC_CHECK(aclrtcDestroyProg(&prog));
    return 0;
}
```

编译命令如下，编译时需要设置-I\$\{ASCEND_HOME_PATH\}/include，用于找到aclrtc相关头文件，并设置-L\$\{ASCEND_HOME_PATH\}/lib64链接alc_rtc动态库。

```
g++ rtc_hello_world.cpp -I${ASCEND_HOME_PATH}/include -L${ASCEND_HOME_PATH}/lib64 -lascendcl -lacl_rtc -o main
```

对于非模板核函数（如`hello_world`），编译器可自动导出符号，无需额外操作。  
当核函数为模板函数时，编译器无法自动确定需要导出的特化实例，需要通过`aclrtcAddNameExpr`手动注册需要导出的核函数名（含模板参数）；编译后通过`aclrtcGetLoweredName`获取mangled name，用于后续`aclrtBinaryGetFunction`查找句柄。

```c++
// 注册需要导出的核函数名（含模板参数）
const char* kernelNameExpr = "Kernel::add_custom<float>";
aclrtcAddNameExpr(prog, kernelNameExpr);
// ... 编译流程aclrtcCompileProg ...
// 获取编译后的mangled name
const char* manglingName = "";
aclrtcGetLoweredName(prog, kernelNameExpr, &manglingName);
```

完整样例请参考：[rtc_template_add](https://gitcode.com/cann/asc-devkit/tree/9.1.0-beta.3/examples/01_simd_cpp_api/02_features/06_aclrtc/rtc_template_add/README.md)。
