# Memory Access Practices Example Introduction

## Overview

This section contains optimization examples based on data transfer APIs. Through the `<<<>>>` direct call implementation, it introduces methods such as reducing invalid data transfers and reducing the number of transfer instructions.

## Example List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [bank_conflict_nd2nz](./bank_conflict_nd2nz) |  This example introduces how to avoid bank conflict by adjusting the UB write stride during ND matrix to NZ layout conversion on Atlas A2/A3 series products and Ascend 950PR/Ascend 950DT, and provides a kernel direct invocation method. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [data_copy](./data_copy) |  This example introduces data transfer practices from Global Memory to UB and from Global Memory to L1, comparing the impact of tile granularity, unaligned data transfer, L2Cache reuse, and same-address access conflict avoidance on DataCopy/DataCopyPad performance. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
