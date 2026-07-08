# Cube指令理论性能汇总<a name="ZH-CN_TOPIC_0000002607728945"></a>

本节汇总介绍了主要的Cube指令的理论性能，以下内容针对如下型号生效：

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->

**表1**  Mmad理论性能计算公式中并行度和k0的取值

<a name="table1877123815211"></a>
| 接口 | 左矩阵A | 右矩阵B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| Mmad | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 4 | 8 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | int4b_t | int4b_t | 16 | 16 | 64 | 64 |
| MmadWithSparse | int8_t | int8_t | 16 | 16 | 32 | 32 |

