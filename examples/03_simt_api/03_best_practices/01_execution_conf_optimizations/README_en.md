# Execution Configuration Optimizations Sample Introduction

## Overview

Operator execution configuration optimization introduces gridDim/blockDim related configurations in SIMT programming model through direct invocation using <<<>>>. Currently, optimization cases for setting maximum thread blocks are provided to fully utilize registers and optimize operator execution performance.

## Sample List

| Directory Name                                        | Description                                                                                                                                      |
| ----------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| [sincos_compute](./max_thread_config/sincos_compute) | This example uses sincos computation as an example to compare performance differences between using default values and 512 for \_\_launch_bounds__, demonstrating the impact of register spilling on performance in SIMT programming and methods for configuring maximum thread count per block. |
| [grid_dim_config](./grid_dim_config)   | Using the Gather operator as an example, demonstrate the impact of different thread block configuration strategies on operator performance under different data size scenarios and provide corresponding optimization guidance. |
