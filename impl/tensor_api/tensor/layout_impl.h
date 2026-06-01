/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/tensor/layout_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/layout_method.h"
#include "impl/tensor_api/tensor/coord_index.h"
#include "impl/tensor_api/tensor/layout_fractal.h"
#include "impl/tensor_api/tensor/layout_frame.h"

namespace AscendC {
namespace Te {

struct MinOp {
    template <typename T, typename U>
    __aicore__ inline constexpr auto operator()(const T& src, const U& dst) const
    {
        return Std::min(src, dst);
    }
};

struct DiffOp {
    template <typename T, typename U>
    __aicore__ inline constexpr auto operator()(const T& shape, const U& coord) const
    {
        return shape - coord;
    }
};

template <typename Coord, typename LayoutType>
__aicore__ inline decltype(auto) MakeCoordLayout(const Coord& coord, const LayoutType& layout) 
{
    using ShapeType = Std::remove_cvref_t<decltype(layout.Shape())>;
    using CoordType = Std::remove_cvref_t<Coord>;
    static_assert(IsLayoutV<LayoutType> && Std::is_tuple_v<CoordType>, "LayoutType must be Layout");
    static_assert(NestingDepthV<ShapeType> == NestingDepthV<CoordType> && 
        Std::tuple_size_v<ShapeType> == Std::tuple_size_v<CoordType>, 
        "Shape and coord must have same tuple structure");
    auto coordShape = TransformTupleApply(layout.Shape(), coord, DiffOp{});
    using TraitType = GetLayoutTrait<LayoutType>;
    using PatternType = GetLayoutPattern<LayoutType>;
    return MakePatternLayout<PatternType, TraitType>(coordShape, layout.Stride());
}

template <typename LayoutType, typename ShapeType>
__aicore__ inline decltype(auto) MakeSlicePatternLayout(const LayoutType& layout, const ShapeType& shape)
{
    using TraitType = GetLayoutTrait<LayoutType>;
    using PatternType = GetLayoutPattern<LayoutType>;
    return MakePatternLayout<PatternType, TraitType>(shape, layout.Stride());
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeTwoDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == TWO_DIM_DATA,
        "SliceShape must be Two Dim when layout is Two Dim");
    auto srcRow = Get<0>(layout.Shape()) - Get<0>(coord);
    auto srcCol = Get<1>(layout.Shape()) - Get<1>(coord);
    auto realRow = Std::min(srcRow, Get<0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1>(sliceShape));
    return MakeSlicePatternLayout(layout, MakeShape(realRow, realCol));
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeThreeDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == THREE_DIM_DATA,
        "SliceShape must be Three Dim when layout is Three Dim");
    auto srcBatch = Get<0>(layout.Shape()) - Get<0>(coord);
    auto srcRow = Get<1, 0>(layout.Shape()) - Get<1, 0>(coord);
    auto srcCol = Get<1, 1>(layout.Shape()) - Get<1, 1>(coord);
    auto realBatch = Std::min(srcBatch, Get<0>(sliceShape));
    auto realRow = Std::min(srcRow, Get<1, 0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1, 1>(sliceShape));
    return MakeSlicePatternLayout(layout, MakeShape(realBatch, MakeShape(realRow, realCol)));
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeFourDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == TWO_DIM_DATA,
        "SliceShape must be Two Dim when layout is Four Dim");
    auto innerRow = Get<0, 0>(layout.Shape());
    auto innerCol = Get<1, 0>(layout.Shape());

    auto srcRow = innerRow * Get<0, 1>(layout.Shape()) - Get<0>(coord);
    auto srcCol = innerCol * Get<1, 1>(layout.Shape()) - Get<1>(coord);

    auto realRow = Std::min(srcRow, Get<0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1>(sliceShape));
    return MakeSlicePatternLayout(layout, MakeFractalShape(MakeShape(realRow, realCol),
        MakeShape(innerRow, innerCol)));
}

template <typename Coord, typename LayoutType, typename SliceShape>
__aicore__ inline decltype(auto) MakeFiveDimSliceLayout(
    const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape)
{
    static_assert(NestingDepthV<SliceShape> == THREE_DIM_DATA,
        "SliceShape must be Three Dim when layout is Five Dim");
    auto innerRow = Get<1, 0, 0>(layout.Shape());
    auto innerCol = Get<1, 1, 0>(layout.Shape());

    auto srcBatch = Get<0>(layout.Shape()) - Get<0>(coord);
    auto srcRow = innerRow * Get<1, 0, 1>(layout.Shape()) - Get<1, 0>(coord);
    auto srcCol = innerCol * Get<1, 1, 1>(layout.Shape()) - Get<1, 1>(coord);

    auto realBatch = Std::min(srcBatch, Get<0>(sliceShape));
    auto realRow = Std::min(srcRow, Get<1, 0>(sliceShape));
    auto realCol = Std::min(srcCol, Get<1, 1>(sliceShape));
    auto fractalShape = MakeFractalShape(MakeShape(realRow, realCol), MakeShape(innerRow, innerCol));
    return MakeSlicePatternLayout(layout, MakeShape(realBatch, fractalShape));
}

template <typename Coord, typename LayoutType, typename SliceShape, Std::enable_if_t<!IsLayoutV<SliceShape>, int> = 0>
__aicore__ inline decltype(auto) MakeSliceLayout(const Coord& coord, const LayoutType& layout, const SliceShape& sliceShape) 
{
    static_assert(IsLayoutV<LayoutType>, "LayoutType must be Layout");
    static_assert(Std::is_tuple_v<SliceShape>, "SliceShape must be a tuple");
    static_assert(NestingDepthV<SliceShape> == TWO_DIM_DATA || NestingDepthV<SliceShape> == THREE_DIM_DATA,
        "Only Support Two Dim or Three Dim SliceShape");
    using OriginShape = Std::remove_cvref_t<decltype(layout.Shape())>;
    constexpr auto originShapeDepth = NestingDepthV<OriginShape>;
    if constexpr (originShapeDepth == TWO_DIM_DATA) {
        return MakeTwoDimSliceLayout(coord, layout, sliceShape);
    } else if constexpr (originShapeDepth == THREE_DIM_DATA) {
        return MakeThreeDimSliceLayout(coord, layout, sliceShape);
    } else if constexpr (originShapeDepth == FOUR_DIM_DATA) {
        return MakeFourDimSliceLayout(coord, layout, sliceShape);
    } else if constexpr (originShapeDepth == FIVE_DIM_DATA) {
        return MakeFiveDimSliceLayout(coord, layout, sliceShape);
    } else {
        static_assert(originShapeDepth == TWO_DIM_DATA || originShapeDepth == THREE_DIM_DATA ||
            originShapeDepth == FOUR_DIM_DATA || originShapeDepth == FIVE_DIM_DATA,
            "Only Support Two Dim, Three Dim, Four Dim, or Five Dim Layout");
    }
}

template <typename Coord, typename SrcLayoutType, typename DstLayoutType, Std::enable_if_t<IsLayoutV<DstLayoutType>, int> = 0>
__aicore__ inline decltype(auto) MakeSliceLayout(const Coord& coord, const SrcLayoutType& srcLayout, const DstLayoutType& dstLayout) 
{
    static_assert(IsLayoutV<SrcLayoutType>, "SrcLayoutType must be Layout");
    static_assert(SrcLayoutType::rank == DstLayoutType::rank,
        "SrcLayout Rank must be equal to DstLayout Rank");

    auto sliceLayout = MakeCoordLayout(coord, srcLayout);
    auto sliceShape = TransformTupleApply(sliceLayout.Shape(), dstLayout.Shape(), MinOp{});
    using TraitType = GetLayoutTrait<SrcLayoutType>;
    using PatternType = GetLayoutPattern<SrcLayoutType>;
    return MakePatternLayout<PatternType, TraitType>(sliceShape, srcLayout.Stride());
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
