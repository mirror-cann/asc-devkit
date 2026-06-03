# asc_zero_bits_cnt

## 产品支持情况

|产品   | 是否支持 |
| ------------|:----:|
| <cann-filter npu_type="950"><term>Ascend 950PR/Ascend 950DT</term>  | √ </cann-filter>|
| <term>Atlas A3 训练系列产品/Atlas A3 推理系列产品</term> |    √     |
| <term>Atlas A2 训练系列产品/Atlas A2 推理系列产品</term> |    √     |

## 功能说明

获取一个uint64_t类型数字的二进制中0的个数。

## 函数原型

```c++
__aicore__ inline int64_t asc_zero_bits_cnt(uint64_t value)
```

## 参数说明

|参数名|输入/输出|描述|
| :------ | :--- | :------------ |
|value   |输入   |被统计的二进制数字。

## 返回值说明

value中0的个数。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```c++
uint64_t value = 33;
int64_t count_zero = asc_zero_bits_cnt(value);
```
