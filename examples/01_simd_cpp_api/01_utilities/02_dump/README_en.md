# Dump Samples

## Overview

This sample shows how to use the Ascend C asc_dump interface series and the simd_vf asc_dump interfaces.

## Sample List

| Directory | Description | Supported Products |
| --- | --- | --- |
| [simple_dump](./simple_dump) | Uses static Tensor programming mode to implement matrix multiplication, showing basic usage of the asc_dump_gm/asc_dump_cbuf/asc_dump_ubuf interfaces; asc_dump_l1buf is demonstrated only on Atlas A2/A3 products | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series/Atlas A3 Inference Series<br>Atlas A2 Training Series/Atlas A2 Inference Series |
| [simd_vf_dump](./simd_vf_dump) | Uses vector programming mode, showing basic usage of the simd_vf asc_dump_ubuf/asc_dump/asc_dump_reg interfaces | Ascend 950PR/Ascend 950DT |
| [special_buffer_dump](./special_buffer_dump) | Implements a HiFloat8 quantized MatMul operator and uses DumpTensor to print the HiFloat8 input matrix, bias, and per-channel scale from L1 Buffer, Bias Table Buffer, and Fixpipe Buffer | Ascend 950PR/Ascend 950DT |
