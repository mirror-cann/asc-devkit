/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hcomm_util.h
 * \brief Hcomm utils
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/common/hcomm_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_UTILS_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_UTIL_H
#define IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_UTIL_H

#include "hcomm_inner_def.h"

namespace AscendC {

__aicore__ inline __ubuf__ uint8_t* AlignAddrTo32Bytes(__ubuf__ uint8_t* buff)
{
    uintptr_t addr = reinterpret_cast<uintptr_t>(buff);
    const uintptr_t alignment = 32;
    uintptr_t alignedAddr = (addr + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<__ubuf__ uint8_t*>(alignedAddr);
}

template <typename T>
__aicore__ inline void Gm2Ub(
    const AscendC::LocalTensor<T>& dstLocal, const AscendC::GlobalTensor<T>& srcGlobal, int32_t length)
{
    AscendC::DataCopyExtParams copyParams{1, (uint32_t)(length * sizeof(T)), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, (ONE_BLK_SIZE - sizeof(T)) / sizeof(T), 0};

    PipeBarrier<PIPE_ALL>();
    AscendC::DataCopyPad(dstLocal, srcGlobal, copyParams, padParams);
    PipeBarrier<PIPE_ALL>();
}

template <typename T>
__aicore__ inline void Ub2Gm(
    const AscendC::GlobalTensor<T>& dstGlobal, const AscendC::LocalTensor<T>& srcLocal, int32_t length)
{
    AscendC::DataCopyExtParams copyParams{1, (uint32_t)(length * sizeof(T)), 0, 0, 0};
    PipeBarrier<PIPE_ALL>();
    AscendC::DataCopyPad(dstGlobal, srcLocal, copyParams);
    PipeBarrier<PIPE_ALL>();
}

__aicore__ inline int32_t HcommFindBufferIdx(
    RegedBufferEntity* bufferAddr, uint32_t bufferNum, GM_ADDR addr, uint64_t len)
{
    if (bufferAddr == nullptr) {
        KERNEL_LOG(KERNEL_ERROR, "HcommFindBufferIdx failed with null bufferAddr, bufferNum=%u", bufferNum);
        return HCOMM_INVALID_HANDLE_ID;
    }
    uint64_t targetAddr = reinterpret_cast<uint64_t>(addr);
    for (uint32_t i = 0; i < bufferNum; i++) {
        uint64_t baseAddr = bufferAddr[i].bufferInfo.rma.addr;
        uint64_t bufferSize = bufferAddr[i].bufferInfo.rma.size;
        if (targetAddr < baseAddr) {
            continue;
        }
        uint64_t offset = targetAddr - baseAddr;
        if (offset <= bufferSize && len <= bufferSize - offset) {
            KERNEL_LOG(KERNEL_INFO, "HcommFindBufferIdx hit idx=%u addr=%llu len=%llu base=%llu size=%llu ", i,
                static_cast<unsigned long long>(targetAddr), static_cast<unsigned long long>(len),
                static_cast<unsigned long long>(baseAddr), static_cast<unsigned long long>(bufferSize));
            return static_cast<int32_t>(i);
        }
    }
    KERNEL_LOG(KERNEL_ERROR, "HcommFindBufferIdx failed addr=%llu len=%llu bufferNum=%u",
        static_cast<unsigned long long>(targetAddr), static_cast<unsigned long long>(len), bufferNum);
    return HCOMM_INVALID_HANDLE_ID;
}

__aicore__ inline void CacheWriteThrough(__gm__ uint8_t* sourceAddr, uint64_t length)
{
    __gm__ uint8_t* start = (__gm__ uint8_t*)((uint64_t)sourceAddr / CACHE_LINE_SIZE * CACHE_LINE_SIZE);
    __gm__ uint8_t* end = (__gm__ uint8_t*)(((uint64_t)sourceAddr + length) / CACHE_LINE_SIZE * CACHE_LINE_SIZE);
    GlobalTensor<uint8_t> global;
    global.SetGlobalBuffer(start);
    for (uint32_t i = 0; i < end - start; i += CACHE_LINE_SIZE) {
        DataCacheCleanAndInvalid<uint8_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(global[i]);
    }
}
} // namespace AscendC

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_UTILS_H__
#endif
