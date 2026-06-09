# Int8/Int4 Channel Merge

Channel Merge是矩阵计算结果搬出过程中的硬件自动合并能力，仅在目的格式NZ的输出场景生效，用户无需也不能通过参数手动配置。

- Int8 Channel Merge：当目标数据类型为`int8_t`或`uint8_t`时，NZ输出自动启用该特性。硬件会将分形矩阵从16 x 16扩展为16 x 32。若输出通道数N是16的偶数倍，则N方向上每两个相邻的16 x 16分形矩阵合并为一个16 x 32分形矩阵；若N是16的奇数倍，则前N - 16个通道参与合并，最后16个通道保持16 x 16分形矩阵不变。针对Ascend 950PR/Ascend 950DT，`fp8_e4m3fn_t`、`hifloat8_t`类型也支持该特性。
- Int4 Channel Merge：当目标数据类型为`int4_t`或`uint4_t`时，NZ输出自动启用该特性。硬件会将分形矩阵从16 x 16扩展为16 x 64。若输出通道数N是64的倍数，则N方向上每4个相邻的16 x 16分形矩阵合并为一个16 x 64分形矩阵。针对Ascend 950PR/Ascend 950DT，若N不是64的倍数，剩余部分会自动补零对齐。

## 使用示例

- Int8 Channel Merge：当目标数据类型为`int8_t`，M为32，N为48时，硬件会将N方向上前两个16 x 16分形矩阵合并成一个16 x 32分形矩阵，剩余的一个16 x 16分形矩阵保持不变并输出。
- Int4 Channel Merge：当目标数据类型为`int4_t`，M为32，N为64时，硬件会将N方向上每4个相邻的16 x 16分形矩阵合并成一个16 x 64分形矩阵后输出。

**图 1**  Int8 Channel Merge示意图

![](../figures/int8_channel_merge_figure.png)

**图 2**  Int4 Channel Merge示意图

![](../figures/int4_channel_merge_figure.png)

## 约束说明

- Channel Merge为硬件自动使能，不能通过参数配置。
- Int8 Channel Merge在NZ输出且目的类型为`int8_t`或`uint8_t`时自动启用；Ascend 950PR/Ascend 950DT上`fp8_e4m3fn_t`、`hifloat8_t` NZ输出也支持该特性。
- Int4 Channel Merge在底层`int4_t`/`uint4_t` NZ输出场景自动启用。针对Ascend 950PR/Ascend 950DT，N不是64的倍数时，剩余部分会自动补零对齐。
