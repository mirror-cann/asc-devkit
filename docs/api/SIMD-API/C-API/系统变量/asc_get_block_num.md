# asc_get_block_num

## 产品支持情况

| 产品 | 是否支持 |
| :-----------| :------: |
| <cann-filter npu_type="950">Ascend 950PR/Ascend 950DT | √    </cann-filter>|
| <term>Atlas A3 训练系列产品/Atlas A3 推理系列产品</term> |    √    |
| <term>Atlas A2 训练系列产品/Atlas A2 推理系列产品</term> |    √    |

## 功能说明

获取当前任务配置的核数，用于代码内部的多核逻辑控制等。

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_block_num()
```

## 参数说明

无

## 返回值说明

当前任务配置的核数。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

无
