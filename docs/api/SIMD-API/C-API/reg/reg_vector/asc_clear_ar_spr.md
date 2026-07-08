# asc_clear_ar_spr

## 产品支持情况

| 产品         | 是否支持 |
| :-----------------------| :-----:|
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径：`"c_api/reg_compute/reg_vector.h"`。

对AR寄存器进行清理，AR寄存器通常由[asc_squeeze_with_status](./asc_squeeze_with_status.md)接口使用。

## 函数原型

```cpp
__simd_callee__ inline void asc_clear_ar_spr()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
asc_clear_ar_spr();
```