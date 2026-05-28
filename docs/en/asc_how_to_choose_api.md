# Ascend C Multi-Level Programming Interface Selection Guide

Welcome to use [Ascend C](https://www.hiascend.com/cann/ascend-c) for Ascend AI processor operator development. Ascend C not only provides **complete programming capabilities for extreme performance** but also enables you to flexibly choose the most suitable API based on project requirements, team skills, and performance goals through multi-level programming API design. You can achieve the best balance between development efficiency and runtime performance.

---

## Design Goals

The design goals of Ascend C can be summarized as **"high performance, completeness, ease of programming, debuggability, and compatibility"**. Through minimal extensions to C/C++ language standards, it supports both pointer-based C language development habits and Tensor-based C++ programming paradigms. Ascend C achieves seamless integration with existing ecosystems while supporting efficient Ascend operator development, ensuring consistent development experience.

We adhere to the following core principles:
- **No silver bullet**: Different scenarios have varying requirements for performance and development efficiency. A single interface cannot achieve optimal adaptation in all scenarios.
- **Progressive learning**: Beginners can start with easy-to-use interfaces for rapid algorithm verification. Experts can drill down for fine-tuning and leverage complex interfaces to fully unleash hardware potential.

## API Levels

Ascend C provides three types of interfaces, all supporting complete low-level programming capabilities:

| API Level | Language | Features | Target Users | Main Purpose |
|---------|------|------|----------|----------|
| **Tpipe/Tque Framework Programming API** | C++ | Based on **Tensor** programming<br>Manages memory and synchronization through Tpipe/Tque framework | Operator library developers | Leverage framework for automatic synchronization and memory management,<br>improve programming ease of use |
| **Basic API** | C++ | Based on **Tensor** programming, provides **C++ complete programming capabilities**<br>Allocates Tensor through MakeTensor / LocalMemoryAllocator, manages synchronization independently | Operator library developers | Independently manage synchronization and memory,<br>match C++ Tensor development habits, support extreme performance |
| **Language Extension Layer<br>SIMD & SIMT API** | C | Based on **pointer** programming, provides **C complete programming capabilities**<br>Allocates memory through array `[]`, manages synchronization independently | Operator library developers | Independently manage synchronization and memory,<br>match C language development habits, support extreme performance |

In addition, Ascend C provides high-level APIs and operator template libraries to further improve operator development efficiency.

| API Level | Target Users | Main Purpose |
|---------|----------|----------|
| **Operator Template Library (CATLASS / ATVOSS, etc.)** | Algorithm developers | Customize and extend based on typical operator implementations for high-performance requirements in specific scenarios |
| **High-level API** | Algorithm developers | Reuse common single-core algorithms for rapid algorithm verification |

---

## How to Quickly Choose the Right API Level?

The following decision flowchart helps you quickly locate the most suitable API level:

> We recommend developing all operators based on **<<<>>> invocation and Host/Device hybrid compilation**

```mermaid
graph TD
    A[**Ascend C Operator Development**] --> B[**1. Ease of use priority, performance insensitive**]
    A --> C[**2. Extreme performance priority**]
    A --> D[**3. Balance performance and ease of use**]
    B -->|Other operator types| E[**SIMD C API**<br>(with sync suffix computation interface)]
    B -->|Familiar with SIMT, discrete vector operators| F[**SIMT API**]
    C -->|Discrete vector operators| F
    C -->|Prefer **pointer programming**| P[**SIMD C API**]
    C -->|Prefer **C++ Tensor programming & independent synchronization/memory management**| I[**Basic API**]
    C -->|Prefer **C++ Tensor programming & automatic synchronization/memory management**| J[**Tpipe/Tque Framework Programming API**]

    D -->|Reuse common algorithms, generalization priority| K[**High-level API**]
    D -->|Typical operators, high performance in specific scenarios| L[**Operator Template Library**]
```

You can also refer to the following key dimensions for quick decision-making:

| Key Factor | Recommended Level | Reason |
|----------|----------|------|
| **Discrete vector operators** | SIMT API | Fully leverage SIMT advantages in discrete scenarios while matching industry programming habits |
| **Pointer-based complete programming capabilities** | SIMD C API | Match C language development habits, support extreme performance |
| **C++ Tensor-based complete programming capabilities** | Basic API | Match C++ Tensor development habits, support extreme performance |
| **Rapid algorithm verification** | High-level API or Operator Template Library | Encapsulates well-generalized implementations of common algorithms, high development efficiency |

---

## Detailed Level Introduction

### Language Extension Layer C API (SIMD & SIMT)

**Features**
- Matches traditional C language operator development habits in the industry. Supports array memory allocation, pointer computation interfaces, and uses `asc_xxx` prefix snake_case naming style.
- SIMT API programming model follows industry-common development practices, reducing learning curve.
- SIMD API provides easy-to-use continuous computation interfaces. It supports most operator development needs, for example, `asc_add(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)`.
- To improve ease of use for beginners, SIMD API simplifies synchronization management. It additionally provides synchronization operation interfaces with `_sync` suffix, such as `asc_add_sync(...)`.
- For extreme performance scenarios, SIMD API provides advanced computation interfaces with `repeat` / `stride` parameters. These support flexible control of data layout and computation mode.

**Applicable Scenarios**
- Operator developers familiar with traditional C language development habits.
- Developers with SIMT programming experience who want to quickly migrate to NPU environments.
- Production environment operator development that requires extracting extreme hardware performance.
- Developers who want to quickly verify algorithms using computation interfaces with `sync` suffix.

**Examples**
- [SIMD Add Operator Example (with Synchronous Computation Interface)](../../examples/02_simd_c_api/00_introduction/01_add/c_api_sync_add/c_api_add.asc)
- [SIMT Gather Operator Example (Matching Industry Habits)](../../examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_1d/gather_1d.asc)
- For more examples, refer to [examples directory](https://gitcode.com/cann/asc-devkit/tree/master/examples)

---

### Basic API: Tensor-based Single Instruction Abstraction

**Features**
- Abstracts NPU instructions based on Tensor and data types, provides Tensor programming model.
- Provides memory allocation and synchronization interfaces independent of `Tque` / `Tpipe`. Developers can manage resources based on Tensor independently.
- Extends Tensor to support `Layout` concept. Simplifies computation interfaces through unified data layout representation, maintaining consistency with industry Tensor programming experience.
- Framework Programming API: Introduces `Tque` / `Tpipe` framework, borrowing C++ `Queue` design concept to simplify NPU synchronization and memory management.

**Applicable Scenarios**
- Operator developers familiar with industry C++ Tensor development habits.
- Scenarios that require developing extreme performance operators in production environments while maintaining code maintainability and extensibility.

**Examples**
- [SIMD Add Operator Example with Tque / Tpipe Automatic Memory and Synchronization Management](../../examples/01_simd_cpp_api/00_introduction/01_vector/add_tpipe_tque/add.asc)
- [SIMD Add Operator Example with LocalMemoryAllocator Independent Memory and Synchronization Management](../../examples/01_simd_cpp_api/00_introduction/01_vector/add/add.asc)
- Tensor API Example Based on Layout (to be added)

---

### High-level API: Single-core Common Algorithm Implementation

**Features**
- Encapsulates common single-core algorithm implementations, provides good generalization performance.
- Can also achieve near-extreme performance in typical network scenarios.

**Applicable Scenarios**
- Quickly verify algorithm feasibility without extreme performance requirements for specific scenarios.
- Scenarios that want to reuse mature algorithm implementations and shorten development cycles.

**Examples**
- [Softmax API Example](../../examples/01_simd_cpp_api/03_libraries/01_activation/softmax/softmax.asc)
- [Matmul API Example](../../examples/01_simd_cpp_api/03_libraries/00_matrix)

---

### Operator Template Library: Operator Implementation Samples

**Features**
- Provides end-to-end complete implementations of typical operators in specific scenarios as best practice references.
- Typically optimized for specific scenarios. Generalization performance is not the primary goal.

**Applicable Scenarios**
- Need to customize and extend typical operators to quickly adapt to specific business scenarios.

**Examples**
- Vector operator template library: [ATVC](https://gitcode.com/cann/atvc), [ATVOSS](https://gitcode.com/cann/atvoss)
- Cube operator template library: [CATLASS](https://gitcode.com/cann/catlass)

---

## Summary

The core concept of Ascend C multi-level interface design is: **always use the most appropriate programming paradigm rather than passively adapting to a single abstraction**. Whether you are a low-level expert pursuing extreme performance or a prototype developer hoping to quickly verify algorithms, you can find handy tools in the Ascend C hierarchical API ecosystem.

Start your operator programming journey now! If you have questions, refer to Ascend C detailed documentation or community examples. We continue to make NPU powerful computing capabilities accessible and efficient for you.
