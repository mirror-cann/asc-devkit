# SetBias

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：支持
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持
- Kirin X90：支持

## 功能说明

设置矩阵乘的Bias。

## 函数原型

```
__aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
```

```
__aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| biasGlobal | 输入 | Bias矩阵。类型为[GlobalTensor](../../../基础API/数据结构/LocalTensor和GlobalTensor定义/GlobalTensor/GlobalTensor.md)。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。<br><br>A矩阵、B矩阵、Bias支持的数据类型组合可参考[Matmul输入输出数据类型的组合说明](Matmul使用说明.md#table1996113269499)；在MxMatmul场景，A矩阵、B矩阵、Bias支持的数据类型组合可参考[MatmulTypeWithScale参数说明](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/MxMatmul场景.md#zh-cn_topic_0000002270097206_section2756107144914)。 |
| inputBias | 输入 | Bias矩阵。类型为[LocalTensor](../../../基础API/数据结构/LocalTensor和GlobalTensor定义/LocalTensor/LocalTensor.md)，支持的TPosition为TSCM/VECOUT。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。<br><br>A矩阵、B矩阵、Bias支持的数据类型组合可参考[Matmul输入输出数据类型的组合说明](Matmul使用说明.md#table1996113269499)；在MxMatmul场景，A矩阵、B矩阵、Bias支持的数据类型组合可参考[MatmulTypeWithScale参数说明](https://gitcode.com/cann/asc-devkit/blob/master/docs/guide/算子实践参考/SIMD算子实现/矩阵编程（高阶API）/特性场景/MxMatmul场景.md#zh-cn_topic_0000002270097206_section2756107144914) 。 |

## 返回值说明

无

## 约束说明

-   在Matmul Tiling计算中，必须配置TCubeTiling结构中的[isBias](../Matmul-Tiling侧接口/Matmul-Tiling类/TCubeTiling结构体.md#p2051215216314)参数为1，即启用Bias后，才能调用本接口设置Bias矩阵。
-   传入的Bias地址空间大小需要保证不小于singleN。
-   对于Ascend 950PR/Ascend 950DT，Bias矩阵的内存逻辑位置为TSCM且数据类型为float或int32\_t时，Bias矩阵的LocalTensor空间必须64字节对齐。
-   支持的数据类型<a id="li12616155731720"></a>

    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16_t、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

    Atlas 推理系列产品AI Core，支持的数据类型为：half、float、int32_t，对于包含biasGlobal参数的原型，仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

    Atlas 200I/500 A2 推理产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

    Kirin X90，支持的数据类型为：half、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);  // 设置Bias
}
mm.IterateAll(gm_c);
mm.End();
```
