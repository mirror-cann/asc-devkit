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
    "impl/tensor_api/arch/vector/binary/binary_vf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
* \file binary_vf.h
* \brief
*/
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_BINARY_BINARY_VF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_BINARY_BINARY_VF_H

#include "impl/tensor_api/arch/vector/binary/instruction.h"

namespace AscendC {
namespace Te {

template<typename CalcFunc, typename UpdateMaskFunc, typename T>
__simd_vf__ inline void BinaryVF(uint16_t rep, uint16_t oneRepSize, uint32_t blockLength, __ubuf__ T* zLocal, __ubuf__ T* xLocal, __ubuf__ T* yLocal)
{
    using VectorTypeTransform = TupleMap<
    Std::tuple<uint8_t,        vector_uint8_t>,
    Std::tuple<uint16_t,       vector_uint16_t>,
    Std::tuple<uint32_t,       vector_uint32_t>,
    Std::tuple<uint64_t,       vector_uint64_t>,
    Std::tuple<int8_t,         vector_int8_t>,
    Std::tuple<int16_t,        vector_int16_t>,
    Std::tuple<int32_t,        vector_int32_t>,
    Std::tuple<int64_t,        vector_int64_t>,
    Std::tuple<bfloat16_t,     vector_bfloat16_t>,
    Std::tuple<half,           vector_half>,
    Std::tuple<float,          vector_float>,
    Std::tuple<hifloat8_t,     vector_hifloat8_t>,
    Std::tuple<fp8_e4m3fn_t,   vector_fp8_e4m3fn_t>,
    Std::tuple<fp8_e5m2_t,     vector_fp8_e5m2_t>,
    Std::tuple<fp8_e8m0_t,     vector_fp8_e8m0_t>,
    Std::tuple<int4x2_t,       vector_int4x2_t>,
    Std::tuple<fp4x2_e2m1_t,   vector_fp4x2_e2m1_t>,
    Std::tuple<fp4x2_e1m2_t,   vector_fp4x2_e1m2_t>>;

    using RegType = typename VectorTypeTransform::template Get<T>;

    vector_bool vmask;
    RegType reg_src0;
    RegType reg_src1;
    RegType reg_dst;
    for (uint16_t i = 0; i < rep; i++) {
        vmask = UpdateMaskFunc::template Run<T>(blockLength);
        asc_loadalign(reg_src0, xLocal + i * oneRepSize);
        asc_loadalign(reg_src1, yLocal + i * oneRepSize);
        CalcFunc::template Run<RegType>(reg_dst, reg_src0, reg_src1, vmask);
        asc_storealign(zLocal + i * oneRepSize, reg_dst, vmask);
    }
}

template<typename CalcFunc, typename Args0, typename... Args>
__aicore__ inline void Transform2VF(const Args0& args0, const Args&... args)
{
    using type = GetAttributeElementType<typename Args0::elementType*>;
    uint32_t blockLength = args0.Size();

    constexpr uint16_t VECTOR_REG_WIDTH = 256;
    constexpr uint16_t oneRepSize = VECTOR_REG_WIDTH / sizeof(type);
    uint16_t rep = Std::ceil_division(blockLength, oneRepSize);
    
    BinaryVF<CalcFunc, Inst::UpdateMask, type>(rep, oneRepSize, blockLength, args0.Data().Get(), args.Data().Get()...);
}

}
}

#endif // IMPL_TENSOR_API_ARCH_VECTOR_BINARY_BINARY_VF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
