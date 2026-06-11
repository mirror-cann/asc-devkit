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
    if (ret != ACL_SUCCESS) {
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
