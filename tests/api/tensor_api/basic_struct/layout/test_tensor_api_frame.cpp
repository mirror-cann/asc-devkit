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

class Tensor_Api_Frame_Layout : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupDefaultTrait)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupTraitWithType)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}

TEST_F(Tensor_Api_Frame_Layout, NDAndDNGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto ndLayout = MakeFrameLayout<NDLayoutPtn, AscendC::Std::_16>(8, 16);
    auto dnLayout = MakeFrameLayout<DNLayoutPtn, AscendC::Std::_16>(8, 16);
    auto ndExtLayout = MakeFrameLayout<NDExtLayoutPtn, AscendC::Std::_16>(8, 16);
    auto dnExtLayout = MakeFrameLayout<DNExtLayoutPtn, AscendC::Std::_16>(8, 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(ndLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(ndLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(ndLayout)), 1);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape(dnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape(dnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<0>(GetStride(dnLayout)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(GetStride(dnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(ndExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(ndExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(ndExtLayout))), 1);

    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetShape(dnExtLayout))), 8);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetShape(dnExtLayout))), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(GetStride(dnExtLayout))), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(GetStride(dnExtLayout))), 8);
}


TEST_F(Tensor_Api_Frame_Layout, ScaleGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto andLayout = MakeFrameLayout<ScaleANDLayoutPtn>(32, 16);
    auto adnLayout = MakeFrameLayout<ScaleADNLayoutPtn>(32, 16);
    auto bndLayout = MakeFrameLayout<ScaleBNDLayoutPtn>(16, 32);
    auto bdnLayout = MakeFrameLayout<ScaleBDNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(andLayout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(andLayout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(adnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(adnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(bndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bndLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bdnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(bdnLayout)), 32);
}

TEST_F(Tensor_Api_Frame_Layout, ScaleGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto andLayout = MakeFrameLayout<ScaleANDLayoutPtn>(32, 16);
    auto adnLayout = MakeFrameLayout<ScaleADNLayoutPtn>(32, 16);
    auto bndLayout = MakeFrameLayout<ScaleBNDLayoutPtn>(16, 32);
    auto bdnLayout = MakeFrameLayout<ScaleBDNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(andLayout)), 32);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(andLayout)), 16);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(adnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(adnLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(bndLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bndLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(bdnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(bdnLayout)), 32);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupDefaultTrait)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, LayoutTrait<fp8_e8m0_t, _2>>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupTraitWithType)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}


TEST_F(Tensor_Api_Frame_Layout, OtherGroupWithDataType)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, float>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, float>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, float>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}


TEST_F(Tensor_Api_Frame_Layout, OtherGroupWithC0Element)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, 8>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, 8>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, 8>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, 2>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupTraitWithTypeAndC0Element)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, LayoutTraitDefault<float>>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 8);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}

TEST_F(Tensor_Api_Frame_Layout, OtherGroupIntegralConstantTrait)
{
    using namespace AscendC::Te;

    auto nzLayout = MakeFrameLayout<NZLayoutPtn, AscendC::Std::_16>(32, 64);
    auto znLayout = MakeFrameLayout<ZNLayoutPtn, AscendC::Std::_16>(32, 64);
    auto zzLayout = MakeFrameLayout<ZZLayoutPtn, AscendC::Std::_16>(32, 64);
    auto nnLayout = MakeFrameLayout<NNLayoutPtn, AscendC::Std::_2>(16, 32);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(znLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(znLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(znLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(zzLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(zzLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(zzLayout)), 4);

    EXPECT_EQ(AscendC::Std::get<0>(GetShape<0>(nnLayout)), 2);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<0>(nnLayout)), 8);
    EXPECT_EQ(AscendC::Std::get<0>(GetShape<1>(nnLayout)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(GetShape<1>(nnLayout)), 2);
}
