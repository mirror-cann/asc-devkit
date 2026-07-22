# Matrix Compute API Sample Introduction

## Overview

This sample set introduces typical usage of different features of matrix computation APIs, providing corresponding end-to-end implementations. Samples in the directory are named according to "API_name_pathway", as follows:
1. **load_data**: Samples starting with "load_data" introduce related APIs for L1 Buffer -> L0 Buffer;
2. **mmad**: Samples starting with "mmad" introduce matrix multiplication Mmad API;
3. **fixpipe**: Samples starting with "fixpipe" introduce related APIs for L0C Buffer → Global Memory/L1 Buffer/Unified Buffer;

## Sample List

| Directory Name | Function Description | Supported Products |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [mmad](./mmad) |  This example uses int8_t and bfloat16 as two input data types to demonstrate how to implement matrix multiplication (C = A x B + Bias) through the C_API | Ascend 950PR/Ascend 950DT |
