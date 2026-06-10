# SetLoadDataBoundary<a name="ZH-CN_TOPIC_0000001787861430"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
|------|:------:|
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品AI Core | √ </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：basic\_api/kernel\_operator\_mm\_intf.h。

设置Load3D接口所需的L1 Buffer（TPosition: A1/B1）边界值。

如果Load3D指令在处理源操作数时，源操作数在A1/B1上的地址超出设置的边界，则会从A1/B1起始地址开始读取数据。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataBoundary(uint32_t boundaryValue)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
|------|------|------|
| boundaryValue | 输入 | 边界值：单位是Byte。<br>&bull; Load3Dv1指令：单位是32字节。<br>&bull; Load3Dv2指令：单位是字节。 |

## 返回值说明

无

## 约束说明<a name="section633mcpsimp"></a>

- 用于Load3Dv1时，boundaryValue的最小值是16（单位：32字节）；用于Load3Dv2时，boundaryValue的最小值是1024（单位：字节）。
- 如果使用SetLoadDataBoundary接口设置了边界值，配合Load3D指令使用时，Load3D指令的A1/B1初始地址要在设置的边界内。
- 如果boundaryValue设置为0，则表示无边界，可使用整个A1/B1。
- 配合Load3D指令使用时，边界值大小最小值要求是1024，如果设置1-1023值是未定义行为。
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

参考[调用示例](SetFmatrix.md#section642mcpsimp)。

本接口用于配合Load3D接口使用，展示代码示例片段：

```cpp
// Load3D指令完成img2col的过程，可知img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ci * kh * kw,代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。最后，配置loadDataParams.enTranspose = true，将整个A 矩阵转置并且将其中每一个分形转置
// 使用load3d接口，实现NZ2ZZ
AscendC::LoadData3DParamsV2<T> loadDataParams;
// 设置loadDataParams相关参数
...

// SetLoadDataBoundary设置为0，可使用整个L1 Buffer
AscendC::SetLoadDataBoundary(0);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
