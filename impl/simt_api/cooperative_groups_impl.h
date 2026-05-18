/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file cooperative_groups_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__
#warning "impl/simt_api/cooperative_groups_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"simt_api/cooperative_groups.h\" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H
#define IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H

namespace cooperative_groups {

template <typename T, typename... Args>
constexpr bool SupportTypeSimtInternel = (std::is_same_v<T, Args> || ...);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __trap_internal()
{
    *((uint8_t*)-1) = 0;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __fns_internal(unsigned int mask, unsigned int base, int offset)
{
    unsigned int temp_mask = mask;
    int temp_offset = offset;
    if (offset == 0) {
        temp_mask &= (1 << base);
        temp_offset = 1;
    } else if (offset < 0) {
        temp_mask = __brev(mask);
        base = 31 - base;
        temp_offset = -offset;
    }

    temp_mask = temp_mask & ((~0U) << base);
    if (__popc(temp_mask) < temp_offset) {
        return -1;
    }
    int total = 0;
    for (int i = 0x10; i > 0; i >>= 1) {
        unsigned int temp_mask_lo = temp_mask & ((1U << i) - 1);
        int pcnt = __popc(temp_mask_lo);
        if (pcnt < temp_offset) {
            temp_mask = temp_mask >> i;
            temp_offset -= pcnt;
            total += i;
        } else {
            temp_mask = temp_mask_lo;
        }
    }
    if (offset < 0) {
        return 31 - total;
    } else {
        return total;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group::thread_group(group_type type) : _type(type) {}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline group_type thread_group::get_type() const { return _type; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::size() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            return static_cast<const thread_block*>(this)->size();
        }
        case group_type::coalesced_group_type: {
            return static_cast<const coalesced_group*>(this)->size();
        }
        default: {
            return 0;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::num_threads() const
{
    return size();
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::thread_rank() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            return static_cast<const thread_block*>(this)->thread_rank();
        }
        case group_type::coalesced_group_type: {
            return static_cast<const coalesced_group*>(this)->thread_rank();
        }
        default: {
            return 0;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void thread_group::sync() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            static_cast<const thread_block*>(this)->sync();
            break;
        }
        case group_type::coalesced_group_type: {
            static_cast<const coalesced_group*>(this)->sync();
            break;
        }
        default: {
            break;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block::thread_block() : thread_group(group_type::thread_block_type) {}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void thread_block::sync() { asc_syncthreads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::thread_rank()
{
    return threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::group_index()
{
    return dim3(blockIdx.x, blockIdx.y, blockIdx.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::thread_index()
{
    return dim3(threadIdx.x, threadIdx.y, threadIdx.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::dim_threads()
{
    return dim3(blockDim.x, blockDim.y, blockDim.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::num_threads()
{
    return blockDim.x * blockDim.y * blockDim.z;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::size() { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::group_dim() { return dim_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ thread_group thread_block::create_tiled_group(unsigned int tile_size) const
{
    const bool pow2 = ((tile_size & (tile_size - 1)) == 0);
    if (tile_size == 0 || tile_size > warpSize || !pow2) {
        __trap_internal();
    }

    unsigned int block_size = num_threads();
    unsigned int rank = thread_rank();
    unsigned int partition = (block_size + tile_size - 1) / tile_size;
    unsigned int tail = (partition * tile_size) - block_size;
    unsigned int partition_size = tile_size;
    if (rank >= (partition - 1) * tile_size) {
        partition_size -= tail;
    }

    unsigned int mask = static_cast<unsigned int>(-1) >> (warpSize - partition_size);
    mask <<= laneid() & ~(tile_size - 1);
    thread_group tiled_group = thread_group(group_type::coalesced_group_type);
    tiled_group._tiled_info.is_tiled = true;
    tiled_group._tiled_info.mask = mask;
    tiled_group._tiled_info.num_threads = partition_size;
    tiled_group._tiled_info.meta_group_rank = rank / tile_size;
    tiled_group._tiled_info.meta_group_size = partition;
    return tiled_group;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block() { return thread_block(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::_packLanes(unsigned int laneMask) const
{
    unsigned int member_pack = 0;
    unsigned int member_rank = 0;
    for (int bit_idx = 0; bit_idx < 32; bit_idx++) {
        unsigned int lane_bit = _tiled_info.mask & (1U << bit_idx);
        if (lane_bit) {
            if (laneMask & lane_bit) {
                member_pack |= 1U << member_rank;
            }
            member_rank++;
        }
    }
    return member_pack;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_group::create_tiled_group(unsigned int tile_size) const
{
    const bool pow2 = ((tile_size & (tile_size - 1)) == 0);
    if (tile_size == 0 || (tile_size > warpSize) || !pow2) {
        __trap_internal();
        return (coalesced_group(0));
    }
    if (size() <= tile_size) {
        return (*this);
    }

    if (_tiled_info.is_tiled) {
        unsigned int base_offset = (thread_rank() & (~(tile_size - 1)));
        unsigned int mask_length = min(static_cast<unsigned int>(num_threads()) - base_offset, tile_size);
        unsigned int mask = static_cast<unsigned int>(-1) >> (32 - mask_length);
        mask <<= (laneid() & ~(tile_size - 1));
        coalesced_group coalesced_tile = coalesced_group(mask);
        coalesced_tile._tiled_info.is_tiled = true;
        coalesced_tile._tiled_info.meta_group_rank = thread_rank() / tile_size;
        coalesced_tile._tiled_info.meta_group_size = num_threads() / tile_size;
        return coalesced_tile;
    } else {
        unsigned int mask = 0;
        unsigned int member_rank = 0;
        int seen_lanes = (thread_rank() / tile_size) * tile_size;

        for (unsigned int bit_idx = 0; bit_idx < 32; bit_idx++) {
            unsigned int lane_bit = _tiled_info.mask & (1U << bit_idx);
            if (lane_bit) {
                if (seen_lanes <= 0 && member_rank < tile_size) {
                    mask |= lane_bit;
                    member_rank++;
                }
                seen_lanes--;
            }
        }
        coalesced_group coalesced_tile = coalesced_group(mask);
        coalesced_tile._tiled_info.meta_group_rank = thread_rank() / tile_size;
        coalesced_tile._tiled_info.meta_group_size = (num_threads() + tile_size - 1) / tile_size;
        return coalesced_tile;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group::coalesced_group(unsigned int mask)
    : thread_group(group_type::coalesced_group_type)
{
    _tiled_info.is_tiled = false;
    _tiled_info.mask = mask;
    _tiled_info.num_threads = __popc(mask);
    _tiled_info.meta_group_rank = 0;
    _tiled_info.meta_group_size = 1;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void coalesced_group::sync() const { asc_threadfence_block(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::get_mask() const { return _tiled_info.mask; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long coalesced_group::num_threads() const
{
    return static_cast<unsigned long long>(_tiled_info.num_threads);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long coalesced_group::size() const { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long coalesced_group::thread_rank() const
{
    return static_cast<unsigned long long>(__popc(_tiled_info.mask & lanemask_lt()));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long coalesced_group::meta_group_size() const
{
    return static_cast<unsigned long long>(_tiled_info.meta_group_size);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long coalesced_group::meta_group_rank() const
{
    return static_cast<unsigned long long>(_tiled_info.meta_group_rank);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl(T var, int src_rank) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    int lane = src_rank % static_cast<int>(num_threads());
    if (num_threads() != warpSize) {
        lane = __fns_internal(_tiled_info.mask, 0, src_rank + 1);
    }
    return asc_shfl(var, lane, warpSize);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl_up(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    if (num_threads() == warpSize) {
        return asc_shfl_up(var, delta, warpSize);
    }

    int lane = static_cast<int>(__fns_internal(_tiled_info.mask, laneid(), -(delta + 1)));
    if (lane == -1) {
        lane = static_cast<int>(laneid());
    }

    return asc_shfl(var, lane, warpSize);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl_down(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    if (num_threads() == warpSize) {
        return asc_shfl_down(var, delta, warpSize);
    }

    int lane = static_cast<int>(__fns_internal(_tiled_info.mask, laneid(), delta + 1));
    if (lane == -1) {
        lane = static_cast<int>(laneid());
    }

    return asc_shfl(var, lane, warpSize);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int coalesced_group::any(int predicate) const
{
    return (asc_ballot(predicate) & _tiled_info.mask) != 0;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int coalesced_group::all(int predicate) const
{
    return (asc_ballot(predicate) & _tiled_info.mask) == _tiled_info.mask;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::ballot(int predicate) const
{
    if (num_threads() == warpSize) {
        return asc_ballot(predicate);
    }
    unsigned int lane_ballot = asc_ballot(predicate) & _tiled_info.mask;
    return _packLanes(lane_ballot);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_threads() { return coalesced_group(asc_activemask()); }

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_base<Size>::build_mask()
{
    unsigned int mask = static_cast<unsigned int>(-1);
    if (numThreads != warpSize) {
        unsigned int lane_id = laneid();
        mask = th::tile_mask << (lane_id & ~(th::lane_mask));
    }
    return mask;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void thread_block_tile_base<Size>::sync() const
{
    asc_threadfence_block();
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_block_tile_base<Size>::thread_rank()
{
    return laneid() & (Size - 1);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned long long thread_block_tile_base<Size>::num_threads()
{
    return numThreads;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned long long thread_block_tile_base<Size>::size()
{
    return numThreads;
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T thread_block_tile_base<Size>::shfl(T var, int src_rank) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    return asc_shfl(var, src_rank, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T thread_block_tile_base<Size>::shfl_up(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    return asc_shfl_up(var, delta, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T thread_block_tile_base<Size>::shfl_down(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    return asc_shfl_down(var, delta, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T thread_block_tile_base<Size>::shfl_xor(T var, unsigned int lane_mask) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2.");
    return asc_shfl_xor(var, static_cast<int32_t>(lane_mask), static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int thread_block_tile_base<Size>::any(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    return (lane_ballot != 0);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int thread_block_tile_base<Size>::all(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    return (lane_ballot == build_mask());
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_base<Size>::ballot(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    unsigned int shift = laneid() & (~(Size - 1));
    return lane_ballot >> shift;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl<Size, ParentT>::thread_block_tile_impl()
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl<Size, ParentT>::thread_block_tile_impl(unsigned int, unsigned int)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int thread_block_tile_impl<Size, ParentT>::meta_group_rank() const
{
    return ParentT::thread_rank() / Size;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int thread_block_tile_impl<Size, ParentT>::meta_group_size() const
{
    return (ParentT::size() + Size - 1) / Size;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl<Size, void>::thread_block_tile_impl(
    unsigned int meta_group_rank, unsigned int meta_group_size)
    : thread_group(group_type::tiled_group_type)
{
    _tiled_info.is_tiled = true;
    _tiled_info.num_threads = Size;
    _tiled_info.mask = thread_block_tile_base<Size>::build_mask();
    _tiled_info.meta_group_rank = meta_group_rank;
    _tiled_info.meta_group_size = meta_group_size;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_block_tile_impl<Size, void>::size() const
{
    return num_threads();
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_block_tile_impl<Size, void>::num_threads() const
{
    return _tiled_info.num_threads;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_block_tile_impl<Size, void>::thread_rank() const
{
    return thread_block::thread_rank() & (_tiled_info.num_threads - 1);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int thread_block_tile_impl<Size, void>::meta_group_rank() const
{
    return _tiled_info.meta_group_rank;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int thread_block_tile_impl<Size, void>::meta_group_size() const
{
    return _tiled_info.meta_group_size;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT>::thread_block_tile(const ParentT& g)
    : thread_block_tile_impl<Size, ParentT>()
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile<Size, ParentT>::operator thread_block_tile<Size, void>() const
{
    return thread_block_tile<Size, void>(*this);
}

template <unsigned int Size>
template <unsigned int OtherSize, typename OtherParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, void>::thread_block_tile(
    const thread_block_tile<OtherSize, OtherParentT>& g)
    : thread_block_tile_impl<Size, void>(g.meta_group_rank(), g.meta_group_size())
{}

template <unsigned int Size>
template <typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, void>::thread_block_tile(
    const thread_block_tile<Size, ParentT>& g)
    : thread_block_tile_impl<Size, void>(g.meta_group_rank(), g.meta_group_size())
{}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ tiled_partition_impl<Size, thread_block>::tiled_partition_impl(const thread_block& g)
    : thread_block_tile<Size, thread_block>(g)
{}

template <unsigned int Size, unsigned int ParentSize, typename GrandParent>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl<Size, thread_block_tile<ParentSize, GrandParent>>::
    tiled_partition_impl(const thread_block_tile<ParentSize, GrandParent>& g)
    : thread_block_tile<Size, thread_block_tile<ParentSize, GrandParent>>(g)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
{
    return tiled_partition_impl<Size, ParentT>(g);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
{
    if (parent.get_type() == group_type::coalesced_group_type) {
        const coalesced_group* _cg = static_cast<const coalesced_group*>(&parent);
        return _cg->create_tiled_group(tilesz);
    } else {
        const thread_block* _tb = static_cast<const thread_block*>(&parent);
        return _tb->create_tiled_group(tilesz);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
{
    return parent.create_tiled_group(tilesz);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group tiled_partition(
    const coalesced_group& parent, unsigned int tilesz)
{
    return parent.create_tiled_group(tilesz);
}

class _coalesced_group_data_access {
public:
    template <typename TyGroup>
    __SIMT_DEVICE_FUNCTIONS_DECL__ static unsigned int get_mask(const TyGroup& group)
    {
        return group.get_mask();
    }

    __SIMT_DEVICE_FUNCTIONS_DECL__ static coalesced_group construct_result(unsigned int mask)
    {
        return coalesced_group(mask);
    }

    __SIMT_DEVICE_FUNCTIONS_DECL__ static void modify_meta_group(
        coalesced_group& group, unsigned int mgRank, unsigned int mgSize)
    {
        group._tiled_info.meta_group_rank = mgRank;
        group._tiled_info.meta_group_size = mgSize;
    }
};

template <typename TyGroup>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group __binary_partition_internal(const TyGroup& g, bool pred)
{
    unsigned int group_mask = _coalesced_group_data_access::get_mask(g);
    unsigned int set_mask = asc_ballot(pred) & group_mask;

    if (set_mask == group_mask || set_mask == 0) {
        coalesced_group result = _coalesced_group_data_access::construct_result(group_mask);
        _coalesced_group_data_access::modify_meta_group(result, 0, 1);
        return result;
    }

    unsigned int pred_mask = pred ? 0 : 0xFFFFFFFF;
    unsigned int sub_mask = group_mask & (set_mask ^ pred_mask);
    coalesced_group result = _coalesced_group_data_access::construct_result(sub_mask);
    _coalesced_group_data_access::modify_meta_group(result, pred ? 1 : 0, 2);
    return result;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(const coalesced_group& g, bool pred)
{
    return __binary_partition_internal(g, pred);
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(
    const thread_block_tile<Size, ParentT>& g, bool pred)
{
    return __binary_partition_internal(g, pred);
}

} // namespace cooperative_groups

#endif // IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__
#endif
