# LoadImageToLocal<a name="ZH-CN_TOPIC_0000001945534165"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
|------|:------:|
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | √ </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品AI Core | √ </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

将图像数据从Global Memory搬运到Local Memory。搬运过程中可以完成图像预处理操作：包括图像翻转，改变图像尺寸（抠图，裁边，缩放，伸展），以及色域转换，类型转换等。图像预处理的相关参数通过[SetAippFunctions](SetAippFunctions.md)进行配置。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadImageToLocal(const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>LocalTensor的起始地址需要保证32字节对齐。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadImageToLocalParams。<br>具体定义请参考：\$\{INSTALL_DIR\}/include/ascendc/basic_api/interface/kernel_struct_mm.h，\$\{INSTALL_DIR\}请替换为CANN软件安装后文件存储路径。<br>参数说明参考[表2](#table8955841508)。 |

<a name="table8955841508"></a>
**表 2** LoadImageToLocalParams结构体内参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| horizSize | 输入 | 从源图中加载图片的水平宽度，单位为像素，取值范围：horizSize∈[2, 4095]。 |
| vertSize | 输入 | 从源图中加载图片的垂直高度，单位为像素，取值范围：vertSize∈[2, 4095]。 |
| horizStartPos | 输入 | 加载图片在源图片上的水平起始地址，单位为像素，取值范围：horizStartPos∈[0, 4095]。默认为0。<br>**注意：当输入图片为YUV420SP、XRGB8888、RGB888、YUV400格式时，该参数需要是偶数。** |
| vertStartPos | 输入 | 加载图片在源图片上的垂直起始地址，单位为像素，取值范围：vertStartPos∈[0, 4095]。默认为0。<br>**注意：当输入图片为YUV420SP格式时，该参数需要是偶数。** |
| srcHorizSize | 输入 | 源图像水平宽度，单位为像素，取值范围：srcHorizSize∈[2, 4095]。<br>**注意：当输入图片为YUV420SP格式时，该参数需要是偶数。** |
| topPadSize | 输入 | 目的图像顶部填充的像素数，取值范围：topPadSize∈[0, 32]，默认为0。进行数据填充时使用，需要先调用SetAippFunctions通过AippPaddingParams配置填充的数值，再通过topPadSize、botPadSize、leftPadSize、rightPadSize配置填充的大小范围。 |
| botPadSize | 输入 | 目的图像底部填充的像素数，取值范围：botPadSize∈[0, 32]，默认为0。 |
| leftPadSize | 输入 | 目的图像左边填充的像素数，取值范围：leftPadSize∈[0, 32]，默认为0。 |
| rightPadSize | 输入 | 目的图像右边填充的像素数，取值范围：rightPadSize∈[0, 32]，默认为0。 |
| sid | 输入 | 预留参数。为后续的功能做保留，开发者暂时无需关注，使用默认值即可。 |

## 数据类型

<cann-filter npu-type="950">Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、half。</cann-filter><br>
<cann-filter npu-type="A3">Atlas A3训练系列产品/Atlas A3推理系列产品，支持数据类型：int8_t、half。</cann-filter><br>
<cann-filter npu-type="910b">Atlas A2训练系列产品/Atlas A2推理系列产品，支持数据类型：int8_t、half。</cann-filter><br>
<cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品，支持数据类型为：int8_t、uint8_t、half。</cann-filter><br>
<cann-filter npu-type="310p">Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、half。</cann-filter>

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。
- 加载到dst的图片的大小加padding的大小必须小于等于所在存储空间的大小。
- 当通过[SetAippFunctions](SetAippFunctions.md)配置padding模式为块填充模式或者镜像块填充模式时，因为padding的数据来自于抠出的图片，左右padding的长度（leftPadSize、rightPadSize）必须小于或等于抠图的水平长度（horizSize），上下padding的长度（topPadSize、botPadSize）必须小于或等于抠图的垂直的长度（vertSize）。
- 支持的物理存储位置为：L1 Buffer（TPosition: A1/B1）。
    <cann-filter npu-type="950">
    - 特别针对Ascend 950PR/Ascend 950DT，支持物理存储位置为：VECIN、VECCALC、VECOUT。
    </cann-filter>

## 调用示例<a name="section22811728184217"></a>

<cann-filter npu-type="310p">

该调用示例支持的运行平台为Atlas 推理系列产品AI Core，示例图片格式为YUV420SP。

```cpp
uint16_t horizSize = 32, vertSize = 32, horizStartPos = 0, vertStartPos = 0, srcHorizSize = 32, srcVertSize = 32, leftPadSize = 0, rightPadSize = 0;
uint32_t dstHorizSize = 32, dstVertSize = 32, cSize = 32;
uint8_t topPadSize = 0, botPadSize = 0;
uint32_t gmSrc0Size = 0, gmSrc1Size = 0, dstSize = 0;
AscendC::AippInputFormat inputFormat = AscendC::AippInputFormat::YUV420SP_U8;
uint32_t cPadMode = 0;
int8_t cPaddingValue = 0;

AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::A1, 1> inQueueA1;
AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueUB;
AscendC::LocalTensor<int8_t> featureMapA1 = inQueueA1.AllocTensor<int8_t>();
uint64_t fm_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(fmGlobal.GetPhyAddr()));
// aipp config
AscendC::AippParams<int8_t> aippConfig;
aippConfig.cPaddingParams.cPaddingMode = cPadMode;
aippConfig.cPaddingParams.cPaddingValue = cPaddingValue;
// fmGlobal为整张输入图片，src1参数处填入图片UV维度的起始地址
AscendC::SetAippFunctions(fmGlobal, fmGlobal[gmSrc0Size], inputFormat, aippConfig);
AscendC::LoadImageToLocal(featureMapA1, { horizSize, vertSize, horizStartPos, vertStartPos, srcHorizSize, topPadSize, botPadSize, leftPadSize, rightPadSize });
```

</cann-filter>
