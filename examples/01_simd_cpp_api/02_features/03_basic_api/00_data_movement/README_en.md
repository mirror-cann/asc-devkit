# Data Movement API Sample Introduction

## Overview

This directory contains samples for multiple APIs related to data movement. Each sample is based on Ascend C's <<<>>> direct call method, supporting implementation of main function and kernel function in the same cpp file.

## Sample List

| Directory Name | Function Description |
| ----------------------------------------------------------| -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [broadcast_ub2l0c](./broadcast_ub2l0c) | This sample implements data broadcast movement based on BroadCastVecToMM, broadcasting data located on UB (Unified Buffer) and moving it to CO1 (L0C Buffer) |
| [copy_ub2ub](./copy_ub2ub) | This sample implements data movement based on Copy, applicable for data movement between VECIN and VECOUT, supporting mask continuous mode and counter mode |
| [data_copy_gm2ub_slice](./data_copy_gm2ub_slice) | This sample implements data slice movement based on DataCopy, extracting subsets of multi-dimensional Tensor data for movement between GM (Global Memory) and UB (Unified Buffer) pathways |
| [data_copy_gm2ub_nddma](./data_copy_gm2ub_nddma) | This sample introduces how to use multi-dimensional data movement interface to implement data movement from GM (Global Memory) to UB (Unified Buffer) pathway. By freely configuring dimension information and corresponding Stride, it can be used for Padding, Transpose, BroadCast, Slice and other data transformation operations |
| [data_copy_l0c2gm](./data_copy_l0c2gm) | This sample implements data inline quantization activation movement based on DataCopy in convolution scenarios |
| [data_copy_pad_gm2ub_ub2gm](./data_copy_pad_gm2ub_ub2gm) | This sample implements non-32-byte aligned data movement based on DataCopyPad, with data padding |
| [data_copy_ub2l1](./data_copy_ub2l1) | This sample implements data movement from UB (Unified Buffer) to L1 (L1 Buffer) based on DataCopy in Mmad matrix multiplication scenarios |
| [ld_st_reg_mask](./ld_st_reg_mask) | This sample implements UB (Unified Buffer) load/store operations to MaskReg (mask register) using Reg programming interface, and mask-based masked store operations |
| [ld_st_reg_align](./ld_st_reg_align) | This sample implements aligned data movement operations (continuous and non-continuous) from UB (Unified Buffer) to RegTensor (Reg vector computation basic unit) using Reg programming interface |
| [ld_st_reg_unalign](./ld_st_reg_unalign) | This sample implements unaligned data movement operations from UB (Unified Buffer) to RegTensor (Reg vector computation basic unit) using Reg programming interface |
| [gather_ld_reg](./gather_ld_reg) | This sample demonstrates using Gather interface to implement discrete data load, including high-dimensional Gather (source is LocalTensor) and Reg::GatherB (collect by DataBlock) scenarios |
| [scatter_st_reg](./scatter_st_reg) | This sample demonstrates using Reg::Scatter interface to implement discrete data store (scatter elements to UB) |
| [auxscalar_reg](./auxscalar_reg) | This sample demonstrates using AuxScalar method to read multiple scalar data from UB for computation |
| [move_reg](./move_reg) | This sample implements data load/store operations from UB (Unified Buffer) to RegTensor using Reg programming interface |