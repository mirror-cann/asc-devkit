# asc_set_gm2l1_nz_para

## 产品支持情况

| 产品 | 是否支持 |
| :-----------| :------: |
| <term>Ascend 950PR/Ascend 950DT</term> |    √     |

## 功能说明

对MTE2_NZ_PARA寄存器中的值进行设置，可以在调用[asc_copy_gm2l1_dn2nz](../矩阵数据搬运/asc_copy_gm2l1_dn2nz.md)和[asc_copy_gm2l1_nd2nz](../矩阵数据搬运/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md)前设置相关参数。

MTE2_NZ_PARA是一个64bit的寄存器，其中各bit含义如下：

| bit范围 |        含义         |
|:------|:-----------------:|
| 15:0  |  需要搬运的ND/DN矩阵数量。  | 
| 31:16 | N维度上相邻两个block块之间的间隔。 |
| 47:32 | 同一个NZ矩阵中2个C0列之间的间隔。 |
| 63:48 | 两个ND/DN矩阵中两个NZ矩阵之间的间隔。 |

## 函数原型

```cpp
__aicore__ inline void asc_set_gm2l1_nz_para(uint64_t config)
```

## 参数说明

|参数名|输入/输出| 描述        |
| :------ | :---  |:----------|
| config | 输入 | 待设置的寄存器值。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例
```cpp
uint64_t nz_dst_stride = 16;
uint64_t config = nz_dst_stride << 48; //[63:48]
uint64_t c0_stride = 32;
config |= c0_stride << 32; //[47:32]
uint64_t n_dst_stride = 32;
config |= n_dst_stride << 16; //[31:16]
uint64_t nd_num = 64;
config |= nd_num; //[15:0]

asc_set_gm2l1_nz_para(config);
```