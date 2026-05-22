# Optimize Datacopy样例介绍

## 概述

基于搬运类API使用的优化样例，通过<<<>>>直调的实现方式，介绍了减少无效数据搬运、减少搬运指令数量等方法。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [optimize_datacopy_loop_mode](./optimize_datacopy_loop_mode) |  在使用DataCopyPad接口时，使用loop模式减少DataCopyPad指令的条数。 | Ascend 950PR/Ascend 950DT |
| [optimize_datacopy_nddma](./optimize_datacopy_nddma) |  在进行非对齐数据搬运时，使用nddma搬运减少搬运指令的条数。 | Ascend 950PR/Ascend 950DT |
| [optimize_reduce_invalid_datacopy](./optimize_reduce_invalid_datacopy) |  在使用DataCopyPad接口时，通过设置Compact模式减少无效数据的搬运。 | Ascend 950PR/Ascend 950DT |
