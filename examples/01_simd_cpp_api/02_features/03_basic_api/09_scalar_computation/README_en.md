# Scalar Computation API Sample Introduction

## Overview

This directory contains API samples related to scalar computation. The samples are based on the Ascend C `<<<>>>` direct invocation method, supporting the implementation of both the main function and kernel function in the same file.

## Sample List

| Directory Name | Description | Supported Products |
| ------- | -------- | --- |
| [gm_by_pass_dcache](./gm_by_pass_dcache) |  This sample demonstrates reading data from and writing data to GM without going through DCache, based on the ReadGmByPassDcache and WriteGmByPassDcache interfaces. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |