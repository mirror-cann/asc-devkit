# Matrix Compute API Sample Introduction

## Overview

This sample set introduces typical usage of different features of matrix computation APIs, providing corresponding end-to-end implementations. Samples in the directory are named according to "API_name_pathway", as follows:
1. **data_copy**: Samples starting with "data_copy" introduce related APIs for Global Memory -> L1 Buffer;
2. **load_data**: Samples starting with "load_data" introduce related APIs for L1 Buffer -> L0 Buffer;
3. **mmad**: Samples starting with "mmad" introduce matrix multiplication Mmad API;
4. **fixpipe**: Samples starting with "fixpipe" introduce related APIs for L0C Buffer → Global Memory/L1 Buffer/Unified Buffer;

## Sample List

| Directory Name | Function Description |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- |
| [data_copy_gm2l1](./data_copy_gm2l1) | This sample introduces how to use DataCopy to move input data from GM (Global Memory) to L1 (L1 Buffer) |
| [fixpipe_l0c2gm](./fixpipe_l0c2gm) | This sample introduces how to use Fixpipe to move matrix multiplication results from CO1 (L0C Buffer) to GM (Global Memory) |
| [fixpipe_l0c2ub](./fixpipe_l0c2ub) | This sample introduces how to use Fixpipe to move matrix multiplication results from CO1 (L0C Buffer) to UB (Unified Buffer) |
| [fixpipe_l0c2l1](./fixpipe_l0c2l1) | This sample introduces how to use Fixpipe to move matrix multiplication results from CO1 (L0C Buffer) to L1 (L1 Buffer) |
| [load_data_l12l0](./load_data_l12l0) | This sample introduces usage of related instructions in 14 matrix multiplication scenarios with left/right matrix transpose/non-transpose combinations under B4/B8/B16/B32 four data types. Focuses on data movement from A1 to A2 and B1 to B2 using basic API LoadData, including Load2D, Load3D, LoadDataWithTranspose |
| [load_data_2dv2_l12l0](./load_data_2dv2_l12l0) | This sample introduces data movement from A1 to A2 and B1 to B2 using basic API LoadData, including Load2Dv2 |
| [load_data_2dmx_l12l0](./load_data_2dmx_l12l0) | This sample introduces usage of related instructions in 6 quantized matrix multiplication scenarios with left matrix and left quantization matrix, right matrix and right quantization matrix transpose/non-transpose combinations under FP4/FP8 two data types. Focuses on data movement from L1 to L0 for A and scaleA, B and scaleB matrices using basic API LoadData |
| [mmad_load3dv2](./mmad_load3dv2) | This sample introduces the process of moving A and B matrices from L1 to L0A/L0B using LoadData3DV2 instruction, where A and B represent left and right input matrices of matrix multiplication. LoadData3DV2 instruction parameter configuration and matrix data layout changes before and after instruction execution are explained with diagrams |
| [batch_mmad](./batch_mmad) | This sample introduces batch matrix multiplication with float data type input and both left/right matrices non-transposed. DataCopy ND2NZ and Fixpipe batch data movement are used for GM-->L1, L0C-->GM, L0C-->L1 pathways, while L1-->L0A/L0B and Mmad matrix multiplication execution steps loop batch times, each loop processes one pair of left/right matrices |
| [mmad](./mmad) | This sample introduces matrix multiplication with ND format input, B4/B8/B16/B32 input data types (specifically using int4_t/int8_t/bfloat16/float as examples), demonstrating how to implement matrix multiplication computation (C = A x B + Bias) using Mmad instruction |
| [mmad_unitflag](./mmad_unitflag) | This sample introduces how to use unitFlag functionality when calling Mmad instruction |
| [mmad_gemv](./mmad_gemv) | This sample introduces matrix multiplication in Gemv (M=1) mode |
| [mmad_with_sparse](./mmad_with_sparse) | This sample introduces basic API MmadWithSparse invocation |