# GetSystemCycle(ISASI)

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT | √ </cann-filter>|
| <cann-filter npu-type="A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
| <cann-filter npu-type="310b">Atlas 200I/500 A2 推理产品 | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 AI Core | x </cann-filter>|
| <cann-filter npu-type="310p">Atlas 推理系列产品 Vector Core | x </cann-filter>|
| <cann-filter npu-type="910">Atlas 训练系列产品 | x </cann-filter>|

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取系统计数器当前的cycle值。通过计算代码段前后cycle值的差值，可得到代码段消耗的系统cycle数，再通过换算即可获得代码段的执行时间。

<cann-filter npu-type="950">针对Ascend 950PR/Ascend 950DT，若换算成时间需要按照1GHz的频率，时间单位为us，换算公式为：time = (cycle数/1000)us。</cann-filter>

<cann-filter npu-type="A3">针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，若换算成时间需要按照50MHz的频率，时间单位为us，换算公式为：time = (cycle数/50)us。</cann-filter>

<cann-filter npu-type="910b">针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，若换算成时间需要按照50MHz的频率，时间单位为us，换算公式为：time = (cycle数/50)us。</cann-filter> 

## 函数原型

```cpp
__aicore__ inline int64_t GetSystemCycle()
```

## 参数说明

无

## 返回值说明

返回系统计数器当前的cycle值。

## 约束说明

该接口属于PIPE\_S流水，若需要测试其他流水的指令时间，需要在调用该接口前通过[PipeBarrier](../../同步控制/核内同步/PipeBarrier(ISASI).md)插入对应流水的同步，具体请参考[调用示例](#example2)。

## 调用示例

- 如下示例通过`GetSystemCycle`获取系统cycle数，并换算成时间（单位：us）。

    ```cpp
    #include "kernel_operator.h"

    __aicore__ inline void InitTilingParam(int32_t& totalSize, int32_t& loopSize)
    {
        int64_t systemCycleBefore = AscendC::GetSystemCycle(); // 调用GetBlockNum指令前的cycle数
        loopSize = totalSize / AscendC::GetBlockNum();
        int64_t systemCycleAfter = AscendC::GetSystemCycle(); // 调用GetBlockNum指令后的cycle数
        int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // 执行GetBlockNum指令所用的cycle数
        int64_t CycleToTimeBase = 50; // cycle数转换成时间的基准单位
        /* 在Ascend 950PR/Ascend 950DT上为1000
        在Atlas A3 训练系列产品/Atlas A3 推理系列产品上为50
        在Atlas A2 训练系列产品/Atlas A2 推理系列产品上为50 */
        int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // 执行GetBlockNum指令所用时间，单位为us
    };
    ```

- <a id="example2"></a>如下示例为获取矢量计算Add指令时间的关键代码片段，在调用`GetSystemCycle`之前，插入了`PIPE_ALL`同步，可以保证相关指令执行完后再获取cycle数。

    ```cpp
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleBefore = AscendC::GetSystemCycle(); // 调用Add指令前的cycle数
    AscendC::Add(dstLocal, src0Local, src1Local, 512);
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleAfter = AscendC::GetSystemCycle(); // 调用Add指令后的cycle数
    int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // 执行Add指令所用的cycle数
    int64_t CycleToTimeBase = 50; // cycle数转换成时间的基准单位
    /* 在Ascend 950PR/Ascend 950DT上为1000
    在Atlas A3 训练系列产品/Atlas A3 推理系列产品上为50
    在Atlas A2 训练系列产品/Atlas A2 推理系列产品上为50 */
    int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // 执行Add指令所用时间，单位为us
    ```
