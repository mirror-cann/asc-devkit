# asc_set_store_atomic_config_v2（废弃）

## 产品支持情况

|产品   | 是否支持 |
| ------------|:----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

**该接口已废弃。原子加操作请使用[asc_atomic_add](../scalar_compute/asc_atomic_add.md)实现。**

头文件路径：`"c_api/atomic/atomic.h"`。

设置原子操作启用位与原子操作类型的值。

## 函数原型

```cpp
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| type | 输入 | 原子操作启用位。<br>1：开启原子操作，进行原子操作的数据类型为int32_t。<br>4：开启原子操作，进行原子操作的数据类型为half。<br>5：开启原子操作，进行原子操作的数据类型为float。<br>7：开启原子操作，进行原子操作的数据类型为bfloat16_t。<br>9：开启原子操作，进行原子操作的数据类型为int16_t。<br>10：开启原子操作，进行原子操作的数据类型为int8_t。<br>其余值无具体含义。 |
| op | 输入 | 原子操作类型。<br>2：求和操作。<br>其余值无具体含义。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint16_t type = 1;
uint16_t op = 2;
asc_set_store_atomic_config_v2(type, op);
```
