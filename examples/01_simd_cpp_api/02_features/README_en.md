# Features Sample Introduction

## Overview

This document introduces Ascend C SIMD feature samples, covering compilation projects, operator invocation, framework integration, Basic API, AI CPU, and Aclrtc usage. It demonstrates typical implementation methods and build/run flows for different features.

## Sample List

| Directory Name | Description |
| -------------- | ----------- |
| [00_framework](./00_framework) | This sample introduces custom operator implementation methods for PyTorch, TensorFlow, and ONNX frameworks |
| [01_basic_api](./01_basic_api) | This sample introduces the usage of Ascend C Basic API, including data movement, matrix computation, memory vector computation, resource management, synchronization control, system access, atomic operations, and scalar computation |
| [06_aclrtc](./06_aclrtc) | This sample demonstrates using Aclrtc (runtime compilation) interfaces to compile Ascend C kernel functions in string form on the Host side and complete the loading and execution of compilation results |
| [99_aclnn_op](./99_aclnn_op) | This sample introduces the implementation of custom operator compilation projects and Aclnn/Aclop operator invocation |
