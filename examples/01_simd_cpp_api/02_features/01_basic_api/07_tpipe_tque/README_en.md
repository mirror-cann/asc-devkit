# Resource Management API Examples Introduction

## Overview

This directory contains examples for multiple APIs related to resource management. Each example is based on the Ascend C <<<>>> direct call method, supporting implementation of both the main function and kernel function in the same cpp file.

## Example List

| Directory Name | Description | Supported Products |
| ----------------------------------------------------------- | --------------------------------------------------- | --- |
| [get_tpipe_ptr](./get_tpipe_ptr) |  This example obtains the global TPipe pointer based on GetTPipePtr, allowing the kernel function to perform TPipe-related operations without explicitly passing the TPipe pointer. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [tpipe_reuse](./tpipe_reuse) |  This example implements TPipe repeated allocation and usage based on TPipe::Init and TPipe::Destroy. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |