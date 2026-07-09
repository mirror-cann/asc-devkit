/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_aicore_dump_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H

#include "impl/utils/debug/asc_debug_utils.h"
#include "impl/utils/sys_macros.h"

namespace __asc_aicore {
constexpr uint32_t ASC_DEBUG_BUS_UNLOCK_OFFSET = 0x078;
constexpr uint32_t ASC_DEBUG_BUS_CTRL_OFFSET = 0x100;
constexpr uint32_t ASC_DEBUG_BUS_ADDR_LOW_OFFSET = 0x108;
constexpr uint32_t ASC_DEBUG_BUS_MODEL_BASE_OFFSET = 0x10c;
constexpr uint32_t ASC_DEBUG_BUS_DATA_OFFSET = 0x110;
constexpr uint32_t ASC_DEBUG_BUS_BUSY_OFFSET = 0x130;
constexpr uint32_t ASC_DEBUG_BUS_UNLOCK_VALUE = 0x19101920;
constexpr uint32_t ASC_DEBUG_BUS_READ_MASK = 0x2;
constexpr uint32_t ASC_DEBUG_BUS_CHUNK_WORDS = 8;
constexpr uint32_t ASC_DEBUG_BUS_CHUNK_BYTES = ASC_DEBUG_BUS_CHUNK_WORDS * sizeof(uint32_t);

template <AscendC::Hardware hardware>
__aicore__ constexpr inline uint32_t get_debug_bus_model_base()
{
    if constexpr (hardware == AscendC::Hardware::L1) {
        return 0x620000U;
    } else if constexpr (hardware == AscendC::Hardware::L0A) {
        return 0x410000U;
    } else if constexpr (hardware == AscendC::Hardware::L0B) {
        return 0x420000U;
    } else if constexpr (hardware == AscendC::Hardware::L0C) {
        return 0x430000U;
    } else {
        return 0U;
    }
}

__aicore__ inline void debug_bus_write_reg(uint64_t debugBusAddr, uint16_t offset, uint32_t value)
{
    dsb((mem_dsb_t)0U);
    st_dev(value, (__gm__ uint32_t*)(debugBusAddr + offset), 0);
    dsb((mem_dsb_t)0U);
}

__aicore__ inline volatile uint32_t debug_bus_read_reg(uint64_t debugBusAddr, uint16_t offset)
{
    dsb((mem_dsb_t)0U);
    volatile uint32_t value = ld_dev((__gm__ uint32_t*)(debugBusAddr + offset), 0);
    dsb((mem_dsb_t)0U);
    return value;
}

__aicore__ inline void debug_bus_read_chunk(
    const uint64_t debugBusAddr, uint32_t localOffset, const uint32_t modelBase, uint64_t dataDst)
{
    constexpr uint32_t max_wait_count = 5;
    constexpr uint64_t wait_time_cycle = 100;
    sync_all();
    debug_bus_write_reg(debugBusAddr, ASC_DEBUG_BUS_UNLOCK_OFFSET, ASC_DEBUG_BUS_UNLOCK_VALUE);
    debug_bus_write_reg(debugBusAddr, ASC_DEBUG_BUS_ADDR_LOW_OFFSET, localOffset);
    debug_bus_write_reg(debugBusAddr, ASC_DEBUG_BUS_MODEL_BASE_OFFSET, modelBase);
    debug_bus_write_reg(debugBusAddr, ASC_DEBUG_BUS_CTRL_OFFSET, ASC_DEBUG_BUS_READ_MASK);
    volatile uint32_t debug_status = 0;
    volatile uint32_t counter = 0;
    while ((debug_status & 0x80000000U) != 0x80000000U) {
        if (counter > max_wait_count) {
            // timeout protection, avoid infinite loop
            break;
        }
        ringbuf_wait_rts_sync();
        sync_all();
        debug_status = debug_bus_read_reg(debugBusAddr, ASC_DEBUG_BUS_BUSY_OFFSET);
        counter++;
    }
    ringbuf_wait_rts_sync<wait_time_cycle>();
    for (uint32_t i = 0; i < ASC_DEBUG_BUS_CHUNK_WORDS; ++i) {
        debug_bus_write_reg(
            dataDst, i * sizeof(uint32_t),
            debug_bus_read_reg(debugBusAddr, ASC_DEBUG_BUS_DATA_OFFSET + i * sizeof(uint32_t)));
    }
    sync_all();
}

__aicore__ inline uint64_t get_debug_bus_addr_impl()
{
    __gm__ DebugBlockHeadInfo* blockInfo = get_block_info();
    return blockInfo == nullptr ? 0U : blockInfo->debugBusAddr;
}

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline uint32_t debug_bus_copy_local_to_gm_impl(
    __gm__ T* dst, U src, const uint32_t alignDumpBytes, const uint64_t debugBusAddr)
{
    if (debugBusAddr == 0U) {
        return 1;
    }

    constexpr uint32_t asc_debug_bus_chunk_shift = 5; // 32 bytes per chunk
    constexpr uint32_t modelBase = get_debug_bus_model_base<hardware>();
    uint64_t dstGlobalAddr = reinterpret_cast<uint64_t>(dst);
    uint64_t curLocalBaseAddr = reinterpret_cast<uint64_t>(src);

    uint64_t srcLocalAddr = 0;
    uint64_t localStep = 0;
    constexpr uint64_t globalStep = ASC_DEBUG_BUS_CHUNK_BYTES;
    if constexpr (hardware == AscendC::Hardware::L1) {
        srcLocalAddr = curLocalBaseAddr >> asc_debug_bus_chunk_shift;
        localStep = 1;
    } else {
        srcLocalAddr = curLocalBaseAddr;
        localStep = ASC_DEBUG_BUS_CHUNK_BYTES;
    }

    const int32_t loopCount = alignDumpBytes >> asc_debug_bus_chunk_shift;

    for (uint32_t idx = 0; idx < loopCount; ++idx) {
        debug_bus_read_chunk(debugBusAddr, srcLocalAddr, modelBase, dstGlobalAddr);
        srcLocalAddr += localStep;
        dstGlobalAddr += globalStep;
    }
    return 0;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_l1buf_to_gm_impl(__gm__ T* dst, __cbuf__ T* src, const uint32_t alignDumpBytes)
{
#if defined(__DAV_CUBE__)
    return debug_bus_copy_local_to_gm_impl<AscendC::Hardware::L1>(dst, src, alignDumpBytes, get_debug_bus_addr_impl());
#else
    return 1;
#endif
}

template <typename T>
__aicore__ inline uint32_t mem_copy_abuf_to_gm_impl(__gm__ T* dst, __ca__ T* src, const uint32_t alignDumpBytes)
{
#if defined(__DAV_CUBE__)
    return debug_bus_copy_local_to_gm_impl<AscendC::Hardware::L0A>(dst, src, alignDumpBytes, get_debug_bus_addr_impl());
#else
    return 1;
#endif
}

template <typename T>
__aicore__ inline uint32_t mem_copy_bbuf_to_gm_impl(__gm__ T* dst, __cb__ T* src, const uint32_t alignDumpBytes)
{
#if defined(__DAV_CUBE__)
    return debug_bus_copy_local_to_gm_impl<AscendC::Hardware::L0B>(dst, src, alignDumpBytes, get_debug_bus_addr_impl());
#else
    return 1;
#endif
}

template <typename T>
__aicore__ inline uint32_t mem_copy_cbuf_to_gm_impl(__gm__ T* dst, __cc__ T* src, const uint32_t& dumpSize)
{
    if ASCEND_IS_NOT_AIC {
        return 1;
    }
    constexpr int32_t blockCube = 16;
    constexpr int32_t defaultOneBlockSize = 256;
    constexpr int32_t srcBurstLenSizeEle = 16;
    constexpr uint16_t b32ByteSize = 4;

    uint16_t align = (dumpSize % defaultOneBlockSize == 0) ? 0 : 1;
    uint16_t countBlks = align + dumpSize / defaultOneBlockSize;
    uint16_t burstLen =
        static_cast<uint16_t>(srcBurstLenSizeEle * srcBurstLenSizeEle * sizeof(float) / ASC_ONE_DATABLOCK_SIZE);
    uint16_t n = countBlks * blockCube;
    uint16_t m = (burstLen * ASC_ONE_DATABLOCK_SIZE / b32ByteSize) / blockCube;
    bool nz2ndEn = true;

    copy_matrix_cc_to_gm(
        (__gm__ float*)dst, (__cc__ float*)src, 0, n, m, m * blockCube, m, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), static_cast<uint8_t>(false), false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, true, false, false, false);
    return 0;
}

template <typename T>
__aicore__ inline uint32_t mem_copy_ub_to_gm_impl(__gm__ T* dst, __ubuf__ T* src, const uint32_t& len)
{
#if defined(__DAV_VEC__)
    constexpr uint8_t byte_32_align = 32;
    constexpr uint32_t blockCount = 1;
    uint32_t blockLen = len;
    constexpr uint32_t dstStride = 0;
    constexpr uint32_t srcStride = 0;

    uint32_t unitOfBytes = byte_32_align;
    uint32_t burstLen = blockLen * unitOfBytes;
    uint32_t srcStride1 = srcStride * byte_32_align + burstLen;
    srcStride1 = div_ceil(srcStride1, byte_32_align) * byte_32_align;
    uint64_t dstStride1 = dstStride * unitOfBytes + burstLen;
    copy_ubuf_to_gm_align_v2(
        (__gm__ void*)dst, (__ubuf__ void*)src, 0, blockCount, burstLen, 0, dstStride1, srcStride1);
    return 0;
#else
    return 1;
#endif
}

} // namespace __asc_aicore

namespace __asc_simd_vf {
enum class DumpTensorPosition : uint16_t { GM = 0, UB, L1, L0A, L0B, L0C, BIAS, FIXBUF, REG, MAX };

template <DumpTensorPosition dumpPosition, typename T, typename U>
__simd_callee__ inline void set_dump_tlv_info_vf(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t desc, uint32_t dump_size,
    uint16_t block_idx)
{
    dump_tlv->type = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    dump_tlv->length = sizeof(DumpTensorTlv) - sizeof(uint32_t[2]) + align_dump_len;
    dump_tlv->tensorAddr = 0U; // set in aicore
    dump_tlv->dataType = static_cast<uint32_t>(get_dump_datatype<T>());
    dump_tlv->desc = desc;
    dump_tlv->blockIdx = block_idx; // set in aicore
    dump_tlv->bufferId = static_cast<uint32_t>(0U);
    dump_tlv->position = static_cast<uint16_t>(dumpPosition);
    dump_tlv->dim = static_cast<uint32_t>(0U);
    for (uint32_t i = 0; i < 8; ++i) {
        dump_tlv->shape[i] = static_cast<uint32_t>(0U);
    }
    dump_tlv->resv1 = static_cast<uint32_t>(0U);
    dump_tlv->dumpSize = dump_size * sizeof(T);
}

template <typename T, typename U>
__simd_callee__ inline void set_dump_tlv_data_vf(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t dump_size)
{
    __ubuf__ T* dump_dst_addr = reinterpret_cast<__ubuf__ T*>(dump_tlv + 1);

    // Scalar copy; performance optimization deferred
    for (uint32_t i = 0; i < dump_size; i++) {
        dump_dst_addr[i] = src[i];
    }
}

template <typename T, typename U>
__simd_callee__ inline void set_dump_tlv_data_reg(
    U& src, __ubuf__ DumpTensorTlv* dump_tlv, uint32_t align_dump_len, uint32_t dump_size)
{
    __ubuf__ T* dump_dst_addr = reinterpret_cast<__ubuf__ T*>(dump_tlv + 1);

    // Store interface simulation, supports unaligned transfer. dump_dst_addr is likely unaligned.
    uint32_t count = dump_tlv->dumpSize / sizeof(T);
    vector_align ureg;
    vstus(ureg, count * 2, (vector_u32&)src, (__ubuf__ uint32_t*&)dump_dst_addr, POST_UPDATE);
    vstas(ureg, (__ubuf__ uint32_t*&)dump_dst_addr, count * 2, POST_UPDATE);
}

template <DumpTensorPosition dumpPosition, typename T, typename U>
__simd_callee__ inline void asc_dump_impl_reg(U& src, uint32_t desc, uint32_t dump_size)
{
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);

    constexpr uint16_t data_block_size = 32;
    uint32_t align_dump_len = align_up(dump_size * sizeof(T), data_block_size);
    uint32_t tlv_len = sizeof(DumpTensorTlv) + align_dump_len;

    __ubuf__ DumpTensorTlv* dump_tlv =
        (__ubuf__ DumpTensorTlv*)((__ubuf__ uint8_t*)(block_info->buffer) + block_info->writeLen);
    set_dump_tlv_info_vf<dumpPosition, T>(src, dump_tlv, align_dump_len, desc, dump_size, block_info->blockIdx);
    set_dump_tlv_data_reg<T>(src, dump_tlv, align_dump_len, dump_size);

    block_info->magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    block_info->writeLen += tlv_len;
    block_info->pidx += 1;
}

template <DumpTensorPosition dumpPosition, typename T, typename U>
__simd_callee__ inline void asc_dump_impl(U& src, uint32_t desc, uint32_t dump_size)
{
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);

    constexpr uint16_t data_block_size = 32;
    uint32_t align_dump_len = align_up(dump_size * sizeof(T), data_block_size);
    uint32_t tlv_len = sizeof(DumpTensorTlv) + align_dump_len;

    __ubuf__ DumpTensorTlv* dump_tlv =
        (__ubuf__ DumpTensorTlv*)((__ubuf__ uint8_t*)(block_info->buffer) + block_info->writeLen);
    set_dump_tlv_info_vf<dumpPosition, T>(src, dump_tlv, align_dump_len, desc, dump_size, block_info->blockIdx);
    set_dump_tlv_data_vf<T>(src, dump_tlv, align_dump_len, dump_size);

    block_info->magic = ASCENDC_SIMD_VF_MAGIC_NUMBER;
    block_info->writeLen += tlv_len;
    block_info->pidx += 1;
}

template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(
    3510, "asc_dump_reg is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)
{
    enable_asc_diagnostics();
    asc_dump_impl_reg<DumpTensorPosition::REG, T>(input, desc, dump_size);
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(
    3510, "asc_dump_ubuf is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::UB, T>(input, desc, dump_size);
}

template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(3510, "asc_dump is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)
{
    enable_asc_diagnostics();
    asc_dump_impl_reg<DumpTensorPosition::REG, T>(input, desc, dump_size);
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "asc_dump is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    enable_asc_diagnostics();
    asc_dump_impl<DumpTensorPosition::UB, T>(input, desc, dump_size);
}
} // namespace __asc_simd_vf

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H
