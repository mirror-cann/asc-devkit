# GetCurNpuArch<a name="ZH-CN_TOPIC_0000002543243123"></a>

## 功能说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

获取当前硬件平台芯片架构版本号。

## 函数原型<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
NpuArch GetCurNpuArch(void) const
```

## 参数说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

无

## 返回值<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

当前硬件平台架构号的枚举类。该枚举类和AI处理器型号的对应关系请通过CANN软件安装后文件存储路径下include/platform/soc\_spec.h头文件获取。

| 产品 | \_\_NPU\_ARCH\_\_ |
|------|------------------|
| Ascend 950PR/Ascend 950DT | DAV\_3510 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | DAV\_2201 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | DAV\_2201 |
| Atlas 200I/500 A2 推理产品 | DAV\_3002 |
| Atlas 推理系列产品AI Core | DAV\_2002 |
| Atlas 推理系列产品Vector Core | DAV\_2002 |
| Atlas 训练系列产品 | DAV\_1001 |

## 约束说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

无

## 调用示例<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    // 根据所获得的版本型号自行设计Tiling策略
    // DAV_XXX请替换为实际的架构号
    if (socVersion == NpuArch::DAV_XXXX) {
        // ...
    }
    return ret;
}
```

