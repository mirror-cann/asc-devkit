# SetWeight

## 产品支持情况

- Ascend 950PR/Ascend 950DT：不支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

设置权重矩阵Weight。

## 函数原型

```
__aicore__ inline void SetWeight(const AscendC::GlobalTensor<WeightT>& weight)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| weight | 输入 | Weight在Global Memory上的地址。类型为[GlobalTensor](../../../../基础API/数据结构/LocalTensor和GlobalTensor定义/GlobalTensor/GlobalTensor.md)。权重矩阵Weight支持的数据类型为：half、bfloat16_t。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
GlobalTensor<half> weightGm;
weightGm.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(weight));
conv3dApi.SetWeight(weightGm);
```
