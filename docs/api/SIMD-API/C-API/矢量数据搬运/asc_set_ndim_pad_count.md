# asc_set_ndim_pad_count

## 产品支持情况

|产品   | 是否支持 |
| ------------|:----:|
| <cann-filter npu_type="950"><term>Ascend 950PR/Ascend 950DT</term>  | √ </cann-filter>|

## 功能说明

用于设置[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)接口的各个维度左右侧的padding元素个数。

## 函数原型

```c++
__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| config | 输入 | 用于设置asc_ndim_copy_gm2ub接口的各个维度左右侧的padding元素个数，详细说明请参考[asc_ndim_pad_count_config.md](../数据结构/asc_ndim_pad_count_config.md)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

需配合[asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md)使用。

## 调用示例

```c++
asc_ndim_pad_count_config config;  // config默认初始值为0。
asc_set_ndim_pad_count(config); 
```