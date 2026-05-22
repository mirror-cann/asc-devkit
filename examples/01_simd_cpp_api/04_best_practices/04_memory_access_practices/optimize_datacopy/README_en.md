# Optimize Datacopy Sample Introduction

## Overview

This sample demonstrates optimization techniques for data copy APIs, implemented through the `<<<>>>` direct invocation method. It covers methods such as reducing invalid data copying and reducing the number of copy instructions.

## Sample List

| Directory Name | Description | Supported Products |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [optimize_datacopy_loop_mode](./optimize_datacopy_loop_mode) |  Use loop mode to reduce the number of DataCopyPad instructions when using the DataCopyPad API. | Ascend 950PR/Ascend 950DT |
| [optimize_datacopy_nddma](./optimize_datacopy_nddma) |  Use nddma copy to reduce the number of copy instructions when performing non-aligned data copying. | Ascend 950PR/Ascend 950DT |
| [optimize_reduce_invalid_datacopy](./optimize_reduce_invalid_datacopy) |  Reduce invalid data copying by setting Compact mode when using the DataCopyPad API. | Ascend 950PR/Ascend 950DT |