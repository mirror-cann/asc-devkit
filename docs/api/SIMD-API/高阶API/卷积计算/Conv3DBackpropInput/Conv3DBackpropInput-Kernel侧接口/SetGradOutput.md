# SetGradOutput

## 产品支持情况

- Ascend 950PR/Ascend 950DT：不支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

设置卷积反向计算的输入矩阵GradOutput。

## 函数原型

```
__aicore__ inline void SetGradOutput(const AscendC::GlobalTensor<SrcT> &gradOutput)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gradOutput | 输入 | GradOutput矩阵在Global Memory上的首地址。类型为[GlobalTensor](../../../../基础API/数据结构/LocalTensor和GlobalTensor定义/GlobalTensor/GlobalTensor.md)。SrcT表示GradOutput矩阵的数据类型，当前支持的数据类型为：half、bfloat16_t。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
// 设置GradOutput中GlobalTensor的地址
GlobalTensor<gradOutputType> gradOutputGm_;
gradOutputGm_.SetGlobalBuffer((__gm__ gradOutputType *)gradOutput);
gradInput_.SetGradOutput(gradOutputGm_);
```
