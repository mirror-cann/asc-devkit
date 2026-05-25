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

using namespace AscendC::Te;

class Tensor_Api_Cube_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

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
using namespace AscendC::Te;
constexpr bool enableRelu = false;
constexpr bool enableChannelSplit = true;
constexpr DualDstMode dualDstCtl = DUAL_DST_DISABLE;
constexpr CopyL0C2UBTrait l0c2ubTrait = {RoundMode::DEFAULT, enableRelu, enableChannelSplit, dualDstCtl};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
};


template <typename LocationTag, typename Pointer, typename Layout>
auto MakeTensorAt(Pointer ptr, const Layout& layout)
{
    return AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr<LocationTag>(ptr), layout);
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

template <typename CopyOp, typename Trait, typename Param, typename DstTensor, typename SrcTensor>
void RunCopyWithParamPaths(const DstTensor& dst, const SrcTensor& src, const Param& param)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}).with(param);
    atom.Call(dst, src);

    auto copiedAtom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param);
    copiedAtom.Call(dst, src);

    Copy(copiedAtom, dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param), dst, src);
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2ND)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NDLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}


TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2DN)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2DNLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NZNoChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}


TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NZWithChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitCustom>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitCustom>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}
