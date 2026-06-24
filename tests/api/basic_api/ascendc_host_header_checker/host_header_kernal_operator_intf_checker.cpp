/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include "include/basic_api/kernel_operator_intf.h"
static void test_host_kernel_prof_trace_intf()
{
    using AscendC::MetricsProfStart;
    using AscendC::MetricsProfStop;
}

static void test_host_kernel_operator_data_copy_intf()
{
    using AscendC::DataCopy;
    using AscendC::Copy;
    using AscendC::DataCopyPad;
    using AscendC::SetPadValue;
    using AscendC::DataCopyL1ToUB;
    using AscendC::NdDmaDci;
    using AscendC::SetLoopModePara;
    using AscendC::ResetLoopModePara;
}

static void test_host_kernel_operator_fixpipe_intf()
{
    using AscendC::SetFixPipeConfig;
    using AscendC::SetFixpipeNz2ndFlag;
    using AscendC::SetFixpipePreQuantFlag;
    using AscendC::SetFixPipeClipRelu;
    using AscendC::SetFixPipeAddr;
    using AscendC::Fixpipe;
}

static void test_host_kernel_operator_dump_tensor_intf()
{
    using AscendC::DumpTensor;
    using AscendC::DumpAccChkPoint;
}

static void test_host_kernel_operator_mm_intf()
{
    using AscendC::HF32Mode;
    using AscendC::HF32TransMode;
    using AscendC::LoadData;
    using AscendC::LoadDataWithStride;
    using AscendC::LoadDataWithTranspose;
    using AscendC::Mmad;
    using AscendC::MmadMx;
    using AscendC::MmadWithSparse;
    using AscendC::LoadDataWithSparse;
    using AscendC::LoadUnzipIndex;
    using AscendC::BroadCastVecToMM;
    using AscendC::Fill;
    using AscendC::InitConstValue;
    using AscendC::SetLoadDataPaddingValue;
    using AscendC::SetFmatrix;
    using AscendC::SetLoadDataBoundary;
    using AscendC::SetLoadDataRepeat;
    using AscendC::SetLoadDataRepeatWithStride;
    using AscendC::LoadImageToLocal;
    using AscendC::LoadDataUnzip;
    using AscendC::SetHF32Mode;
    using AscendC::SetHF32TransMode;
    using AscendC::SetMMRowMajor;
    using AscendC::SetMMColumnMajor;
    using AscendC::SetMMLayoutTransform;
}

static void test_host_kernel_operator_gemm_intf()
{
    using AscendC::GetGemmTiling;
    using AscendC::Gemm;
}

static void test_host_kernel_operator_conv2d_intf()
{
    using AscendC::GetConv2dTiling;
    using AscendC::Conv2D;
}

static void test_host_kernel_operator_common_intf()
{
    using AscendC::AtomicDtype;
    using AscendC::AtomicOp;
    using AscendC::SetNextTaskStart;
    using AscendC::WaitPreTaskEnd;
    using AscendC::InitSocState;
    using AscendC::InitOutput;
    using AscendC::SetStoreAtomicConfig;
    using AscendC::GetStoreAtomicConfig;
    using AscendC::CheckLocalMemoryIA;
    using AscendC::SetCtrlSpr;
    using AscendC::GetCtrlSpr;
    using AscendC::ResetCtrlSpr;
    using AscendC::SetSaturationFlag;
    using AscendC::GetSaturationFlag;
}

static void test_host_kernel_operator_vec_binary_intf()
{
    using AscendC::Add;
    using AscendC::Sub;
    using AscendC::Mul;
    using AscendC::Div;
    using AscendC::MulAddDst;
    using AscendC::Max;
    using AscendC::Min;
    using AscendC::And;
    using AscendC::Or;
    using AscendC::ShiftLeft;
    using AscendC::ShiftRight;
    using AscendC::AddRelu;
    using AscendC::AddDeqRelu;
    using AscendC::FusedMulAdd;
    using AscendC::MulAddRelu;
    using AscendC::FusedMulAddRelu;
    using AscendC::SubRelu;
    using AscendC::Prelu;
    using AscendC::Mull;
    using AscendC::AbsSub;
    using AscendC::FusedAbsSub;
    using AscendC::ExpSub;
    using AscendC::FusedExpSub;
}

static void test_host_kernel_operator_vec_binary_scalar_intf()
{
    using AscendC::Adds;
    using AscendC::Muls;
    using AscendC::Maxs;
    using AscendC::Mins;
    using AscendC::ShiftLeft;
    using AscendC::ShiftRight;
    using AscendC::LeakyRelu;
    using AscendC::Subs;
    using AscendC::Divs;
    using AscendC::Ands;
    using AscendC::Ors;
    using AscendC::MulsCast;
    using AscendC::FusedMulsCast;
}

static void test_host_kernel_operator_vec_duplicate_intf()
{
    using AscendC::Duplicate;
    using AscendC::Interleave;
    using AscendC::DeInterleave;
}

static void test_host_kernel_operator_vec_gather_mask_intf()
{
    using AscendC::GatherMask;
}

static void test_host_kernel_operator_vec_vconv_intf()
{
    using AscendC::Cast;
    using AscendC::CastDequant;
    using AscendC::CastDeq;
    using AscendC::AddReluCast;
    using AscendC::SubReluCast;
    using AscendC::SetDeqScale;
    using AscendC::Truncate;
}

static void test_host_kernel_operator_scalar_intf()
{
    using AscendC::GetBitCount;
    using AscendC::ScalarGetCountOfValue;
    using AscendC::CountLeadingZero;
    using AscendC::ScalarCountLeadingZero;
    using AscendC::GetUintDivMagicAndShift;
    using AscendC::CountBitsCntSameAsSignBit;
    using AscendC::GetSFFValue;
    using AscendC::ScalarGetSFFValue;
    using AscendC::Cast;
    using AscendC::ScalarCast;
    using AscendC::WriteGmByPassDCache;
    using AscendC::ReadGmByPassDCache;
}

static void test_host_kernel_operator_vec_reduce_intf()
{
    using AscendC::ReduceDataBlock;
    using AscendC::ReducePairElem;
    using AscendC::ReduceRepeat;
    using AscendC::ReduceMax;
    using AscendC::ReduceMin;
    using AscendC::ReduceSum;
    using AscendC::GetReduceRepeatMaxMinSpr;
    using AscendC::GetReduceMaxMinCount;
    using AscendC::GetReduceRepeatSumSpr;
    using AscendC::GetAccVal;
}

static void test_host_kernel_operator_proposal_intf()
{
    using AscendC::MrgSort4;
    using AscendC::RpSort16;
    using AscendC::MrgSort;
    using AscendC::Sort32;
    using AscendC::ProposalConcat;
    using AscendC::ProposalExtract;
    using AscendC::Concat;
    using AscendC::Extract;
    using AscendC::GetSortOffset;
    using AscendC::GetSortLen;
    using AscendC::GetMrgSortResult;
}

static void test_host_kernel_operator_determine_compute_sync_intf()
{
    using AscendC::InitDetermineComputeWorkspace;
    using AscendC::WaitPreBlock;
    using AscendC::NotifyNextBlock;
}

static void test_host_kernel_operator_vec_transpose_intf()
{
    using AscendC::Transpose;
    using AscendC::TransDataTo5HD;
}

static void test_host_kernel_operator_vec_gather_intf()
{
    using AscendC::Gatherb;
    using AscendC::Gather;
}

static void test_host_kernel_operator_vec_scatter_intf()
{
    using AscendC::Scatter;
}

static void test_host_kernel_operator_vec_brcb_intf()
{
    using AscendC::Brcb;
}

static void test_host_kernel_operator_vec_cmpsel_intf()
{
    using AscendC::Compare;
    using AscendC::GetCmpMask;
    using AscendC::SetCmpMask;
    using AscendC::Compares;
    using AscendC::CompareScalar;
    using AscendC::Select;
}

static void test_host_kernel_operator_vec_mulcast_intf()
{
    using AscendC::MulCast;
}

static void test_host_kernel_operator_vec_bilinearinterpolation_intf()
{
    using AscendC::BilinearInterpolation;
}

static void test_host_kernel_operator_vec_createvecindex_intf()
{
    using AscendC::CreateVecIndex;
}

static void test_host_kernel_operator_vec_ternary_scalar_intf()
{
    using AscendC::Axpy;
}

static void test_host_kernel_operator_vec_unary_intf()
{
    using AscendC::Relu;
    using AscendC::Exp;
    using AscendC::Ln;
    using AscendC::Abs;
    using AscendC::Reciprocal;
    using AscendC::Rsqrt;
    using AscendC::Sqrt;
    using AscendC::Not;
    using AscendC::Neg;
}

static void test_host_kernel_operator_vec_vpadding_intf()
{
    using AscendC::VectorPadding;
}

static void test_host_kernel_operator_limits_intf()
{
    using AscendC::NumericLimits;
}

static void test_host_kernel_operator_sys_var_intf()
{
    using AscendC::GetBlockNum;
    using AscendC::GetBlockIdx;
    using AscendC::GetSubBlockIdx;
    using AscendC::GetTaskRatio;
    using AscendC::GetDataBlockSizeInBytes;
    using AscendC::GetArchVersion;
    using AscendC::GetSubBlockNum;
    using AscendC::GetProgramCounter;
    using AscendC::Trap;
    using AscendC::GetSystemCycle;
    using AscendC::GetSpr;
    using AscendC::ClearSpr;
    using AscendC::GetUBSizeInBytes;
    using AscendC::GetVecLen;
    using AscendC::GetRuntimeUBSize;
    using AscendC::GetSsbufBaseAddr;
}

static void test_host_kernel_operator_atomic_intf()
{
    using AscendC::AtomicAdd;
    using AscendC::AtomicMax;
    using AscendC::AtomicMin;
    using AscendC::AtomicCas;
    using AscendC::AtomicExch;
}

static void test_host_kernel_operator_set_atomic_intf()
{
    using AscendC::SetAtomicType;
    using AscendC::SetAtomicAdd;
    using AscendC::DisableDmaAtomic;
    using AscendC::SetAtomicNone;
    using AscendC::SetAtomicMax;
    using AscendC::SetAtomicMin;
}

static void test_host_kernel_operator_cache_intf()
{
    using AscendC::DataCachePreload;
    using AscendC::DataCacheCleanAndInvalid;
    using AscendC::ICachePreLoad;
    using AscendC::GetICachePreloadStatus;
}

static void test_host_kernel_operator_utils_intf()
{
    using AscendC::EngineType;
    using AscendC::Nop;
    using AscendC::Async;
}

static void test_host_kernel_operator_cube_group_intf()
{
    using AscendC::KfcWorkspace;
    using AscendC::CubeResGroupHandle;
    using AscendC::CreateCubeResGroup;
}

static void test_host_kernel_operator_group_barrier_intf()
{
    using AscendC::GroupBarrier;
}

static void test_host_dropout()
{
    using AscendC::DropOut;
}

static void test_host_sigmoid()
{
    using AscendC::Sigmoid;
}

static void test_host_softmax()
{
    using AscendC::SoftMax;
    using AscendC::AdjustSoftMaxRes;
}

static void test_host_simplesoftmax()
{
    using AscendC::SimpleSoftMax;
}

static void test_host_softmaxflashv2()
{
    using AscendC::SoftMaxFlashV2TilingFunc;
    using AscendC::SoftmaxFlashV2;
}

static void test_host_softmaxgrad()
{
    using AscendC::SoftmaxGrad;
    using AscendC::SoftmaxGradFront;
}

static void test_host_arithprogression()
{
    using AscendC::Arange;
}

static void test_host_layernormgrad()
{
    using AscendC::LayerNormGrad;
}

static void test_host_layernormgradbeta()
{
    using AscendC::LayerNormGradBeta;
}

static void test_host_pad()
{
    using AscendC::Pad;
    using AscendC::UnPad;
}

static void test_host_frac()
{
    using AscendC::Frac;
}

static void test_host_power()
{
    using AscendC::Power;
}

static void test_host_log()
{
    using AscendC::Log;
    using AscendC::Log2;
    using AscendC::Log10;
}

static void test_host_sin()
{
    using AscendC::Sin;
}

static void test_host_cos()
{
    using AscendC::Cos;
}

static void test_host_asin()
{
    using AscendC::Asin;
}

static void test_host_acos()
{
    using AscendC::Acos;
}

static void test_host_asinh()
{
    using AscendC::Asinh;
}

static void test_host_acosh()
{
    using AscendC::Acosh;
}

static void test_host_atan()
{
    using AscendC::Atan;
}

static void test_host_cosh()
{
    using AscendC::Cosh;
}

static void test_host_erf()
{
    using AscendC::Erf;
}

static void test_host_erfc()
{
    using AscendC::Erfc;
}

static void test_host_clamp()
{
    using AscendC::ClampMax;
    using AscendC::ClampMin;
    using AscendC::Clamp;
}

static void test_host_rmsnorm()
{
    using AscendC::RmsNorm;
}

static void test_host_batchnorm()
{
    using AscendC::BatchNorm;
}

static void test_host_tanh()
{
    using AscendC::Tanh;
}

static void test_host_atanh()
{
    using AscendC::Atanh;
}

static void test_host_deepnorm()
{
    using AscendC::DeepNorm;
}

static void test_host_exp_adv()
{
    using AscendC::Exp;
}

static void test_host_layernorm()
{
    using AscendC::LayerNorm;
    using AscendC::WelfordUpdate;
}

static void test_host_sum()
{
    using AscendC::Sum;
}

static void test_host_silu()
{
    using AscendC::Silu;
}

static void test_host_gelu()
{
    using AscendC::Gelu;
    using AscendC::FasterGelu;
    using AscendC::FasterGeluV2;
}

static void test_host_ascend_quant()
{
    using AscendC::AscendQuant;
}

static void test_host_ascend_dequant()
{
    using AscendC::AscendDequant;
}

static void test_host_ascend_antiquant()
{
    using AscendC::AscendAntiQuant;
}

static void test_host_logsoftmax()
{
    using AscendC::LogSoftMax;
}

static void test_host_softmaxflash()
{
    using AscendC::SoftmaxFlash;
}

static void test_host_confusion_transpose()
{
    using AscendC::Transpose;
}

static void test_host_selectwithbytesmask()
{
    using AscendC::Select;
}

static void test_host_sinh()
{
    using AscendC::Sinh;
}

static void test_host_swiglu()
{
    using AscendC::SwiGLU;
}

static void test_host_reglu()
{
    using AscendC::ReGlu;
}

static void test_host_tan()
{
    using AscendC::Tan;
}

static void test_host_round()
{
    using AscendC::Round;
}

static void test_host_trunc()
{
    using AscendC::Trunc;
}

static void test_host_swish()
{
    using AscendC::Swish;
}

static void test_host_topk()
{
    using AscendC::TopK;
}

static void test_host_geglu()
{
    using AscendC::GeGLU;
}

static void test_host_lgamma()
{
    using AscendC::Lgamma;
}

static void test_host_digamma()
{
    using AscendC::Digamma;
}

static void test_host_xor()
{
    using AscendC::Xor;
}

static void test_host_sign()
{
    using AscendC::Sign;
}

static void test_host_mean()
{
    using AscendC::Mean;
}

static void test_host_axpy_adv()
{
    using AscendC::Axpy;
}

static void test_host_ceil()
{
    using AscendC::Ceil;
}

static void test_host_floor()
{
    using AscendC::Floor;
}

static void test_host_broadcast()
{
    using AscendC::Broadcast;
    using AscendC::GetBroadcastTilingInfo;
}

static void test_host_reduce_xor_sum()
{
    using AscendC::ReduceXorSum;
}

static void test_host_cumsum()
{
    using AscendC::CumSum;
}