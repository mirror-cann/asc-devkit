# Cast（float转bfloat16\_t）<a name="ZH-CN_TOPIC_0000001623365980"></a>

## 产品支持情况<a name="section1550532418810"></a>

| 产品 | 是否支持 |
| ---- | -------- |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √</cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | x</cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | x</cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x</cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x</cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_scalar_intf.h"`。

float类型标量数据转换成bfloat16\_t类型标量数据。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bfloat16_t Cast(const float& fVal)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| fVal | 输入 | float类型标量数据。 |

## 返回值说明<a name="section640mcpsimp"></a>

转换后的bfloat16\_t类型标量数据。

## 约束说明<a name="section633mcpsimp"></a>

- float到bfloat16\_t转换采用[CAST\_RINT舍入](../数据结构/precision_conversion.md#二进制精度舍入规则)。
- 输入参数为立即数时，需要使用显式类型转换后输入，例如：

    ```cpp
    // 错误写法，AscendC暂不支持整数类型转bfloat16_t，编译报错
    bfloat16_t x = AscendC::Cast(1);
    // 正确写法
    bfloat16_t x = AscendC::Cast((float)1);
    ```

## 调用示例<a name="section19372434133520"></a>

```cpp
float m = 3.14159f;
bfloat16_t n = AscendC::Cast(m);  // n = 3.140625  float到bfloat16_t进行了舍入
```
