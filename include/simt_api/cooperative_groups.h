/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_COOPERATIVE_GROUPS_H
#define INCLUDE_SIMT_API_COOPERATIVE_GROUPS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__
#endif

#include "simt_api/common_functions.h"
#include "simt_api/device_types.h"
#include "simt_api/asc_fp16.h"

namespace cooperative_groups {

class _coalesced_group_data_access;

enum class group_type : unsigned int {
    thread_block_type,
    tiled_group_type,
    coalesced_group_type,
};

class thread_group {
public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline group_type get_type() const;

protected:
    group_type _type;

    struct tiled_info {
        bool is_tiled;
        unsigned int mask;
        unsigned int num_threads;
        unsigned int meta_group_rank;
        unsigned int meta_group_size;
    } _tiled_info;

    friend class thread_block;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group(group_type type);
};

class thread_block : public thread_group {
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block();
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ thread_group tiled_partition(const thread_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ thread_group tiled_partition(const thread_block& parent, unsigned int tilesz);

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static void sync();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 group_index();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 thread_index();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 dim_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int size();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 group_dim();

private:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block();
    __SIMT_DEVICE_FUNCTIONS_DECL__ thread_group create_tiled_group(unsigned int tile_size) const;
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block();

template <unsigned int Size, typename ParentT>
class thread_block_tile;

class coalesced_group : public thread_group {
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ coalesced_group coalesced_threads();
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ thread_group tiled_partition(const thread_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ coalesced_group
    tiled_partition(const coalesced_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ coalesced_group binary_partition(const coalesced_group& g, bool pred);
    friend class _coalesced_group_data_access;

    template <unsigned int Size, typename ParentT>
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ coalesced_group
    binary_partition(const thread_block_tile<Size, ParentT>& g, bool pred);

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long meta_group_size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long meta_group_rank() const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl(T var, int src_rank) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_up(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_down(T var, unsigned int delta) const;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int ballot(int predicate) const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group(unsigned int mask);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int get_mask() const;

private:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int _packLanes(unsigned int laneMask) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group create_tiled_group(unsigned int tile_size) const;
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_threads();

template <unsigned int TileCount, unsigned int TileMask, unsigned int LaneMask, unsigned int ShiftCount>
struct _tile_helpers {
    static constexpr unsigned int tile_count = TileCount;
    static constexpr unsigned int tile_mask = TileMask;
    static constexpr unsigned int lane_mask = LaneMask;
    static constexpr unsigned int shift_count = ShiftCount;
};

template <unsigned int>
struct tile_helpers;
template <>
struct tile_helpers<32> : public _tile_helpers<1, 0xFFFFFFFF, 0x1F, 5> {};
template <>
struct tile_helpers<16> : public _tile_helpers<2, 0x0000FFFF, 0x0F, 4> {};
template <>
struct tile_helpers<8> : public _tile_helpers<4, 0x000000FF, 0x07, 3> {};
template <>
struct tile_helpers<4> : public _tile_helpers<8, 0x0000000F, 0x03, 2> {};
template <>
struct tile_helpers<2> : public _tile_helpers<16, 0x00000003, 0x01, 1> {};
template <>
struct tile_helpers<1> : public _tile_helpers<32, 0x00000001, 0x00, 0> {};

template <unsigned int Size>
class thread_block_tile_base {
    using th = tile_helpers<Size>;
    static constexpr unsigned int numThreads = Size;

    template <unsigned int Sz, typename ParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ friend coalesced_group binary_partition(
        const thread_block_tile<Sz, ParentT>& g, bool pred);
    friend class _coalesced_group_data_access;

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned long long thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned long long num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned long long size();

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl(T var, int src_rank) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_up(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_down(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_xor(T var, unsigned int lane_mask) const;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int ballot(int predicate) const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int build_mask();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int get_mask() { return build_mask(); }
};

template <unsigned int Size, typename ParentT = void>
class thread_block_tile_impl : public thread_block_tile_base<Size> {
public:
    using thread_block_tile_base<Size>::thread_rank;
    using thread_block_tile_base<Size>::num_threads;
    using thread_block_tile_base<Size>::size;
    __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int meta_group_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int meta_group_size() const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl();
    __SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl(unsigned int, unsigned int);
};

template <unsigned int Size>
class thread_block_tile_impl<Size, void> : public thread_block_tile_base<Size>, public thread_group {
public:
    using thread_block_tile_base<Size>::sync;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int meta_group_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int meta_group_size() const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ thread_block_tile_impl(
        unsigned int meta_group_rank = 0, unsigned int meta_group_size = 1);
};

template <unsigned int Size, typename ParentT = void>
class thread_block_tile : public thread_block_tile_impl<Size, ParentT> {
public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const ParentT& g);
    __SIMT_DEVICE_FUNCTIONS_DECL__ operator thread_block_tile<Size, void>() const;
};

template <unsigned int Size>
class thread_block_tile<Size, void> : public thread_block_tile_impl<Size, void> {
    template <unsigned int, typename ParentT>
    friend class thread_block_tile;

public:
    template <typename ParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const thread_block_tile<Size, ParentT>& g);

protected:
    template <unsigned int OtherSize, typename OtherParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const thread_block_tile<OtherSize, OtherParentT>& g);
};

template <unsigned int Size, typename ParentT>
struct tiled_partition_impl;

template <unsigned int Size>
struct tiled_partition_impl<Size, thread_block> : public thread_block_tile<Size, thread_block> {
    __SIMT_DEVICE_FUNCTIONS_DECL__ tiled_partition_impl(const thread_block& g);
};

template <unsigned int Size, unsigned int ParentSize, typename GrandParent>
struct tiled_partition_impl<Size, thread_block_tile<ParentSize, GrandParent>>
    : public thread_block_tile<Size, thread_block_tile<ParentSize, GrandParent>> {
    static_assert(Size < ParentSize, "Tile size bigger or equal to the parent group size");
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl(const thread_block_tile<ParentSize, GrandParent>& g);
};

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_group& parent, unsigned int tilesz);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_block& parent, unsigned int tilesz);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group tiled_partition(
    const coalesced_group& parent, unsigned int tilesz);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(const coalesced_group& g, bool pred);

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(
    const thread_block_tile<Size, ParentT>& g, bool pred);

} // namespace cooperative_groups

#include "impl/simt_api/cooperative_groups_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__
#endif

#endif
