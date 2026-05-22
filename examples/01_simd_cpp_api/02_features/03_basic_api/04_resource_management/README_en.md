# Resource Management API Examples Introduction

## Overview

This directory contains examples for multiple APIs related to resource management. Each example is based on the Ascend C <<<>>> direct call method, supporting implementation of both the main function and kernel function in the same cpp file.

## Example List

| Directory Name | Description | Supported Products |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [tpipe_reuse](./tpipe_reuse) |  This example implements TPipe repeated allocation and usage based on TPipe::Init and TPipe::Destroy. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [get_tpipe_ptr](./get_tpipe_ptr) |  This example obtains the global TPipe pointer based on GetTPipePtr, allowing the kernel function to perform TPipe-related operations without explicitly passing the TPipe pointer. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [tbufpool_management](./tbufpool_management) |  This example implements TBufPool memory resource management based on TPipe::InitBufPool and TBufPool::InitBufPool interfaces, demonstrating usage patterns such as TBufPool resource allocation, memory partitioning, memory reuse, and custom TBufPool. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [list_tensor_desc_input](./list_tensor_desc_input) |  This example implements an AddN example based on the static Tensor programming model, using the ListTensorDesc structure to handle dynamic input parameters, combined with static memory allocation and event synchronization mechanisms to achieve coordinated scheduling of data transfer and computation tasks. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [tmp_buffer](./tmp_buffer) |  This example initializes TBuf memory space based on the TPipe::InitBuffer interface and uses TBuf temporary buffer for data conversion during computation, implementing a vector addition (Add) example with bfloat16_t data type. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [get_ub_size](./get_ub_size) |  This example demonstrates the usage of GetUBSizeInBytes and GetRuntimeUBSize interfaces, which are used to obtain the maximum user-available UB (Unified Buffer) size in bytes. | Ascend 950PR/Ascend 950DT |