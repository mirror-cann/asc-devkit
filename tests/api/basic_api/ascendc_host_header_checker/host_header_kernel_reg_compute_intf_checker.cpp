/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include "reg_compute/kernel_reg_compute_intf.h"
static void test_host_kernel_reg_compute_utils()
{
    using AscendC::ExpAlgo;
    using AscendC::LnAlgo;
    using AscendC::LogAlgo;
    using AscendC::Log2Algo;
    using AscendC::Log10Algo;
    using AscendC::ReciprocalAlgo;
    using AscendC::DivAlgo;
    using AscendC::SqrtAlgo;
    using AscendC::RsqrtAlgo;
    using AscendC::ExpConfig;
    using AscendC::LnConfig;
    using AscendC::LogConfig;
    using AscendC::Log2Config;
    using AscendC::Log10Config;
    using AscendC::ReciprocalConfig;
    using AscendC::DivConfig;
    using AscendC::SqrtConfig;
    using AscendC::RsqrtConfig;
    using AscendC::Reg::RegLayout;
    using AscendC::Reg::SatMode;
    using AscendC::Reg::IndexOrder;
    using AscendC::Reg::MaskMergeMode;
    using AscendC::Reg::HistogramsBinType;
    using AscendC::Reg::HistogramsType;
    using AscendC::Reg::HighLowPart;
    using AscendC::Reg::PostLiteral;
    using AscendC::Reg::MaskPattern;
    using AscendC::Reg::LoadDist;
    using AscendC::Reg::MaskDist;
    using AscendC::Reg::StoreDist;
    using AscendC::Reg::MemType;
    using AscendC::Reg::DataCopyMode;
    using AscendC::Reg::GatherMaskMode;
    using AscendC::Reg::StoreMode;
    using AscendC::Reg::RoundControl;
    using AscendC::Reg::PairReduce;
    using AscendC::Reg::DefaultType;
    using AscendC::Reg::CastTrait;
    using AscendC::Reg::ExpSpecificMode;
    using AscendC::Reg::LnSpecificMode;
    using AscendC::Reg::LogSpecificMode;
    using AscendC::Reg::Log2SpecificMode;
    using AscendC::Reg::Log10SpecificMode;
    using AscendC::Reg::DivSpecificMode;
    using AscendC::Reg::SqrtSpecificMode;
}

static void test_host_kernel_reg_compute_maskreg_intf()
{
#if !defined(__NPU_ARCH__)
    using AscendC::Reg::UpdateMask;
    using AscendC::Reg::CreateMask;
    using AscendC::Reg::Not;
    using AscendC::Reg::And;
    using AscendC::Reg::MaskGenWithRegTensor;
    using AscendC::Reg::Or;
    using AscendC::Reg::Xor;
    using AscendC::Reg::Move;
    using AscendC::Reg::Interleave;
    using AscendC::Reg::DeInterleave;
    using AscendC::Reg::Select;
    using AscendC::Reg::Pack;
    using AscendC::Reg::UnPack;
    using AscendC::Reg::MoveMask;
#endif
}

static void test_host_kernel_reg_compute_membar_intf()
{
    using AscendC::Reg::LocalMemBar;
}

static void test_host_kernel_reg_compute_addrreg_intf()
{
    using AscendC::Reg::CreateAddrReg;
}

static void test_host_kernel_reg_compute_vec_duplicate_intf()
{
    using AscendC::Reg::Duplicate;
    using AscendC::Reg::Interleave;
    using AscendC::Reg::DeInterleave;
}

static void test_host_kernel_reg_compute_vec_cmpsel_intf()
{
    using AscendC::Reg::Compare;
    using AscendC::Reg::Compares;
    using AscendC::Reg::Select;
}

static void test_host_kernel_reg_compute_vec_binary_intf()
{
    using AscendC::Reg::Add;
    using AscendC::Reg::Sub;
    using AscendC::Reg::Mul;
    using AscendC::Reg::Div;
    using AscendC::Reg::Max;
    using AscendC::Reg::Min;
    using AscendC::Reg::ShiftLeft;
    using AscendC::Reg::ShiftRight;
    using AscendC::Reg::And;
    using AscendC::Reg::Or;
    using AscendC::Reg::Xor;
    using AscendC::Reg::Prelu;
    using AscendC::Reg::Mull;
    using AscendC::Reg::MulAddDst;
    using AscendC::Reg::AddC;
    using AscendC::Reg::SubC;
}

static void test_host_kernel_reg_compute_vec_binary_scalar_intf()
{
    using AscendC::Reg::Adds;
    using AscendC::Reg::Muls;
    using AscendC::Reg::Maxs;
    using AscendC::Reg::Mins;
    using AscendC::Reg::ShiftLefts;
    using AscendC::Reg::ShiftRights;
    using AscendC::Reg::LeakyRelu;
}

static void test_host_kernel_reg_compute_copy_intf()
{
    using AscendC::Reg::Move;
}

static void test_host_kernel_reg_compute_datacopy_intf()
{
    using AscendC::Reg::LoadAlign;
    using AscendC::Reg::StoreAlign;
    using AscendC::Reg::LoadUnAlignPre;
    using AscendC::Reg::LoadUnAlign;
    using AscendC::Reg::Load;
    using AscendC::Reg::StoreUnAlign;
    using AscendC::Reg::StoreUnAlignPost;
    using AscendC::Reg::Store;
    using AscendC::Reg::Gather;
    using AscendC::Reg::GatherB;
    using AscendC::Reg::Scatter;
}

static void test_host_kernel_reg_compute_gather_mask_intf()
{
    using AscendC::Reg::Squeeze;
    using AscendC::Reg::Unsqueeze;
    using AscendC::Reg::GetSpr;
    using AscendC::Reg::ClearSpr;
    using AscendC::Reg::Gather;
}

static void test_host_kernel_reg_compute_pack_intf()
{
    using AscendC::Reg::Pack;
    using AscendC::Reg::UnPack;
}

static void test_host_kernel_reg_compute_struct_intf()
{
    using AscendC::Reg::RegTrait;
    using AscendC::Reg::RegTensor;
}

static void test_host_kernel_reg_compute_vec_arange_intf()
{
    using AscendC::Reg::Arange;
}

static void test_host_kernel_reg_compute_vec_reduce_intf()
{
    using AscendC::Reg::Reduce;
    using AscendC::Reg::ReduceDataBlock;
    using AscendC::Reg::PairReduceElem;
}

static void test_host_kernel_reg_compute_vec_ternary_scalar_intf()
{
    using AscendC::Reg::Axpy;
}

static void test_host_kernel_reg_compute_vec_unary_intf()
{
    using AscendC::Reg::Abs;
    using AscendC::Reg::Relu;
    using AscendC::Reg::Exp;
    using AscendC::Reg::Sqrt;
    using AscendC::Reg::Ln;
    using AscendC::Reg::Log;
    using AscendC::Reg::Log2;
    using AscendC::Reg::Log10;
    using AscendC::Reg::Neg;
    using AscendC::Reg::Not;
}

static void test_host_kernel_reg_compute_vec_vconv_intf()
{
    using AscendC::Reg::Cast;
    using AscendC::Reg::Truncate;
}

static void test_host_kernel_reg_compute_vec_fused_intf()
{
    using AscendC::Reg::MulsCast;
    using AscendC::Reg::AbsSub;
    using AscendC::Reg::ExpSub;
    using AscendC::Reg::MulDstAdd;
}

static void test_host_kernel_reg_compute_histograms_intf()
{
    using AscendC::Reg::Histograms;
}