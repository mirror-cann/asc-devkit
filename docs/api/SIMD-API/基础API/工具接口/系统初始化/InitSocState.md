# InitSocState<a name="ZH-CN_TOPIC_0000002309203540"></a>

## 产品支持情况<a name="section73648168211"></a>

| 产品 | 是否支持 |
| ---- | -------- |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | √</cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √</cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | x</cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | √</cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | √</cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x</cann-filter> |

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_common_intf_impl.h"`。

初始化AI Core上的一些全局状态，包括Cube Core（AIC）和Vector Core（AIV）的共有状态，以及各自特有的状态：

- 共有状态：
    - 初始化[原子累加状态](../../原子操作/DisableDmaAtomic.md)，关闭数据搬运随路原子操作功能。
    - 初始化[Mask工作模式](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/编程指南/类库API/基础API/常用操作速查指导/如何使用掩码操作API.md)为Normal模式。

- 在AIC上：
    - 初始化[Load3D接口所需的A1(L1)/B1(L1)边界值](../../矩阵计算（ISASI）/数据搬运/SetLoadDataBoundary.md#功能说明)为0，表示无边界。
    - 初始化[Load3D接口调用时Pad填充的数值](../../矩阵计算（ISASI）/数据搬运/SetLoadDataPaddingValue.md#功能说明)为0。

- 在AIV上：
    - 将[Mask](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/编程指南/类库API/基础API/常用操作速查指导/如何使用掩码操作API.md)配置为全1，表示所有数都参与计算。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void InitSocState()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 在实际运行中，这些值可能被前序执行的算子修改，若不调用该接口进行初始化，非预期的值可能导致计算结果出现精度错误。

  例如前序算子使用Counter模式但未重置为Normal模式，当前算子以默认的Normal模式设置Mask时，会导致Mask设置不符合预期，进而引发精度错误。
- 在[TPipe框架编程](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程原理.md)中，初始化过程由TPipe完成，无需开发者关注；在[静态Tensor编程](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的C++编程/静态Tensor编程.md)的场景中用户必须在Kernel入口处调用此函数来初始化AI Core状态。

## 调用示例<a name="section177231425115410"></a>

```cpp
__global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    // 静态Tensor编程方式中需要开发者手动调用InitSocState()接口初始化全局状态寄存器。
    AscendC::InitSocState();
    KernelAdd op;
    op.Init(x, y, z);
    op.Process(); 
}
```
