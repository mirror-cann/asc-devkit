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
    "impl/tensor_api/arch/vector/binary/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
* \file instruction.h
* \brief
*/
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_BINARY_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_BINARY_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {
namespace Inst {

class Add {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_add(dst, src0, src1, mask);
    }
};

class Madd {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_madd(dst, src0, src1, mask);
    }
};

class Sub {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_sub(dst, src0, src1, mask);
    }
};

class And {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_and(dst, src0, src1, mask);
    }
};

class Select {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_select(dst, src0, src1, mask);
    }
};

class AbsSub {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_abs_sub(dst, src0, src1, mask);
    }
};

class Min {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_min(dst, src0, src1, mask);
    }
};

class Max {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_max(dst, src0, src1, mask);
    }
};

class Or {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_or(dst, src0, src1, mask);
    }
};

class Mul {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_mul(dst, src0, src1, mask);
    }
};

class ExpSubEven {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_exp_sub(dst, src0, src1, mask);
    }
};

class ExpSubOdd {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_exp_sub_v2(dst, src0, src1, mask);
    }
};

class ShiftLeft {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_shiftleft(dst, src0, src1, mask);
    }
};

class ShiftRight {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_shiftright(dst, src0, src1, mask);
    }
};

class Div {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_div(dst, src0, src1, mask);
    }
};

class Xor {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_xor(dst, src0, src1, mask);
    }
};

class Prelu {
public:
    template <typename T>
    __simd_callee__ inline static void Run(T& dst, T src0, T src1, vector_bool mask) {
        asc_prelu(dst, src0, src1, mask);
    }
};

class UpdateMask {
public:
    template <typename T, typename U>
    __simd_callee__ inline static vector_bool Run(U& value)
    {
        if constexpr (sizeof(T) == sizeof(uint32_t)) {
            return asc_update_mask_b32(value);
        } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
            return asc_update_mask_b16(value);
        } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
            return asc_update_mask_b8(value);
        } else {
            return vector_bool{};
        }
    }
};

}
}
}

#endif // IMPL_TENSOR_API_ARCH_VECTOR_BINARY_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
