# GetBlockNum

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | √ </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 AI Core | √ </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | √ </cann-filter>|
| <cann-filter npu-type="x90">Kirin X90 | √ </cann-filter>|
| <cann-filter npu-type="9030">Kirin 9030 | √ </cann-filter>|

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取当前任务配置的逻辑AI Core的数量，用于代码内部的多核逻辑控制等。

## 函数原型

```cpp
__aicore__ inline int64_t GetBlockNum()
```

## 参数说明

无

## 返回值说明

返回当前任务配置的逻辑AI Core的数量。

- 返回值与<<<\>\>\>调用时配置的numBlocks一致，为参与计算的逻辑AI Core的数量。
- 当仅启动Cube Core（AIC）或Vector Core（AIV）时，实际启动的AIC或AIV的数量与numBlocks一致。
- 当同时启动Cube Core（AIC）与Vector Core（AIV）时，实际启动的AIC或AIV的数量可以通过[GetTaskRatio\(\)](GetTaskRatio.md) \* numBlocks计算得到，在当前支持的配置下：
    - 当AIC与AIV比例为1:2时，AIC启动的数量为numBlocks，AIV启动的数量为2 \* numBlocks；
    - 当AIC与AIV比例为1:1时，AIC启动的数量为numBlocks，AIV启动的数量为numBlocks。

## 约束说明

无

## 调用示例

```cpp
// srcGm、dstGm为外部输入的gm空间
AscendC::GlobalTensor<float> srcGlobal;
AscendC::GlobalTensor<float> dstGlobal;
int64_t blockNum = AscendC::GetBlockNum(); // 获取当前任务配置的逻辑AI Core的数量
int64_t perBlockSize = srcDataSize / blockNum; // 每个核平分处理相同个数
int64_t blockIdx = AscendC::GetBlockIdx(); // 获取当前工作的核ID
srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(srcGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // 分配每个核上的srcGlobal的内存地址
dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(dstGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // 分配每个核上的dstGlobal的内存地址
```
