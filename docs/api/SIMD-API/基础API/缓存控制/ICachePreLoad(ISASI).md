# ICachePreLoad\(ISASI\)<a name="ZH-CN_TOPIC_0000001834752537"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持  |
| :----------------------- | :------: |
|<cann-filter npu-type="950"> Ascend 950PR/Ascend 950DT | √ </cann-filter>|
|<cann-filter npu-type="A3"> Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="910b"> Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ </cann-filter>|
|<cann-filter npu-type="310b"> Atlas 200I/500 A2 推理产品 | x </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品AI Core | √ </cann-filter>|
|<cann-filter npu-type="310p"> Atlas 推理系列产品Vector Core | x </cann-filter>|
|<cann-filter npu-type="910"> Atlas 训练系列产品 | x </cann-filter>|

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_cache_intf.h"`。

开发者手动调用ICachePreLoad接口，能够从指令所在GM地址预加载指令到ICache中。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ICachePreLoad(const int64_t preFetchLen)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| preFetchLen | 输入 | 预取长度。<cann-filter npu-type="950"><br>&bull;针对Ascend 950PR/Ascend 950DT，preFetchLen参数单位为2K Byte，取值应小于ICache的大小/2K。AIC和AIV的ICache大小分别为32KB和16KB。</cann-filter><cann-filter npu-type="A3"><br>&bull;针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，preFetchLen参数单位为2K Byte，取值应小于ICache的大小/2K。AIC和AIV的ICache大小分别为32KB和16KB。</cann-filter><cann-filter npu-type="910b"><br>&bull;针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，preFetchLen参数单位为2K Byte，取值应小于ICache的大小/2K。AIC和AIV的ICache大小分别为32KB和16KB。</cann-filter><cann-filter npu-type="310p"><br>&bull;针对Atlas 推理系列产品AI Core，传入该参数无效，预取长度均为128Byte。</cann-filter> |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

```cpp
int64_t preFetchLen = 2; // 预取指令长度。
AscendC::ICachePreLoad(preFetchLen);
```
