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

class Tensor_Api_Layout_Struct : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromShapeAndStride)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(2, 3, 4), MakeStride(12, 4, 1));

    EXPECT_EQ(AscendC::Std::get<0>(layout.Shape()), 2);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Shape()), 3);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Shape()), 4);

    EXPECT_EQ(AscendC::Std::get<0>(layout.Stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Stride()), 1);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromFlatShape)
{
    using namespace AscendC::Te;

    auto layout = MakeLayout(MakeShape(2, 3, 4));

    EXPECT_EQ(AscendC::Std::get<0>(layout.Stride()), 12);
    EXPECT_EQ(AscendC::Std::get<1>(layout.Stride()), 4);
    EXPECT_EQ(AscendC::Std::get<2>(layout.Stride()), 1);
    EXPECT_EQ(layout.Size(), 24);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeLayoutFromNestedShape)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(MakeShape(2, 3), MakeShape(4, 5));
    auto layout = MakeLayout(shape);

    auto rowStride = AscendC::Std::get<0>(layout.Stride());
    auto colStride = AscendC::Std::get<1>(layout.Stride());

    EXPECT_EQ(AscendC::Std::get<0>(rowStride), 1);
    EXPECT_EQ(AscendC::Std::get<1>(rowStride), 8);
    EXPECT_EQ(AscendC::Std::get<0>(colStride), 2);
    EXPECT_EQ(AscendC::Std::get<1>(colStride), 24);
}

TEST_F(Tensor_Api_Layout_Struct, TestRemoveBatchDimFromThreeDimLayout)
{
    using namespace AscendC::Te;

    auto layout = MakePatternLayout<NDLayoutPtn, LayoutTraitDefault<>>(
        MakeShape(2, MakeShape(8, 16)), MakeStride(128, MakeStride(16, 1)));
    auto noBatchLayout = RemoveBatchDim(layout);

    static_assert(decltype(noBatchLayout)::depth == TWO_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(noBatchLayout)>, NDLayoutPtn>);
    EXPECT_EQ(AscendC::Std::get<0>(noBatchLayout.Shape()), 8);
    EXPECT_EQ(AscendC::Std::get<1>(noBatchLayout.Shape()), 16);
    EXPECT_EQ(AscendC::Std::get<0>(noBatchLayout.Stride()), 16);
    EXPECT_EQ(AscendC::Std::get<1>(noBatchLayout.Stride()), 1);
}

TEST_F(Tensor_Api_Layout_Struct, TestRemoveBatchDimFromFiveDimLayout)
{
    using namespace AscendC::Te;

    auto layout = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(
        MakeShape(2, MakeShape(MakeShape(16, 2), MakeShape(16, 4))),
        MakeStride(2048, MakeStride(MakeStride(16, 256), MakeStride(1, 512))));
    auto noBatchLayout = RemoveBatchDim(layout);
    auto shape = noBatchLayout.Shape();
    auto stride = noBatchLayout.Stride();

    static_assert(decltype(noBatchLayout)::depth == FOUR_DIM_DATA);
    static_assert(AscendC::Std::is_same_v<GetLayoutPattern<decltype(noBatchLayout)>, NZLayoutPtn>);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(shape)), 2);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(shape)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(shape)), 4);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<0>(stride)), 16);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<0>(stride)), 256);
    EXPECT_EQ(AscendC::Std::get<0>(AscendC::Std::get<1>(stride)), 1);
    EXPECT_EQ(AscendC::Std::get<1>(AscendC::Std::get<1>(stride)), 512);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeTuple)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(2, 3, 4);
    auto stride = MakeStride(12, 4, 1);
    auto tile = MakeTile(8, 16);
    auto coord = MakeCoord(1, 2, 3);

    EXPECT_EQ(AscendC::Std::get<0>(shape), 2);
    EXPECT_EQ(AscendC::Std::get<1>(shape), 3);
    EXPECT_EQ(AscendC::Std::get<2>(shape), 4);

    EXPECT_EQ(AscendC::Std::get<0>(stride), 12);
    EXPECT_EQ(AscendC::Std::get<1>(stride), 4);
    EXPECT_EQ(AscendC::Std::get<2>(stride), 1);

    EXPECT_EQ(AscendC::Std::get<0>(tile), 8);
    EXPECT_EQ(AscendC::Std::get<1>(tile), 16);

    EXPECT_EQ(AscendC::Std::get<0>(coord), 1);
    EXPECT_EQ(AscendC::Std::get<1>(coord), 2);
    EXPECT_EQ(AscendC::Std::get<2>(coord), 3);
}

TEST_F(Tensor_Api_Layout_Struct, TestMakeIntTuple)
{
    using namespace AscendC::Te;

    auto shape = MakeShape(AscendC::Std::Int<4>{}, AscendC::Std::Int<5>{});
    auto stride = MakeStride(AscendC::Std::Int<5>{}, AscendC::Std::Int<1>{});
    auto coord = MakeCoord(AscendC::Std::Int<1>{}, AscendC::Std::Int<3>{});

    EXPECT_EQ(AscendC::Std::get<0>(shape).value, 4);
    EXPECT_EQ(AscendC::Std::get<1>(shape).value, 5);
    EXPECT_EQ(AscendC::Std::get<0>(stride).value, 5);
    EXPECT_EQ(AscendC::Std::get<1>(stride).value, 1);
    EXPECT_EQ(AscendC::Std::get<0>(coord).value, 1);
    EXPECT_EQ(AscendC::Std::get<1>(coord).value, 3);
}
