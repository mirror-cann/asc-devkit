# Bank Conflict Operator Direct Invocation Example

## Overview
This directory provides bank conflict tuning examples, including bank conflict caused by ND2NZ data rearrangement, and provides a kernel function direct invocation method.

## Example List
| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [bank_conflict_nd2nz](./bank_conflict_nd2nz) |  This example introduces the implementation of avoiding bank conflict by adjusting UB write stride during ND matrix to NZ layout conversion on Atlas A2/A3 series products and Ascend 950PR/Ascend 950DT, and provides a kernel function direct invocation method | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |