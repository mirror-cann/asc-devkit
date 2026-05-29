/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <array>
#include <gtest/gtest.h>
#include <vector>
#define private public
#include "kernel_operator.h"

using namespace AscendC;

namespace {

constexpr uint32_t URMA_SQ_DEPTH = 10;
constexpr uint32_t URMA_WQE_SIZE = 64;
constexpr uint32_t URMA_CQE_SIZE = 64;
constexpr uint32_t URMA_BUFFER_NUM = 2;

class UrmaChannelResource {
public:
    UrmaChannelResource()
        : sqBuffer_(URMA_SQ_DEPTH * URMA_WQE_SIZE, 0),
          cqBuffer_(URMA_SQ_DEPTH * URMA_CQE_SIZE, 0)
    {
        channel_.engine = 4;
        channel_.sqNum = 1;
        channel_.cqNum = 1;
        channel_.remoteBufferNum = URMA_BUFFER_NUM;
        channel_.localBufferNum = URMA_BUFFER_NUM;
        channel_.sqContextAddr = &sqCtx_;
        channel_.cqContextAddr = &cqCtx_;
        channel_.remoteBufferAddr = remoteBuffers_.data();
        channel_.localBufferAddr = localBuffers_.data();

        sqCtx_.type = AscendC::SQ_CONTEXT_TYPE_UB_JFS;
        sqCtx_.contextInfo.ubJfs.sqVa = reinterpret_cast<uint64_t>(sqBuffer_.data());
        sqCtx_.contextInfo.ubJfs.headAddr = reinterpret_cast<uint64_t>(&sqHead_);
        sqCtx_.contextInfo.ubJfs.tailAddr = reinterpret_cast<uint64_t>(&sqTail_);
        sqCtx_.contextInfo.ubJfs.dbVa = reinterpret_cast<uint64_t>(&sqDoorbell_);
        sqCtx_.contextInfo.ubJfs.jfsID = 1;
        sqCtx_.contextInfo.ubJfs.wqeSize = URMA_WQE_SIZE;
        sqCtx_.contextInfo.ubJfs.sqDepth = URMA_SQ_DEPTH;
        sqCtx_.contextInfo.ubJfs.tpID = 1;

        cqCtx_.type = AscendC::CQ_CONTEXT_TYPE_UB_JFC;
        cqCtx_.contextInfo.ubJfc.scqVa = reinterpret_cast<uint64_t>(cqBuffer_.data());
        cqCtx_.contextInfo.ubJfc.headAddr = reinterpret_cast<uint64_t>(&cqHead_);
        cqCtx_.contextInfo.ubJfc.tailAddr = reinterpret_cast<uint64_t>(&cqTail_);
        cqCtx_.contextInfo.ubJfc.dbVa = reinterpret_cast<uint64_t>(&cqDoorbell_);
        cqCtx_.contextInfo.ubJfc.jfcID = 1;
        cqCtx_.contextInfo.ubJfc.cqeSize = URMA_CQE_SIZE;
        cqCtx_.contextInfo.ubJfc.cqDepth = URMA_SQ_DEPTH;

        InitBuffer(remoteBuffers_[0], 0x1000, 0x1000, 0x123456, 0x654321);
        InitBuffer(remoteBuffers_[1], 0x3000, 0x1000, 0x223456, 0x754321);
        InitBuffer(localBuffers_[0], 0x2000, 0x1000, 0x111111, 0x222222);
        InitBuffer(localBuffers_[1], 0x5000, 0x1000, 0x333333, 0x444444);
    }

    AscendC::ChannelPtr GetHandle()
    {
        return reinterpret_cast<AscendC::ChannelPtr>(&channel_);
    }

    void CompleteCurrentSq()
    {
        cqTail_ = sqHead_;
        sqTail_ = sqHead_;
    }

private:
    void InitBuffer(AscendC::RegedBufferEntity& buffer, uint64_t addr, uint64_t size, uint32_t tokenId,
        uint32_t tokenValue)
    {
        buffer.type = AscendC::REGED_BUFFER_RMA;
        buffer.bufferInfo.rma.addr = addr;
        buffer.bufferInfo.rma.size = size;
        buffer.bufferInfo.rma.protectionInfo.type = AscendC::PROTECTION_TYPE_UB;
        buffer.bufferInfo.rma.protectionInfo.memInfo.ub.tokenId = tokenId;
        buffer.bufferInfo.rma.protectionInfo.memInfo.ub.tokenValue = tokenValue;
    }

private:
    AscendC::ChannelEntity channel_ = {};
    AscendC::SqContext sqCtx_ = {};
    AscendC::CqContext cqCtx_ = {};
    std::array<AscendC::RegedBufferEntity, URMA_BUFFER_NUM> remoteBuffers_ = {};
    std::array<AscendC::RegedBufferEntity, URMA_BUFFER_NUM> localBuffers_ = {};
    std::vector<uint8_t> sqBuffer_;
    std::vector<uint8_t> cqBuffer_;
    uint32_t sqHead_ = 0;
    uint32_t sqTail_ = 0;
    uint32_t cqHead_ = 0;
    uint32_t cqTail_ = 0;
    uint32_t sqDoorbell_ = 0;
    uint32_t cqDoorbell_ = 0;
};

} // namespace

class HcommUrmaTestSuite : public testing::Test {
protected:
    void SetUp() override
    {
        blockIdxBak_ = block_idx;
    }

    void TearDown() override
    {
        block_idx = blockIdxBak_;
    }

    int32_t InitHcomm(AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV>& hcomm)
    {
        pipe_.InitBuffer(hcommBuf_, AscendC::HCOMM_URMA_TMP_BUF_SIZE);
        AscendC::LocalTensor<uint8_t> hcommLocal = hcommBuf_.Get<uint8_t>();
        __ubuf__ uint8_t* bufPtr = reinterpret_cast<__ubuf__ uint8_t*>(hcommLocal.GetPhyAddr());
        for (uint32_t i = 0; i < AscendC::HCOMM_URMA_TMP_BUF_SIZE; i++) {
            bufPtr[i] = 0;
        }
        return hcomm.Init(bufPtr, AscendC::HCOMM_URMA_TMP_BUF_SIZE);
    }

    // Read sqPI value from hcomm UB buffer using the same offset layout as Init:
    // wqeItem_: HCOMM_URMA_WQE_U32_NUM uint32_t's
    // cqeItem_: HCOMM_URMA_CQE_U32_NUM uint32_t's
    // sqPI_: starts at (WQE_U32_NUM + CQE_U32_NUM) uint32_t offset
    uint32_t GetSqPIFromBuf()
    {
        AscendC::LocalTensor<uint32_t> hcommLocal = hcommBuf_.Get<uint32_t>();
        constexpr uint32_t sqPIIdx =
            AscendC::HCOMM_URMA_WQE_U32_NUM + AscendC::HCOMM_URMA_CQE_U32_NUM;
        return hcommLocal.GetValue(sqPIIdx);
    }

private:
    AscendC::TPipe pipe_;
    AscendC::TBuf<AscendC::TPosition::VECOUT> hcommBuf_;
    int64_t blockIdxBak_;
};

// ReadNbi with default commit=true: auto-commit internally, then Wait succeeds
TEST_F(HcommUrmaTestSuite, Aiv_Urma_Read)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);
    AscendC::HcommHandle handleId = hcomm.ReadNbi(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x5008), reinterpret_cast<GM_ADDR>(0x3008), 8);
    EXPECT_EQ(handleId, 0);
    channel.CompleteCurrentSq();
    int32_t ret = hcomm.Wait(handleId);
    EXPECT_EQ(ret, 0);
}

// WriteNbi with commit=false: explicit Commit then Wait succeeds
TEST_F(HcommUrmaTestSuite, Aiv_Urma_Write)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);
    AscendC::HcommHandle handleId = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x3008), reinterpret_cast<GM_ADDR>(0x5008), 8);
    EXPECT_EQ(handleId, 0);
    int32_t ret = hcomm.Commit(handleId);
    EXPECT_EQ(ret, 0);
    channel.CompleteCurrentSq();
    ret = hcomm.Wait(handleId);
    EXPECT_EQ(ret, 0);
}

// WriteWithNotifyNbi with commit=false: explicit Commit then Wait succeeds
TEST_F(HcommUrmaTestSuite, Aiv_Urma_WriteWithNotify)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);
    AscendC::HcommHandle handleId = hcomm.WriteWithNotifyNbi<false>(channel.GetHandle(),
        reinterpret_cast<GM_ADDR>(0x3008), reinterpret_cast<GM_ADDR>(0x5008), 8, reinterpret_cast<GM_ADDR>(0x33), 1);
    EXPECT_EQ(handleId, 0);
    int32_t ret = hcomm.Commit(handleId);
    EXPECT_EQ(ret, 0);
    channel.CompleteCurrentSq();
    ret = hcomm.Wait(handleId);
    EXPECT_EQ(ret, 0);
}

// WriteWithNotifyNbi occupies 2 BBs, WriteNbi occupies 1 BB
// Verified by reading sqPI value directly from the UB buffer
TEST_F(HcommUrmaTestSuite, Aiv_Urma_WriteWithNotify_WqeBbCnt)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);

    // WriteNbi should advance sqPI by 1
    AscendC::HcommHandle h0 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1008), reinterpret_cast<GM_ADDR>(0x2008), 8);
    EXPECT_EQ(h0, 0);
    EXPECT_EQ(GetSqPIFromBuf(), 1U);

    // WriteWithNotifyNbi should advance sqPI by 2
    AscendC::HcommHandle h1 = hcomm.WriteWithNotifyNbi<false>(channel.GetHandle(),
        reinterpret_cast<GM_ADDR>(0x3008), reinterpret_cast<GM_ADDR>(0x5008), 8, reinterpret_cast<GM_ADDR>(0x33), 1);
    EXPECT_EQ(h1, 1);
    EXPECT_EQ(GetSqPIFromBuf(), 3U);

    // Another WriteNbi should advance by 1 more
    AscendC::HcommHandle h2 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1008), reinterpret_cast<GM_ADDR>(0x2008), 8);
    EXPECT_EQ(h2, 2);
    EXPECT_EQ(GetSqPIFromBuf(), 4U);
}

// Remote buffer lookup failure when address is out of range
TEST_F(HcommUrmaTestSuite, Aiv_Urma_RemoteBufferNotFound)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);

    // Address completely outside any remote buffer range (remote buffers are at 0x1000-0x2000 and 0x3000-0x4000)
    AscendC::HcommHandle h = hcomm.WriteWithNotifyNbi<false>(channel.GetHandle(),
        reinterpret_cast<GM_ADDR>(0x9000), reinterpret_cast<GM_ADDR>(0x5008), 8, reinterpret_cast<GM_ADDR>(0x33), 1);
    EXPECT_EQ(h, AscendC::HCOMM_INVALID_HANDLE_ID);

    // Address in range but len exceeds buffer boundary
    h = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1FF0), reinterpret_cast<GM_ADDR>(0x2008), 0x100);
    EXPECT_EQ(h, AscendC::HCOMM_INVALID_HANDLE_ID);
}

// Commit/Wait state machine constraints
TEST_F(HcommUrmaTestSuite, Aiv_Urma_CommitWaitStateMachine)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);

    AscendC::HcommHandle h0 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1008), reinterpret_cast<GM_ADDR>(0x2008), 8);
    EXPECT_EQ(h0, 0);

    // Wait without Commit should fail
    EXPECT_EQ(hcomm.Wait(h0), AscendC::HCOMM_FAILED);

    // First Commit should succeed
    EXPECT_EQ(hcomm.Commit(h0), AscendC::HCOMM_SUCCESS);

    // Duplicate Commit should fail
    EXPECT_EQ(hcomm.Commit(h0), AscendC::HCOMM_FAILED);

    // Wait after Commit should succeed
    channel.CompleteCurrentSq();
    EXPECT_EQ(hcomm.Wait(h0), AscendC::HCOMM_SUCCESS);

    // Duplicate Wait should fail
    EXPECT_EQ(hcomm.Wait(h0), AscendC::HCOMM_FAILED);
}

// Auto-commit via commit=true template parameter, then explicit Commit should fail
TEST_F(HcommUrmaTestSuite, Aiv_Urma_AutoCommitThenExplicitCommitFails)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);

    // commit=true (default) means PostSend internally calls Commit
    AscendC::HcommHandle h0 = hcomm.WriteNbi(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1008), reinterpret_cast<GM_ADDR>(0x2008), 8);
    EXPECT_EQ(h0, 0);

    // Explicit Commit should fail since already committed internally
    EXPECT_EQ(hcomm.Commit(h0), AscendC::HCOMM_FAILED);
}

// Batch commit: Commit(latest) marks all prior handles on same channel as committed
TEST_F(HcommUrmaTestSuite, Aiv_Urma_BatchCommit)
{
    UrmaChannelResource channel;

    AscendC::Hcomm<AscendC::CommProtocol::URMA, AscendC::CommEngine::AIV> hcomm;
    EXPECT_EQ(InitHcomm(hcomm), AscendC::HCOMM_SUCCESS);

    // Issue 3 writes without commit
    AscendC::HcommHandle h0 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1008), reinterpret_cast<GM_ADDR>(0x2008), 8);
    AscendC::HcommHandle h1 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1010), reinterpret_cast<GM_ADDR>(0x2010), 8);
    AscendC::HcommHandle h2 = hcomm.WriteNbi<false>(
        channel.GetHandle(), reinterpret_cast<GM_ADDR>(0x1018), reinterpret_cast<GM_ADDR>(0x2018), 8);
    EXPECT_EQ(h0, 0);
    EXPECT_EQ(h1, 1);
    EXPECT_EQ(h2, 2);

    // Commit only the last handle
    EXPECT_EQ(hcomm.Commit(h2), AscendC::HCOMM_SUCCESS);

    // All prior handles on same channel should now be committed (re-Commit fails)
    EXPECT_EQ(hcomm.Commit(h0), AscendC::HCOMM_FAILED);
    EXPECT_EQ(hcomm.Commit(h1), AscendC::HCOMM_FAILED);

    // Wait on the last handle should succeed (covers all)
    channel.CompleteCurrentSq();
    EXPECT_EQ(hcomm.Wait(h2), AscendC::HCOMM_SUCCESS);

    // All prior handles should also be marked as waited
    EXPECT_EQ(hcomm.Wait(h0), AscendC::HCOMM_FAILED);
    EXPECT_EQ(hcomm.Wait(h1), AscendC::HCOMM_FAILED);
}
