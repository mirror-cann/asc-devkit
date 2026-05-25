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

class Tensor_Api_Tensor_Slice : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceByShape)
{
    using namespace AscendC::Te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = MakeFrameLayout<ScaleANDLayoutPtn>(6, 8);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto sliced = Slice(tensor, MakeCoord(1, 2), MakeShape(3, 3));

    EXPECT_EQ(sliced.Data(), tensor.Data() + layout(MakeCoord(1, 2)));
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0))], 10);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 2), MakeCoord(0, 1))], 27);
}

TEST_F(Tensor_Api_Tensor_Slice, TestLocalTensorSliceByLayout)
{
    using namespace AscendC::Te;

    __gm__ float data[48] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
    auto layout = MakeFrameLayout<ScaleANDLayoutPtn>(6, 8);
    auto tensor = MakeTensor(MakeMemPtr<Location::GM>(data), layout);
    auto infoLayout = MakeFrameLayout<ScaleANDLayoutPtn>(2, 4);
    auto sliced = Slice(tensor, MakeCoord(MakeCoord(0, 1), MakeCoord(0, 2)), infoLayout);

    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0))], 10);
    EXPECT_EQ(sliced[MakeCoord(MakeCoord(0, 1), MakeCoord(0, 3))], 21);
}
