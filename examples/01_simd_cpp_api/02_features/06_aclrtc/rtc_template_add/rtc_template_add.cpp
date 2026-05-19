/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "acl/acl.h"
#include "acl/acl_rt.h"
#include "acl/acl_rt_compile.h"
#include "data_utils.h"

int main(int argc, char *argv[])
{
    const char *src = R""""(
#include "kernel_operator.h"
constexpr int32_t TOTAL_LENGTH = 1024;

class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z)
    {
        xGm.SetGlobalBuffer((__gm__ float *)x, TOTAL_LENGTH);
        yGm.SetGlobalBuffer((__gm__ float *)y, TOTAL_LENGTH);
        zGm.SetGlobalBuffer((__gm__ float *)z, TOTAL_LENGTH);
    }
    __aicore__ inline void Process()
    {
        AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
        AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, TOTAL_LENGTH>();
        AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, TOTAL_LENGTH>();
        AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, TOTAL_LENGTH>();

        AscendC::DataCopy(xLocal, xGm, TOTAL_LENGTH);
        AscendC::DataCopy(yLocal, yGm, TOTAL_LENGTH);
        // MTE2 → V 同步：确保 DataCopy 完成后再计算
        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        AscendC::Add(zLocal, xLocal, yLocal, TOTAL_LENGTH);
        // V → MTE3 同步：确保 Add 完成后再将结果拷出
        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

        AscendC::DataCopy(zGm, zLocal, TOTAL_LENGTH);
    }

private:
    AscendC::GlobalTensor<float> xGm;
    AscendC::GlobalTensor<float> yGm;
    AscendC::GlobalTensor<float> zGm;
};

namespace Kernel {
template<typename T>
__vector__ __global__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    AscendC::InitSocState();
    KernelAdd op;
    op.Init(x, y, z);
    op.Process();
}
}
)"""";

    aclrtcProg prog;
    CHECK_ACL(aclrtcCreateProg(&prog, src, "add_custom", 0, nullptr, nullptr));

    const char* kernelNameExpr = "Kernel::add_custom<float>";
    CHECK_ACL(aclrtcAddNameExpr(prog, kernelNameExpr));

#ifndef ACLRTC_NPU_ARCH
#define ACLRTC_NPU_ARCH "dav-2201"
#endif
    const char *options[] = {
        "--npu-arch=" ACLRTC_NPU_ARCH,
    };

    int numOptions = sizeof(options) / sizeof(options[0]);
    CHECK_ACL(aclrtcCompileProg(prog, numOptions, options));

    size_t binDataSizeRet;
    CHECK_ACL(aclrtcGetBinDataSize(prog, &binDataSizeRet));

    std::vector<char> deviceELF(binDataSizeRet);
    CHECK_ACL(aclrtcGetBinData(prog, deviceELF.data()));

    const char* manglingName = "";
    CHECK_ACL(aclrtcGetLoweredName(prog, kernelNameExpr, &manglingName));

    uint32_t numBlocks = 1;
    uint32_t dataLen = 1024; // TOTAL_LENGTH
    size_t inputByteSize = dataLen * sizeof(float);
    size_t outputByteSize = dataLen * sizeof(float);

    CHECK_ACL(aclInit(nullptr));
    int32_t deviceId = 0;
    CHECK_ACL(aclrtSetDevice(deviceId));
    aclrtStream stream = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));

    float *xHost, *yHost, *zHost, *golden;
    uint8_t *xDevice, *yDevice, *zDevice;

    CHECK_ACL(aclrtMallocHost((void **)(&xHost), inputByteSize));
    CHECK_ACL(aclrtMallocHost((void **)(&yHost), inputByteSize));
    CHECK_ACL(aclrtMallocHost((void **)(&zHost), outputByteSize));
    CHECK_ACL(aclrtMallocHost((void **)(&golden), outputByteSize));
    CHECK_ACL(aclrtMalloc((void **)&xDevice, inputByteSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMalloc((void **)&yDevice, inputByteSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMalloc((void **)&zDevice, outputByteSize, ACL_MEM_MALLOC_HUGE_FIRST));

    GenTestData(xHost, yHost, golden, dataLen);

    CHECK_ACL(aclrtMemcpy(xDevice, inputByteSize, xHost, inputByteSize, ACL_MEMCPY_HOST_TO_DEVICE));
    CHECK_ACL(aclrtMemcpy(yDevice, inputByteSize, yHost, inputByteSize, ACL_MEMCPY_HOST_TO_DEVICE));

    aclrtBinHandle binHandle = nullptr;
    aclrtBinaryLoadOptions loadOption;
    loadOption.numOpt = 1;
    aclrtBinaryLoadOption option;
    option.type = ACL_RT_BINARY_LOAD_OPT_MAGIC;
    option.value.magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    loadOption.options = &option;
    CHECK_ACL(aclrtBinaryLoadFromData(deviceELF.data(), binDataSizeRet, &loadOption, &binHandle));
    aclrtFuncHandle funcHandle = nullptr;

    CHECK_ACL(aclrtBinaryGetFunction(binHandle, manglingName, &funcHandle));

    aclrtArgsHandle argsHandle = nullptr;
    aclrtParamHandle paramHandle = nullptr;
    CHECK_ACL(aclrtKernelArgsInit(funcHandle, &argsHandle));
    CHECK_ACL(aclrtKernelArgsAppend(argsHandle, (void **)&xDevice, sizeof(uintptr_t), &paramHandle));
    CHECK_ACL(aclrtKernelArgsAppend(argsHandle, (void **)&yDevice, sizeof(uintptr_t), &paramHandle));
    CHECK_ACL(aclrtKernelArgsAppend(argsHandle, (void **)&zDevice, sizeof(uintptr_t), &paramHandle));
    CHECK_ACL(aclrtKernelArgsFinalize(argsHandle));
    CHECK_ACL(aclrtLaunchKernelWithConfig(funcHandle, numBlocks, stream, nullptr, argsHandle, nullptr));

    CHECK_ACL(aclrtSynchronizeStream(stream));
    CHECK_ACL(aclrtMemcpy(zHost, outputByteSize, zDevice, outputByteSize, ACL_MEMCPY_DEVICE_TO_HOST));

    // 精度校验（C++ 内完成，无需 Python 脚本）
    if (VerifyResult(zHost, golden, dataLen)) {
        printf("test pass!\n");
    } else {
        printf("[ERROR] result error\n");
        return 1;
    }

    CHECK_ACL(aclrtBinaryUnLoad(binHandle));
    CHECK_ACL(aclrtFree(xDevice));
    CHECK_ACL(aclrtFree(yDevice));
    CHECK_ACL(aclrtFree(zDevice));
    CHECK_ACL(aclrtFreeHost(golden));
    CHECK_ACL(aclrtFreeHost(xHost));
    CHECK_ACL(aclrtFreeHost(yHost));
    CHECK_ACL(aclrtFreeHost(zHost));
    CHECK_ACL(aclrtDestroyStream(stream));
    CHECK_ACL(aclrtResetDevice(deviceId));
    CHECK_ACL(aclFinalize());

    CHECK_ACL(aclrtcDestroyProg(&prog));

    return 0;
}
