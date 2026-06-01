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
    "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/cube/l1_to_fb/copy.h"
#include "impl/tensor_api/arch/cube/utils/l0c2out_utils.h"
#include "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

struct CopyL0C2UBTrait {
    __aicore__ constexpr CopyL0C2UBTrait() {}

    __aicore__ constexpr CopyL0C2UBTrait(RoundMode roundModeIn, bool enableReluIn, bool enableChannelSplitIn,
                                         DualDstMode dualDstCtlIn) :
        roundMode(roundModeIn), enableRelu(enableReluIn), enableChannelSplit(enableChannelSplitIn),
        dualDstCtl(dualDstCtlIn)
    {}

    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
    DualDstMode dualDstCtl = DUAL_DST_DISABLE;
};

class DataCopyL0C2UBNoVectorQuant {
public:
    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src, const FixpipeParams& params)
    {
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();
        constexpr bool isNdFormat = IsSatisfiedPtnFormatV<T, NDExtLayoutPtn> || IsSatisfiedPtnFormatV<T, NDLayoutPtn>;
        constexpr bool isDnFormat = IsSatisfiedPtnFormatV<T, DNExtLayoutPtn> || IsSatisfiedPtnFormatV<T, DNLayoutPtn>;

        uint32_t nSize = Std::min(GetTotalColumnShape(srcLayout), GetTotalColumnShape(dstLayout));
        uint32_t mSize = Std::min(GetTotalRowShape(srcLayout), GetTotalRowShape(dstLayout));
        uint32_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) / FRACTAL_FIXED;
        uint32_t dstStride = 0;

        if constexpr (isNdFormat) {
            if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Row>(dstLayout);
            } else {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
            }
        } else {
            if constexpr (IsSatisfiedPtnFormatV<T, DNLayoutPtn>) {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column>(dstLayout);
            } else {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
            }
        }

        bool reluEn = trait.enableRelu;
        uint8_t unitFlag = params.unitFlag;
        bool nz2ndEn = isNdFormat;
        bool nz2dnEn = isDnFormat;

        bool isChannelSplit = trait.enableChannelSplit;

        uint8_t dualDstCtl = trait.dualDstCtl;
        bool subBlockId = params.subBlockId;
        CopyMatrixCcToUbInstr::DataCopy<quantPre, T, U>(dst, src, nSize, mSize, srcStride, dstStride, dualDstCtl,
                                                        reluEn, unitFlag, subBlockId, isChannelSplit, nz2ndEn, nz2dnEn);
    }
};

class DataCopyL0C2UBVectorQuant {
public:
    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src, const V& quant, const FixpipeParams& params)
    {
        uint32_t nSize = GetTotalColumnShape(src.Layout());
        uint16_t nIterNum = 1;
        uint32_t calNSize = nSize;
        uint32_t tailNSize = 0;
        if (calNSize > MAIN_LOOP_N_SIZE) {
            nIterNum = nSize / MAIN_LOOP_N_SIZE;
            tailNSize = nSize % MAIN_LOOP_N_SIZE;
            calNSize = MAIN_LOOP_N_SIZE;
        }
        ExecuteDataCopy<trait, quantPre>(dst, src, quant, nIterNum, calNSize, tailNSize, params);
    }

private:
    template <const CopyL0C2UBTrait& trait, typename T, typename U, bool IsTail>
    __aicore__ inline static auto GenerateParams(const T& dst, const U& src, const FixpipeParams& params)
    {
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();
        constexpr bool isNdFormat = IsSatisfiedPtnFormatV<T, NDExtLayoutPtn> || IsSatisfiedPtnFormatV<T, NDLayoutPtn>;
        constexpr bool isDnFormat = IsSatisfiedPtnFormatV<T, DNExtLayoutPtn> || IsSatisfiedPtnFormatV<T, DNLayoutPtn>;

        uint32_t nSize = Std::min(GetTotalColumnShape(srcLayout), GetTotalColumnShape(dstLayout));
        uint32_t mSize = Std::min(GetTotalRowShape(srcLayout), GetTotalRowShape(dstLayout));
        if constexpr (IsTail) {
            nSize %= MAIN_LOOP_N_SIZE;
        } else {
            if (nSize > MAIN_LOOP_N_SIZE) {
                nSize = MAIN_LOOP_N_SIZE;
            }
        }

        const uint32_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) / FRACTAL_FIXED;
        uint32_t dstStride = 0;
        if constexpr (isNdFormat) {
            if constexpr (IsSatisfiedPtnFormatV<T, NDLayoutPtn>) {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Row>(dstLayout);
            } else {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
            }
        } else {
            if constexpr (IsSatisfiedPtnFormatV<T, DNLayoutPtn>) {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column>(dstLayout);
            } else {
                dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
            }
        }

        const bool reluEn = trait.enableRelu;
        const uint8_t unitFlag = params.unitFlag;

        constexpr bool nz2ndEn = isNdFormat;
        constexpr bool nz2dnEn = isDnFormat;

        const bool channelSplit = trait.enableChannelSplit;
        bool subBlockId = params.subBlockId;
        uint8_t dualDstCtl = trait.dualDstCtl;

        return Std::make_tuple(nSize, mSize, srcStride, dstStride, dualDstCtl, reluEn, unitFlag, subBlockId,
                               channelSplit, nz2ndEn, nz2dnEn);
    }

    template <typename T>
    __aicore__ inline static void CopyL12Fb(const T& src, uint16_t calNSize, uint16_t nIterIndex)
    {
        auto dstAddr = reinterpret_cast<__fbuf__ uint64_t*>(AllocFbTempBuf(calNSize));
        auto dst = MakeTensor(MakeMemPtr<Location::FIXBUF>(dstAddr), src.Layout());
        auto coord = MakeCoord(_0{}, nIterIndex * MAIN_LOOP_N_SIZE);
        auto shape = MakeShape(_1{}, calNSize);
        auto tileSrc = src.Slice(coord, shape);
        CopyL12FBND::Run<DEFAULT_COPY_L1_FB_TRAIT>(dst, tileSrc);
        SetFpc(dstAddr);
    }

    template <QuantMode_t quantPre, typename T, typename U, typename ParamTuple, size_t... Is>
    __aicore__ inline static void DataCopyWrapper(const T& dst, const U& src, const ParamTuple& paramTuple,
                                                  Std::index_sequence<Is...>)
    { CopyMatrixCcToUbInstr::DataCopy<quantPre>(dst, src, Std::get<Is>(paramTuple)...); }

    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V>
    __aicore__ inline static void ExecuteDataCopy(const T& dst, const U& src, const V& quant, uint16_t nIterNum,
                                                  uint32_t calNSize, uint32_t tailNSize, const FixpipeParams& params)
    {
        const auto mainLoopParam = GenerateParams<trait, T, U, false>(dst, src, params);

        for (uint16_t i = 0; i < nIterNum; ++i) {
            CopyL12Fb(quant, calNSize, i);
            InsertSync();

            const auto srcCoord = MakeCoord(MakeCoord(0, 0), MakeCoord(0, i * CBURST_NUM));
            const auto dstCoord = MakeCoord(MakeCoord(0, 0), MakeCoord(0, i * MAIN_LOOP_N_SIZE));

            DataCopyWrapper<quantPre>(dst(dstCoord), src(srcCoord), mainLoopParam,
                                      Std::make_index_sequence<Std::tuple_size_v<decltype(mainLoopParam)>>{});
        }

        if (tailNSize) {
            const auto tailParam = GenerateParams<trait, T, U, true>(dst, src, params);

            CopyL12Fb(quant, tailNSize, nIterNum);
            InsertSync();

            const auto srcCoord = MakeCoord(MakeCoord(0, 0), MakeCoord(0, nIterNum * CBURST_NUM));
            const auto dstCoord = MakeCoord(MakeCoord(0, 0), MakeCoord(0, nIterNum * MAIN_LOOP_N_SIZE));

            DataCopyWrapper<quantPre>(dst(dstCoord), src(srcCoord), tailParam,
                                      Std::make_index_sequence<Std::tuple_size_v<decltype(tailParam)>>{});
        }
    }
};

class DataCopyL0C2UB {
public:
    template <const CopyL0C2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U>();
        CheckDataType::CheckL0C2UbDataType<quantPre, T, U>();
        SetRegisterImpl<T, U>(dst, src);
        DataCopyL0C2UBNoVectorQuant::DataCopyImpl<trait, quantPre, T, U>(dst, src, params);
    }

    template <const CopyL0C2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src, uint64_t quant, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U, uint64_t>();
        SetRegisterImpl<T, U>(dst, src, quant);
        DataCopyL0C2UBNoVectorQuant::DataCopyImpl<trait, quantPre, T, U>(dst, src, params);
    }

    template <const CopyL0C2UBTrait& trait, typename T, typename U, typename V>
    __aicore__ inline static void Run(const T& dst, const U& src, const V& quant, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U, V>();
        SetRegisterImpl<T, U>(dst, src);
        DataCopyL0C2UBVectorQuant::DataCopyImpl<trait, quantPre, T, U, V>(dst, src, quant, params);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
