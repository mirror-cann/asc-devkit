# asc_sync_pipe

## 产品支持情况

| 产品 | 是否支持  |
| :-----------| :------: |
| <cann-filter npu_type="950"><term>Ascend 950PR/Ascend 950DT</term>  | √ </cann-filter>|
| <term>Atlas A3 训练系列产品/Atlas A3 推理系列产品</term> |    √     |
| <term>Atlas A2 训练系列产品/Atlas A2 推理系列产品</term> |    √     |

## 功能说明

完成同一流水线内的同步控制，用于在同一流水线内部约束执行顺序。其作用是保证前序指令中所有数据的读写工作全部完成，后序指令才能执行。

## 函数原型

```cpp
__aicore__ inline void asc_sync_pipe(pipe_t pipe)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| pipe | 输入 | 阻塞的流水类别。不支持PIPE_S。如果不关注流水类别，希望阻塞所有流水，可以传入PIPE_ALL。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- Scalar流水之间的同步由硬件自动保证，asc_sync_pipe接口不支持PIPE_S单流水的同步。
- asc_sync_pipe(PIPE_ALL)会等待所有流水线中所有先前提交的接口完成，这会对性能产生影响。若仅阻塞单条流水线即可解决问题，应避免随意调用asc_sync_pipe(PIPE_ALL)。
- PIPE_MTE2/PIPE_MTE3在搬运地址有重叠的情况下需要开发者插入同步。

## 调用示例

```cpp
// src0_gm，src1_gm是外部输入的指向GM内存空间的float类型的指针，有效数据个数分别为128和32。需要将src0_gm中的连续32个数据替换成src1_gm中的数据。
__ubuf__ float src0_ub[128];            // 接收src0_gm中的128个数据。
__ubuf__ float* src1_ub = src0_ub + 64; // 接收src1_gm中的32个数据。位于src0_ub的有效空间内部。

asc_copy_gm2ub(src0_ub, src0_gm, 128 * sizeof(float));

// 插入同步，确保重叠区域以src1_gm中的数据为准。
asc_sync_pipe(PIPE_MTE2);

asc_copy_gm2ub(src1_ub, src1_gm, 32 * sizeof(float));

/*
 * PIPE_ALL可以阻塞所有流水，确保GM -> UB的搬运操作完成。
 * 注意，此处使用asc_sync_pipe(PIPE_ALL)目的是说明其作用。实际应用场景下请灵活选用asc_sync_mte2、asc_sync_notify/asc_sync_wait等接口精准控制，以提升性能。
 */
asc_sync_pipe(PIPE_ALL);

// 完成数据替换，搬出到src0_gm中。
asc_copy_ub2gm(src0_gm, src0_ub, 128 * sizeof(float));
```
