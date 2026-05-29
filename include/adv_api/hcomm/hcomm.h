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
 * \file hcomm.h
 * \brief Hcomm interface
 */
#ifndef INCLUDE_ADV_API_HCOMM_HCOMM_H
#define INCLUDE_ADV_API_HCOMM_HCOMM_H

#include "kernel_basic_intf.h"
#include "hcomm_common.h"
#include "../../../impl/adv_api/detail/hcomm/impl/hcomm_impl_def.h"

namespace AscendC {

/*!
 * @class Hcomm
 * @brief This class mainly provides a series of point-to-point communication primitive interfaces,
 *        benchmarking against Huawei's point-to-point communication C++ interface,
 *        including WriteNbi、ReadNbi and so on.
 *        The typical usage of this class is as follows:
 *          1) Create Hcomm object.
 *          2) Initialize communication channel.
 *          3) Launch comm tasks asynchronously through the corresponding interface,
 *             and the server starts assembling and launching comm tasks as soon as it listens.
 *          4) If commit is false when launching task, call the Commit interface to notify the execution of the
 * corresponding comm task. 5) Call the Wait interface (blocking) to wait for the server to complete the corresponding
 * comm task.
 * @tparam commProtocol: The communication protocol to use, ROCE supported as default.
 * @tparam commEngine: The communication engine to use, AIV supported as default.
 */
template <CommProtocol commProtocol = CommProtocol::ROCE, CommEngine commEngine = CommEngine::AIV>
class Hcomm {
public:
    /*!
     * @brief Initialize Hcomm workspace.
     * @param [in] buff: The UB buffer provided by caller.
     * @param [in] len: The buffer length in bytes.
     * @return 0 indicates success and -1 indicates failure.
     * @note URMA uses buff as its temporary workspace after 32-byte alignment.
     */
    __aicore__ inline int32_t Init(__ubuf__ uint8_t* buff, uint32_t len);

    /*!
     * @class Hcomm
     * @brief The task launching interface of the Write point-to-point communication operator, returns the identifier
     *        handleId. (task content: Write data of length len from src to dst through the specified channel.)
     * @tparam commit: true/false true: commit the task immediately; false: do not commit immediately.
     * @tparam commitPipe: The pipe type to use for commit, PIPE_MTE3 supported as default.
     * @tparam reqPipe: The pipe type to use for req, PIPE_MTE supported as default.
     * @tparam config: URMA WQE control config, only used by URMA. Default: strongly ordered + fence + CQE enabled.
     * @param [in] channelPtr: The handle of the communication channel.
     * @param [out] dst: The destination address of the data.
     * @param [in] src: The source address of the data.
     * @param [in] len: The length of the data to write, using byte as the basic unit.
     * @return The identifier(handleId) of Write task. HandleId >= 0 when successful, otherwise return -1.
     * @note Must be called after channel initialization.
     */
    template <bool commit = true, pipe_t commitPipe = PIPE_MTE3, pipe_t reqPipe = PIPE_MTE3,
        auto const &config = URMA_DEFAULT_CFG>
    __aicore__ inline HcommHandle WriteNbi(ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src, uint64_t len);

    /*!
     * @class Hcomm
     * @brief The task launching interface of the Write-with-notify point-to-point communication operator.
     * @tparam commit: true/false true: commit the task immediately; false: do not commit immediately.
     * @tparam commitPipe: The pipe type to use for commit, PIPE_MTE3 supported as default.
     * @tparam reqPipe: The pipe type to use for req, PIPE_MTE supported as default.
     * @tparam config: URMA WQE control config, only used by URMA. Default: strongly ordered + fence + CQE enabled.
     * @param [in] channelPtr: The handle of the communication channel.
     * @param [out] dst: The destination address of the data.
     * @param [in] src: The source address of the data.
     * @param [in] len: The length of the data to write, using byte as the basic unit.
     * @param [in] notifyAddr: The remote notify address.
     * @param [in] notifyVal: The remote notify value.
     * @return The identifier(handleId) of Write-with-notify task. HandleId >= 0 when successful, otherwise return -1.
     * @note Must be called after channel initialization.
     */
    template <bool commit = true, pipe_t commitPipe = PIPE_MTE3, pipe_t reqPipe = PIPE_MTE3,
        auto const &config = URMA_DEFAULT_CFG>
    __aicore__ inline HcommHandle WriteWithNotifyNbi(ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src,
        uint64_t len, GM_ADDR notifyAddr, uint64_t notifyVal);

    /*!
     * @class Hcomm
     * @brief The task launching interface of the Read point-to-point communication operator, returns the identifier
     *        handleId. (task content: Read data of length len from src to dst through the specified channel.)
     * @tparam commit: true/false true: commit the task immediately; false: do not commit immediately.
     * @tparam commitPipe: The pipe type to use for commit, PIPE_MTE3 supported as default.
     * @tparam reqPipe: The pipe type to use for req, PIPE_MTE supported as default.
     * @tparam config: URMA WQE control config, only used by URMA. Default: strongly ordered + fence + CQE enabled.
     * @param [in] channelPtr: The handle of the communication channel.
     * @param [out] dst: The destination address of the data.
     * @param [in] src: The source address of the data.
     * @param [in] len: The length of the data to read, using byte as the basic unit.
     * @return The identifier(handleId) of Read task. HandleId >= 0 when successful, otherwise return -1.
     * @note Must be called after channel initialization.
     */
    template <bool commit = true, pipe_t commitPipe = PIPE_MTE3, pipe_t reqPipe = PIPE_MTE3,
        auto const &config = URMA_DEFAULT_CFG>
    __aicore__ inline HcommHandle ReadNbi(ChannelPtr channelPtr, GM_ADDR dst, GM_ADDR src, uint64_t len);

    /*!
     * @class Hcomm
     * @brief Informed that the task corresponding to handleId can be executed.
     * @tparam pipe: The pipe type to use for commit, PIPE_MTE3 supported as default.
     * @param [in] handleId: The identifier(handleId) of WriteNbi / ReadNbi task.
     * @return 0 indicates success and -1 indicates failure.
     * @note Must be called after WriteNbi / ReadNbi task.
     */
    template <pipe_t pipe = PIPE_MTE3>
    __aicore__ inline int32_t Commit(HcommHandle handleId);
    
    /*!
     * @class Hcomm
     * @brief Block Aicore and wait for the comm task of handleId to finish processing once.
     * @tparam pipe: The pipe type to use for commit, PIPE_MTE3 supported as default.
     * @param [in] handleId: The identifier(handleId) of Write / Read task.
     * @return 0 indicates success and -1 indicates failure.
     * @note Must be called after Commit task.
     */
    template <pipe_t pipe = PIPE_MTE3>
    __aicore__ inline int32_t Wait(HcommHandle handleId);
private:
    HcommImpl<commProtocol, commEngine> impl_;
};
} // namespace AscendC

#include "../../../impl/adv_api/detail/hcomm/impl/hcomm_impl.h"

#endif // #endif  // LIB_HCCL_HCCL_H
