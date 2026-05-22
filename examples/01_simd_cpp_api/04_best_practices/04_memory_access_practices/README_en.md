# Memory Access Practices Example Introduction

## Overview

This section contains optimization examples based on data transfer APIs. Through the `<<<>>>` direct call implementation, it introduces methods such as reducing invalid data transfers and reducing the number of transfer instructions.

## Example List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [bank_conflict](./bank_conflict) |  Bank conflict performance optimization example based on Ascend C. |  |
| [gm_address_conflict](./gm_address_conflict) |  This example introduces the impact of same-address conflicts and two solutions, and provides kernel direct call methods. | Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [optimize_datacopy](./optimize_datacopy) |  Optimization example based on data transfer API usage. Through the `<<<>>>` direct call implementation, it introduces methods such as reducing invalid data transfers and reducing the number of transfer instructions. |  |