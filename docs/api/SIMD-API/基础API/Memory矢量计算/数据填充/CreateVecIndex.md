# CreateVecIndex<a name="ZH-CN_TOPIC_0000002573904793"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas 训练系列产品：不支持
<!-- end id15 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_createvecindex_intf.h"`。

CreateVecIndex接口创建指定起始值的向量索引。

接口支持使用前n个数据计算和高维切分计算的方式，前n个数据计算原理和参考伪代码如下：

```python
import numpy as np

def CreateVecIndex(dst, firstValue, count):
    dst = np.arange(firstValue, firstValue + count, dtype=np.float16)
```

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算

    ```cpp
    template <typename T> 
    __aicore__ inline void CreateVecIndex(LocalTensor<T> dst, const T &firstValue, uint32_t count) 
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T> 
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride)  
        ```

    - mask连续模式

        ```cpp
        template <typename T> 
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride) 
        ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 操作数数据类型。 |

**表2**  接口参数说明
| 参数说明 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT（存储位置为Unified Buffer）。LocalTensor的起始地址需要按照32字节对齐。|
| firstValue | 输入 | 索引的第一个数值，数据类型需与dst中元素的数据类型保持一致。|
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。|
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。<br>设置详见[掩码操作](../掩码操作/掩码操作.md)。 |
| repeatTime | 输入 | 指令迭代次数，每次迭代完成8个DataBlock的数据收集，取值范围：repeatTime∈[0,255]。|
| dstBlkStride | 输入 | 单次迭代内，矢量目的操作数不同DataBlock间地址步长，单位为DataBlock。 |
| dstRepStride | 输入 | 相邻迭代间，矢量目的操作数相同DataBlock地址步长，单位为DataBlock。 |

## 数据类型

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、int16_t、half、int32_t、float、int64_t。
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int16_t、half、int32_t、float。
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int16_t、half、int32_t、float。
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 推理产品，支持的数据类型为：int16_t、half、int32_t、float。
<!-- end id4 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- firstValue需保证不超出dst中元素数据类型对应的大小范围。
<!-- npu="A3,910b" id8 -->
- repeatTime = 0表示不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。该约束适用于以下型号。
  <!-- npu="A3" id5 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id5 -->
  <!-- npu="910b" id6 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id6 -->
<!-- end id8 -->

<!-- npu="950" id7 -->
- 针对Ascend 950PR/Ascend 950DT，int8_t/int64_t数据类型仅支持tensor前n个数据计算接口。
<!-- end id7 -->

## 调用示例<a name="section642mcpsimp"></a>

本样中只展示部分关键代码。如果您需要运行样例代码，请将该代码段拷贝并替换[CreateVecIndex样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/create_vec_index)中Compute函数相关代码片段即可。

- tensor高维切分计算样例-mask连续模式

    ```cpp
    // repeatTime = 1, mask = 128, 128 elements one repeat, 128 elements total
    // firstValue数据类型为int16_t，dstLocal数据类型为int16_t
    // dstBlkStride = 1, 单次迭代内数据连续写入
    // dstRepStride = 8, 相邻迭代内数据连续写入
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- tensor高维切分计算样例-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 1, 128 elements one repeat, 128 elements total
    // firstValue数据类型为int16_t，dstLocal数据类型为int16_t
    // dstBlkStride = 1, 单次迭代内数据连续写入
    // dstRepStride = 8, 相邻迭代内数据连续写入
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- tensor前n个数据计算样例

    ```cpp
    uint32_t count = 128;    // 参与计算的元素个数
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, count);
    ```

结果示例如下：

```plain
输入数据（firstValue）：0 
输出数据（dstLocal）：[0 1 2 ... 127]
```
