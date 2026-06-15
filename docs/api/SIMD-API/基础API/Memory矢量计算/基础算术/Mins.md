# Mins<a name="ZH-CN_TOPIC_0000001539843321"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
|---|---|
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √</cann-filter> |
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | √</cann-filter> |
| <cann-filter npu-type="310p">Atlas 推理系列产品AI Core | √</cann-filter> |
| <cann-filter npu-type="310p">Atlas 推理系列产品Vector Core | x</cann-filter> |
| <cann-filter npu-type="910">Atlas 训练系列产品 | x</cann-filter> |
| <cann-filter npu-type="x90">Kirin X90 | √</cann-filter> |
| <cann-filter npu-type="9030">Kirin 9030 | √</cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_binary_scalar_intf.h"`。

Mins属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，Mins负责将矢量tensor内的每个元素与标量进行求最小值。计算公式如下：

$dst_i = \min(src_i, scalarValue)$

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据连续计算

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

dst和src使用[TensorTrait](../../数据结构/辅助数据结构/TensorTrait/TensorTrait.md)类型时，其数据类型TensorTrait和scalarValue的数据类型（对应TensorTrait中的LiteType类型）不一致。因此新增模板类型U表示scalarValue的数据类型，并通过std::enable\_if检查T中萃取出的LiteType和U是否完全一致，一致则接口通过编译，否则编译失败。接口原型定义如下：

- tensor前n个数据计算

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void Mins(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|---|---|
| T | 操作数数据类型。 |
| U | scalarValue的数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，本接口入参中的mask值必须设置为占位符`MASK_PLACEHOLDER`。<br>具体使用方式可参考[掩码](../SIMD计算说明/掩码/掩码.md)。<br><cann-filter npu-type="950,310b">针对以下型号，tensor前n个数据计算API中的isSetMask参数不生效，保持默认值即可。<br>&bull; <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT</cann-filter><br>&bull; <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品</cann-filter></cann-filter> |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。 |
| src | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。 |
| scalarValue | 输入 | 源操作数，数据类型需要与目的操作数中的元素类型保持一致。 |
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体说明请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[UnaryRepeatParams](../../数据结构/辅助数据结构//UnaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

<cann-filter npu-type="950">

- 针对Ascend 950PR/Ascend 950DT，T和U支持的数据类型为：int8_t、uint8_t、int16_t、half、bfloat16_t、int32_t、float、int64_t、uint64_t。数据类型int8_t、uint8_t、int64_t、uint64_t仅支持tensor前n个数据计算接口。

</cann-filter>

<cann-filter npu-type="A3">

- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

<cann-filter npu-type="910b">

- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

<cann-filter npu-type="310b">

- 针对Atlas 200I/500 A2 推理产品，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

<cann-filter npu-type="310p">

- 针对Atlas 推理系列产品AI Core，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

<cann-filter npu-type="x90">

- 针对Kirin X90，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

<cann-filter npu-type="9030">

- 针对Kirin 9030，T和U支持的数据类型为：int16_t、half、int32_t、float。

</cann-filter>

## 返回值说明<a name="section194321251175110"></a>

无

## 约束说明<a name="section199031843133716"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md)。

<cann-filter npu-type="A3,910b">

- 针对如下型号，当参数count或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。

  <cann-filter npu-type="A3">

  - Atlas A3 训练系列产品/Atlas A3 推理系列产品

  </cann-filter>
  <cann-filter npu-type="910b">

  - Atlas A2 训练系列产品/Atlas A2 推理系列产品

  </cann-filter>

</cann-filter>

## 调用示例<a name="section633mcpsimp"></a>

- tensor高维切分计算样例-mask连续模式

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Mins(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor高维切分计算样例-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Mins(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- tensor前n个数据计算接口样例

    ```cpp
    int16_t scalar = 2;
    AscendC::Mins(dstLocal, srcLocal, scalar, 512);
    ```

结果示例如下：

```bash
输入数据src0Local：[1 2 3 ... 512]
输入数据scalar = 2
输出数据dstLocal：[1 2 2 ... 2]
```
