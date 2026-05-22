# System Variable Access API Samples

## Overview

This directory contains API samples related to system variable access. The samples are based on Ascend C's `<<<>>>` direct invocation method, supporting implementation of both main function and kernel function in the same file.

## Sample List

| Directory Name | Description | Supported Products |
| ------- | -------- | --- |
| [ctrl_spr](./ctrl_spr) |  This sample demonstrates setting, reading, and resetting specific bit fields of the CTRL register (control register) using SetCtrlSpr, GetCtrlSpr, and ResetCtrlSpr interfaces, and verifies whether non-saturation mode is working correctly. | Ascend 950PR/Ascend 950DT |