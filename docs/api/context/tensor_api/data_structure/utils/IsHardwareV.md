# IsHardwareV

## 产品支持情况

| 产品     | 是否支持 |
| ----------- | :----: |
| Ascend 950PR/Ascend 950DT | √ |

## 功能说明

头文件路径为：`"tensor_api/tensor.h"`。

IsHardwareV用于判断一个类型是否为Tensor API中定义的硬件存储位置类型。

## 函数原型

```cpp
template <typename T>
constexpr bool IsHardwareV = IsHardware<T>::value;
```

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| T | 输入 | 待判断的类型。当前支持识别的硬件位置类型包括：Location::INVALID、Location::GM、Location::UB、Location::L1、Location::L0A、Location::L0B、Location::L0ScaleA、Location::L0ScaleB、Location::L0C、Location::BIAS、Location::FIXBUF、Location::SSBUF |

## 返回值说明

若T硬件位置类型，则返回true，否则返回false。

## 调用示例

```cpp
using namespace AscendC::Te;

constexpr bool isGM = IsHardwareV<Location::GM>;
// isGM = true

constexpr bool isL0A = IsHardwareV<const Location::L0A&>;
// isL0A = true

constexpr bool isInt = IsHardwareV<int>;
// isInt = false
```
