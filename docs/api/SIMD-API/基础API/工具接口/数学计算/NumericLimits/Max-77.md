# Max

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 AI Core | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明

头文件路径为：`"basic_api/kernel_operator_limits_intf.h"`。

返回指定数据类型的最大有限值。

## 函数原型

- 标量接口，返回值为标量：

    ```cpp
    constexpr __aicore__ static inline T Max()
    ```

- 矢量接口，为dst前count个元素赋最大有限值：

    ```cpp
    __aicore__ static inline void Max(const LocalTensor<T>& dst, uint32_t count)
    ```

## 参数说明

**表 1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../../基础数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 输入数据元素个数。 |

## 数据类型

<cann-filter npu-type="950">针对Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。</cann-filter>

## 返回值说明

标量接口返回值为对应数据类型的最大有限值。

矢量接口无返回值。

## 约束说明

无。

## 调用示例

- 标量接口：

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Max();
    ```

- 矢量接口：

    ```cpp
    AscendC::NumericLimits<int32_t>::Max(dstLocal, 256); // 返回256个int32_t类型的最大值
    ```
