/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hcomm_impl.h
 * \brief Hcomm implementation
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/impl/hcomm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_IMPL_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_IMPL_HCOMM_IMPL_H
#define IMPL_ADV_API_DETAIL_HCOMM_IMPL_HCOMM_IMPL_H

#if __NPU_ARCH__ == 2201
#include "platform_v220/hcomm_aiv.h"
#elif __NPU_ARCH__ == 3510
#include "platform_v310/hcomm_aiv.h"
#include "platform_v310/hcomm_aiv_urma.h"
#endif

namespace AscendC {

template <CommProtocol commProtocol, CommEngine commEngine>
__aicore__ inline int32_t Hcomm<commProtocol, commEngine>::Init(__ubuf__ uint8_t* buff, uint32_t len)
{
    return impl_.Init(buff, len);
}

template <CommProtocol commProtocol, CommEngine commEngine>
template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const &config>
__aicore__ inline HcommHandle Hcomm<commProtocol, commEngine>::WriteNbi(
    ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    return impl_.template WriteNbi<commit, commitPipe, reqPipe, config>(channelPtr, dst, src, len);
}

template <CommProtocol commProtocol, CommEngine commEngine>
template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const &config>
__aicore__ inline HcommHandle Hcomm<commProtocol, commEngine>::WriteWithNotifyNbi(
    ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src, uint64_t len, GM_ADDR notifyAddr, uint64_t notifyVal)
{
    return impl_.template WriteWithNotifyNbi<commit, commitPipe, reqPipe, config>(
        channelPtr, dst, src, len, notifyAddr, notifyVal);
}

template <CommProtocol commProtocol, CommEngine commEngine>
template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const &config>
__aicore__ inline HcommHandle Hcomm<commProtocol, commEngine>::ReadNbi(
    ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    return impl_.template ReadNbi<commit, commitPipe, reqPipe, config>(channelPtr, dst, src, len);
}

template <CommProtocol commProtocol, CommEngine commEngine>
template <pipe_t pipe>
__aicore__ inline int32_t Hcomm<commProtocol, commEngine>::Commit(HcommHandle handleId)
{
    return impl_.template Commit<pipe>(handleId);
}

template <CommProtocol commProtocol, CommEngine commEngine>
template <pipe_t pipe>
__aicore__ inline int32_t Hcomm<commProtocol, commEngine>::Wait(HcommHandle handleId)
{
    return impl_.template Wait<pipe>(handleId);
}
} // namespace AscendC

#endif // IMPL_HCOMM_HCOMM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_IMPL_H__
#endif
