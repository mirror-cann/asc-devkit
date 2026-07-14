# Nop

## 产品支持情况

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas 训练系列产品：不支持
<!-- end id14 -->

## 功能说明

用户输入延迟的个数，实现延迟若干个cycle。

## 函数原型

```cpp
template <int count = 1>
__aicore__ inline void Nop()
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| count | 输入 | 延迟count个cycle。 |

## 约束说明

无

## 调用示例

如下样例中，延迟10个cycle。

```cpp
Nop<10>();
```

