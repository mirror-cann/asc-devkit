# thread_block构造函数

## 产品支持情况

| 产品 | 是否支持 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x |
| Atlas 200I/500 A2 推理产品 | x |
| Atlas 推理系列产品AI Core | x |
| Atlas 推理系列产品Vector Core | x |
| Atlas 训练系列产品 | x |

## 功能说明

`thread_block`不提供默认的构造函数，用户使用`this_thread_block`函数获取当前线程所属的`thread_block`对象。

## 函数原型

```c++
thread_block this_thread_block()
```

## 参数说明

无

## 返回值说明

返回当前线程所属的`thread_block`对象。

## 约束说明

无

## 调用示例

参考[调用示例](sync.md#调用示例)
