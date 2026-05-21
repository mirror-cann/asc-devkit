/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include <gtest/gtest.h>
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class Tensor_Api_Cube_Copy_L12L0ScaleB_3510 : public testing::Test {
protected:
    void SetUp() override
    {
        AscendC::SetGCoreType(1);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
    }
};

namespace {

template <typename LocationTag, typename Pointer, typename Layout>
auto MakeTensorAt(Pointer ptr, const Layout& layout)
{
    return AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr<LocationTag, fp8_e8m0_t>(reinterpret_cast<uint64_t>(ptr) / 16), layout);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyCallPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}, Trait{});
    atom.Call(dst, src);

    CopyAtom<CopyTraits<CopyOp, Trait>>{}.Call(dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}, dst, src);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyWithPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with();
    atom.Call(dst, src);
    Copy(atom, dst, src);
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_L12L0ScaleB_3510, CopyL12L0ScaleBRoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 16;
    __cbuf__ fp8_e8m0_t src[m * n] = {0};
    __cb__ fp8_e8m0_t dst[m * n] = {0};

    auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(src), MakeFrameLayout<NNLayoutPtn, AscendC::Std::Int<2>>(m, n));
    auto dstPtr = MakeMemPtr<Location::L0ScaleB, fp8_e8m0_t>((reinterpret_cast<uint64_t>(dst)) / 16);
    auto l0bTensor = MakeTensor(dstPtr, MakeFrameLayout<NNLayoutPtn, AscendC::Std::Int<2>>(m, n));
    auto coord = MakeCoord(AscendC::Std::Int<0>{}, AscendC::Std::Int<0>{});

    RunCopyCallPaths<CopyL12L0ScaleB, CopyL12L0ScaleBTraitDefault>(l0bTensor, l1Tensor);
    RunCopyWithPaths<CopyL12L0ScaleB, CopyL12L0ScaleBTraitDefault>(l0bTensor, l1Tensor);
    CopyAtom<CopyTraits<CopyL12L0ScaleB, CopyL12L0ScaleBTraitDefault>>{}.Call(l0bTensor, l1Tensor, coord);

    EXPECT_EQ(dst[0], static_cast<fp8_e8m0_t>(0));
}