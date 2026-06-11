# Reg Vector Compute Practices Example Introduction

## Overview

VF-based performance optimization examples using the <<<>>> direct invocation implementation method, introducing VF fusion optimization and loop unrolling optimization methods.

## Example List
| Directory Name | Description | Supported Products |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [gelu_eltwise_high_performance](./gelu_eltwise_high_performance) |  This example uses Gelu+Element-wise computation to introduce RegBase vector performance tuning methods, demonstrating performance gains after parallelism adjustment, loop splitting, and loop unrolling. | Ascend 950PR/Ascend 950DT |
| [gelu_high_performance](./gelu_high_performance) |  This example uses Gelu computation to introduce RegBase vector performance tuning methods, demonstrating performance gains after enabling VF fusion and loop unrolling. | Ascend 950PR/Ascend 950DT |