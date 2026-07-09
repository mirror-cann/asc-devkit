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
 * \file asc_aicore_printf_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_PRINTF_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_PRINTF_UTILS_H

#include "impl/utils/debug/asc_debug_utils.h"
#include "impl/utils/debug/npu_arch_3510/asc_type_conversion_utils.h"
namespace __asc_aicore {

template <typename T>
__aicore__ inline void set_scalar_param_impl(__gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    __gm__ uint64_t *scalarAddr = (__gm__ uint64_t *)paramAddr + paramIdx;
    *scalarAddr = 0;

    if constexpr (is_same_in_list<T, half, float>()) {
        *((__gm__ float *)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr (is_same_in_list<T, double>()) {
        *((__gm__ double *)scalarAddr) = static_cast<double>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t *)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr(is_same_in_list<T, bfloat16_t, float8_e5m2_t, float8_e8m0_t, float8_e4m3_t, hifloat8_t>()) {
        *((__gm__ float *)scalarAddr) = to_float(scalar);
    } else if constexpr(std::is_pointer<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = (uintptr_t)scalar;
    } else if constexpr(std::is_enum<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    }
    asc_entire_dcci((__gm__ uint64_t*)scalarAddr);
}

} // namespace __asc_aicore

namespace __asc_simd_vf {
template <typename T>
__simd_callee__ inline void set_scalar_param_vf_impl(__ubuf__ uint8_t* param_addr, uint32_t param_idx, T scalar)
{
    __ubuf__ uint64_t *scalar_addr = (__ubuf__ uint64_t *)param_addr + param_idx;
    *scalar_addr = 0;

    if constexpr (is_same_in_list<T, half, float>()) {
        *((__ubuf__ float *)scalar_addr) = static_cast<float>(scalar);
    } else if constexpr (is_same_in_list<T, double>()) {
        *((__ubuf__ double *)scalar_addr) = static_cast<double>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__ubuf__ int64_t *)scalar_addr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__ubuf__ uint64_t *)scalar_addr) = static_cast<uint64_t>(scalar);
    } else if constexpr(is_same_in_list<T, bfloat16_t, float8_e5m2_t, float8_e8m0_t, float8_e4m3_t, hifloat8_t>()) {
        *((__ubuf__ float *)scalar_addr) = to_float(scalar);
    } else if constexpr(std::is_pointer<T>::value) {
        *((__ubuf__ uint64_t *)scalar_addr) = (uintptr_t)scalar;
    } else if constexpr(std::is_enum<T>::value) {
        *((__ubuf__ uint64_t *)scalar_addr) = static_cast<uint64_t>(scalar);
    }
}

__simd_callee__ constexpr uint32_t align_print_tlv_len(const uint32_t data_len)
{
    constexpr uint32_t align_bytes = 8;
    return ((data_len + (align_bytes - 1)) & ~(align_bytes - 1)) + align_bytes;
}

template <typename T>
__simd_callee__ inline void set_scalar_param_vf(__ubuf__ uint8_t* param_addr, uint32_t param_idx, T scalar)
{
    set_scalar_param_vf_impl(param_addr, param_idx, scalar);
}

__simd_callee__ inline void set_string_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, __ubuf__ const char* s, uint32_t& offset)
{
    __ubuf__ uint64_t* string_addr = reinterpret_cast<__ubuf__ uint64_t*>(param_addr) + param_idx;
    __ubuf__ uint8_t* dst_str_addr = param_addr + offset;

    // write string value offset
    *string_addr = static_cast<uint64_t>(offset - sizeof(uint64_t) * param_idx);

    // write string content: GM -> UBuf
    uint32_t str_len = get_cstring_len_vf(s);
    for (uint32_t i = 0; i < str_len; i++) {
        *(dst_str_addr + i) = *(s + i);
    }
    offset += str_len;
}

__simd_callee__ inline void set_param_vf(__ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset)
{
    (void)param_addr;
    (void)param_idx;
    (void)offset;
    return;
}

template <typename... Args>
__simd_callee__ inline void set_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, Args&&... args);

template <typename... Args>
__simd_callee__ inline void set_param_vf_impl(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, __ubuf__ const char* s, Args&&... args)
{
    set_string_param_vf(param_addr, param_idx, s, offset);
    set_param_vf(param_addr, param_idx + 1, offset, args...);
}

template <typename T, typename... Args>
__simd_callee__ inline void set_param_vf_impl(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, T scalar, Args&&... args)
{
    set_scalar_param_vf(param_addr, param_idx, scalar);
    set_param_vf(param_addr, param_idx + 1, offset, args...);
}

template <typename... Args>
__simd_callee__ inline void set_param_vf(
    __ubuf__ uint8_t* param_addr, uint32_t param_idx, uint32_t& offset, Args&&... args)
{
    set_param_vf_impl(param_addr, param_idx, offset, args...);
}

__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num)
{
    (void)args_num;
    return 0;
}

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num, Args&&... args);

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf_impl(uint32_t& args_num, __ubuf__ const char* s, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    const uint32_t str_len = get_cstring_len_vf(s);
    args_num += 1;
    return param_size + str_len + get_args_len_vf(args_num, args...);
}

template <typename T, typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf_impl(uint32_t& args_num, T scalar, Args&&... args)
{
    constexpr uint32_t param_size = sizeof(uint64_t);
    args_num += 1;
    return param_size + get_args_len_vf(args_num, args...);
}

template <typename... Args>
__simd_callee__ inline uint32_t get_args_len_vf(uint32_t& args_num, Args&&... args)
{
    return get_args_len_vf_impl(args_num, args...);
}

template <typename... Args>
__simd_callee__ inline uint32_t get_print_tlv_len_simd(uint32_t& args_num, __ubuf__ const char* fmt, Args&&... args)
{
    constexpr uint32_t print_info_len = sizeof(PrintTlv);
    const uint32_t args_len = get_args_len_vf(args_num, args...);
    const uint32_t fmt_len = get_cstring_len_vf(fmt);
    return align_print_tlv_len(print_info_len + args_len + fmt_len);
}

__simd_callee__ inline void set_print_tlv_info_vf(
    DumpType debug_type, __ubuf__ PrintTlv* print_tlv, const uint32_t& tlv_len, const uint32_t& args_num, uint16_t block_idx)
{
    print_tlv->type = static_cast<uint32_t>(debug_type);
    print_tlv->length = tlv_len - sizeof(uint32_t[2]); // exclude type and length
    print_tlv->blockIdx = block_idx;  // set in aicore
    print_tlv->resv = static_cast<uint32_t>(0U);
    print_tlv->fmtOffset = (args_num + 1) * sizeof(uint64_t); // include fmt offset
}

__simd_callee__ inline void copy_fmt_to_ubuf(__ubuf__ uint8_t* dst, __ubuf__ const char* src, uint32_t len)
{
    // Workaround for -O2 optimization issue: copy byte-by-byte to avoid miscompilation
    for (uint32_t i = 0; i < len; i += 2) {
        dst[i] = src[i];
        dst[i + 1] = src[i + 1];
    }
}

template <typename... Args>
__simd_callee__ inline void set_print_tlv_data_vf(
    __ubuf__ PrintTlv* print_tlv, __ubuf__ const char* fmt, Args&&... args)
{
    const uint32_t str_len = get_cstring_len_vf(fmt);
    __ubuf__ uint8_t* param_addr = reinterpret_cast<__ubuf__ uint8_t*>(print_tlv + 1);
    __ubuf__ uint8_t* fmt_addr = param_addr + print_tlv->fmtOffset - sizeof(uint64_t);

    copy_fmt_to_ubuf(fmt_addr, fmt, str_len);

    uint32_t str_param_offset = print_tlv->fmtOffset + str_len;
    set_param_vf(param_addr, 0, str_param_offset, args...);
}

template <class... Args>
__simd_callee__ inline void scalar_printf_impl(DumpType debug_type, __ubuf__ const char* fmt, Args&&... args)
{
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);

    uint32_t args_num = 0;
    const uint32_t tlv_len = get_print_tlv_len_simd(args_num, fmt, args...);

    // construct PrintTlv TLV in BlockVFBufInfo.buffer (UBuf)
    __ubuf__ PrintTlv* print_tlv =
        reinterpret_cast<__ubuf__ PrintTlv*>((__ubuf__ uint8_t*)(block_info->buffer) + block_info->writeLen);
    set_print_tlv_info_vf(debug_type, print_tlv, tlv_len, args_num, block_info->blockIdx);
    set_print_tlv_data_vf(print_tlv, fmt, args...);

    block_info->magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    block_info->writeLen += tlv_len;
    block_info->pidx += 1;
}

template <class... Args>
__simd_callee__ inline void printf_impl(__ubuf__ const char* fmt, Args&&... args)
{
    enable_asc_diagnostics();
    scalar_printf_impl(DumpType::DUMP_SCALAR, fmt, args...);
}
} // namespace __asc_simd_vf

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_PRINTF_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_PRINTF_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_PRINTF_UTILS_H
