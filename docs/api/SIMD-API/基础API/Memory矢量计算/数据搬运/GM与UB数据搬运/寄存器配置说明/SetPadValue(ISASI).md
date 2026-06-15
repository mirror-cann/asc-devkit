# SetPadValue\(ISASI\)<a name="ZH-CN_TOPIC_0000001786582454"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持  |
| :----------------------- | :------: |
|<cann-filter npu-type="950"> Ascend 950PR/Ascend 950DT | √</cann-filter>|
|<cann-filter npu-type="A3"> Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √</cann-filter>|
|<cann-filter npu-type="910b"> Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √</cann-filter>|
|<cann-filter npu-type="310b"> Atlas 200I/500 A2 推理产品 | x</cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品AI Core | x</cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品Vector Core | x</cann-filter>|
|<cann-filter npu-type="910"> Atlas 训练系列产品 | x</cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

[DataCopyPad](../GMToUB非对齐数据搬运(DataCopyPad).md)从Global Memory将数据非对齐搬运至Unified Buffer时，可根据开发者的需要自行填充数据。SetPadValue用于设置DataCopyPad需要填充的数值。

具体支持的数据通路为（以[逻辑位置TPosition](../../../../数据结构/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, TPosition pos = TPosition::MAX>
__aicore__ inline void SetPadValue(T paddingValue)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| T | 输入 | 填充值的数据类型，与DataCopyPad接口搬运的数据类型一致。 |
| pos | 输入 | 用于指定DataCopyPad接口搬运过程中从Global Memory搬运数据到哪一个目的地址，目的地址通过逻辑位置来表达。支持的取值为：TPosition::VECIN、TPosition::VECOUT、TPosition::MAX，默认值为TPosition::MAX，等效于TPosition::VECIN或TPosition::VECOUT。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| paddingValue | 输入 | DataCopyPad接口填充的数值，数据类型与DataCopyPad接口搬运的数据类型一致。 |

## 数据类型

<cann-filter npu-type="950">

- Ascend 950PR/Ascend 950DT，支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

</cann-filter>

<cann-filter npu-type="A3">

- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

<cann-filter npu-type="910b">

- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

</cann-filter>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section1227835243314"></a>

```cpp
// 场景1：使用SetPadValue进行自定义填充。
// DataCopyExtParams参数：blockCount=1, blockLen=20*sizeof(half), srcBlkStride=0, srcRepStride=0, reserved=0。
// DataCopyPadExtParams参数：isPad=false, leftPadding=0, rightPadding=12。
// SetPadValue设置填充值为1，配合isPad=false使用。
AscendC::DataCopyExtParams copyParams{1, srcCols * sizeof(T), 0, 0, 0};
AscendC::DataCopyPadExtParams<T> padParams;
padParams.isPad = false;
padParams.leftPadding = 0;
padParams.rightPadding = dstCols - srcCols;

AscendC::SetPadValue((T)1);
AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
```

完整样例请参考[DataCopyPad样例场景1](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm)。
