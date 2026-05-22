/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/*!
 * \file Int.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_INT_IMPL_H
#define IMPL_STD_ASCENDC_STD_INT_IMPL_H
#include "integral_constant.h"
#include "enable_if.h"
#include "is_const.h"
#include "../algorithm/max.h"
#include "../algorithm/min.h"
#include "../cmath/divide.h"
#include "../cmath/ceil_division.h"
#include "../cmath/ceil_align.h"
#include "../cmath/sqrt.h"
#include "is_integral.h"

namespace AscendC {
namespace Std {
template <size_t v>
using Int = integral_constant<size_t, v>;

using _0      = Int<0>;
using _1      = Int<1>;
using _2      = Int<2>;
using _3      = Int<3>;
using _4      = Int<4>;
using _5      = Int<5>;
using _6      = Int<6>;
using _7      = Int<7>;
using _8      = Int<8>;
using _9      = Int<9>;
using _10     = Int<10>;
using _16     = Int<16>;
using _24     = Int<24>;
using _32     = Int<32>;
using _64     = Int<64>;
using _128    = Int<128>;
using _256    = Int<256>;
using _512    = Int<512>;
using _1024   = Int<1024>;
using _2048   = Int<2048>;
using _4096   = Int<4096>;


#define STD_INT_BINARY_OP(OP) \
template <auto t, auto u> \
__aicore__ inline constexpr Int<(t OP u)> operator OP (Int<t>, Int<u>) { \
    return {}; \
}

STD_INT_BINARY_OP(+);
STD_INT_BINARY_OP(-);
STD_INT_BINARY_OP(*);
STD_INT_BINARY_OP(/);
STD_INT_BINARY_OP(%);
#undef STD_INT_BINARY_OP

#define STD_INT_NAMED_BINARY_FN(OP, CMP) \
template <auto t, auto u> \
ASCENDC_HOST_AICORE inline constexpr Int<(CMP ? t : u)> OP(Int<t>, Int<u>) { \
    return {}; \
} \
template <auto t, typename U, typename = enable_if_t<is_integral<U>::value>> \
ASCENDC_HOST_AICORE inline constexpr auto OP(Int<t>, const U& u) { \
    return OP(static_cast<size_t>(t), u); \
} \
template <typename T, auto u, typename = enable_if_t<is_integral<T>::value>> \
ASCENDC_HOST_AICORE inline constexpr auto OP(const T& t, Int<u>) { \
    return OP(t, static_cast<size_t>(u)); \
}

STD_INT_NAMED_BINARY_FN(max, t > u);
STD_INT_NAMED_BINARY_FN(min, t < u);
#undef STD_INT_NAMED_BINARY_FN

#define STD_INT_BINARY_FN(OP, EXPR) \
template <auto t, auto u> \
ASCENDC_HOST_AICORE inline constexpr Int<EXPR> OP(Int<t>, Int<u>) { \
    return {}; \
} \
template <auto t, typename U, typename = enable_if_t<is_integral<U>::value>> \
ASCENDC_HOST_AICORE inline constexpr auto OP(Int<t>, const U& u) { \
    return OP(static_cast<size_t>(t), u); \
} \
template <typename T, auto u, typename = enable_if_t<is_integral<T>::value>> \
ASCENDC_HOST_AICORE inline constexpr auto OP(const T& t, Int<u>) { \
    return OP(t, static_cast<size_t>(u)); \
}

STD_INT_BINARY_FN(divide, (t / u));
STD_INT_BINARY_FN(ceil_division, ((t + u - 1) / u));
STD_INT_BINARY_FN(ceil_align, (((t + u - 1) / u) * u));
#undef STD_INT_BINARY_FN
}
}
#endif
