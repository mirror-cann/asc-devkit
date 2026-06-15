# MmadMx<a name="ZH-CN_TOPIC_000000253823118102"></a>

## 产品支持情况


| 产品                                        | 是否支持（<br><br>不传入bias的原型<br><br>） | 是否支持（<br><br>传入bias的原型<br><br>） |
| ------------------------------------------- | -------------------------------------------- | ------------------------------------------ |
| Ascend 950PR/Ascend 950DT                   | √                                           | √                                         |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | x                                            | x                                          |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | x                                            | x                                          |
| Atlas 200I/500 A2 推理产品                  | x                                            | x                                          |
| Atlas 推理系列产品AI Core                   | x                                            | x                                          |
| Atlas 推理系列产品Vector Core               | x                                            | x                                          |
| Atlas 训练系列产品                          | x                                            | x                                          |
| Kirin X90                                   | x                                            | x                                          |
| Kirin 9030                                  | x                                            | x                                          |

## 功能说明

头文件路径为：\#include "basic\_api/kernel\_operator\_mm\_intf.h"。

MmadMx（全称Microscaling Mmad）为带有量化系数的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，左量化系数矩阵scaleA和右量化系数矩阵scaleB。MmadMx场景中，左量化系数矩阵与左矩阵乘积，右量化系数矩阵与右矩阵乘积，对两个乘积的结果做矩阵乘法。

MmadMx的计算公式为：C=(scaleA⊗A)*(scaleB⊗B)+Bias，“⊗”表示广播乘法，左/右矩阵与左/右量化系数矩阵做乘积时，K方向上每32个元素共享一个量化因子，如[图1](#zh_cn_topic_mmadmx_section2_figure1)所示（其中以AB矩阵均为fp4x2_e2m1_t数据类型为例）。

- A、scaleA、B、scaleB为源操作数。A为左矩阵，形状为[M,K]；scaleA为左量化系数矩阵，形状为[M,K/32]；B为右矩阵，形状为[K,N]；scaleB为右量化系数矩阵，形状为[K/32,N]。
- C为目的操作数，存放矩阵乘结果的矩阵，形状为[M,N]。
- Bias为矩阵乘偏置，形状为[1,N]。对(scaleA⊗A)*(scaleB⊗B)结果矩阵的每一行都采用该Bias进行偏置。

**图1**  MmadMx接口矩阵乘分形示意图<a id="zh_cn_topic_mmadmx_section2_figure1"></a>
![MmadMx接口矩阵乘分形示意图](../../../../figures/mxmmad_demo_a5.png "MmadMx接口矩阵乘分形示意图")

**表1**  矩阵计算矩阵A、B、C解释说明

<table>
    <thead>
      <tr>
        <th>矩阵计算逻辑</th>
        <th>矩阵计算物理位置</th>
        <th>维度</th>
        <th>输入/输出数据格式</th>
        <th>数据类型</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>[Nz]</td>
        <td rowspan="5"><a href="#zh_cn_topic_mmadmx_section_datatype">数据类型</a></td>
      </tr>
      <tr>
        <td>scaleA</td>
        <td>L0A_MX Buffer</td>
        <td>M x K/32</td>
        <td>[大Z小z]小z shape为（2，16）</td>
      </tr>
      <tr>
        <td>B</td>
        <td>L0B Buffer</td>
        <td>K x N</td>
        <td>[Zn]</td>
      </tr>
      <tr>
        <td>scaleB</td>
        <td>L0B_MX Buffer</td>
        <td>K/32 x N</td>
        <td>[大N小n]小n shape为（2，16）</td>
      </tr>
      <tr>
        <td>C</td>
        <td>L0C Buffer</td>
        <td>M x N，可支持使用偏置矩阵Bias进行初始化，维度为1 x N</td>
        <td>[Nz]</td>
      </tr>
    </tbody>
  </table>

## 函数原型

- 不传入bias

  ```cpp
  template <typename T, typename U, typename S>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
  ```
- 传入bias

  ```cpp
  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadParams& mmadParams)
  ```

## 参数说明

**表2**  参数说明


| 参数名称   | 输入/输出 | 含义                                                                                                                                                                                                                                                                         |
| ---------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst        | 输出      | 目的操作数，结果矩阵c，类型为LocalTensor，支持的物理存储位置为L0C Buffer（TPosition:CO1）。<br><br>LocalTensor的起始地址需要按照1024字节对齐。                                                                                                                               |
| fm         | 输入      | 源操作数，左矩阵a，类型为LocalTensor，支持的物理存储位置为L0A Buffer（TPosition:A2）。<br><br>左矩阵a对应的scale矩阵起始地址为：A矩阵起始对应地址/16。<br><br>对于fp4场景LocalTensor的起始地址需要按照512字节对齐。对于fp8场景LocalTensor的起始地址需要按照1024字节对齐。    |
| filter     | 输入      | 源操作数，右矩阵b，类型为LocalTensor，支持的物理存储位置为L0B Buffer（TPosition:B2）。<br><br>右矩阵b对应的scale矩阵起始地址为：B矩阵起始对应地址/16。<br><br>对于fp4场景LocalTensor的起始地址需要按照512字节对齐。对于fp8场景LocalTensor的起始地址需要按照1024字节对齐。    |
| bias       | 输入      | 源操作数，bias矩阵，类型为LocalTensor，支持的物理存储位置为BT Buffer（TPosition:C2）。<br><br>LocalTensor的起始地址需要按照64字节对齐。                                                                                                                                      |
| mmadParams | 输入      | 矩阵乘相关参数。该参数类型的具体定义请参考\${INSTALL_DIR}/include/ascendc/basic_api/interface/kernel_struct_mm.h，${INSTALL_DIR}请替换为CANN软件安装后文件存储路径。<br><br>MmadParams参数说明请参考[MmadParams结构体内参数说明](Mmad.md#zh_cn_topic_mmad_section4_table4)。 |

## 数据类型<a id="zh_cn_topic_mmadmx_section_datatype"></a>

**表3**  MmadMx接口左矩阵、右矩阵、Scale矩阵、Bias矩阵、结果矩阵支持的精度类型组合（Ascend 950PR/Ascend 950DT）


| 左矩阵fm     | 右矩阵filter | Scale矩阵  | 偏置Bias | 结果矩阵dst |
| ------------ | ------------ | ---------- | -------- | ----------- |
| fp4x2_e1m2_t | fp4x2_e1m2_t | fp8_e8m0_t | float    | float       |
| fp4x2_e2m1_t | fp4x2_e1m2_t | fp8_e8m0_t | float    | float       |
| fp4x2_e1m2_t | fp4x2_e2m1_t | fp8_e8m0_t | float    | float       |
| fp4x2_e2m1_t | fp4x2_e2m1_t | fp8_e8m0_t | float    | float       |
| fp8_e4m3fn_t | fp8_e4m3fn_t | fp8_e8m0_t | float    | float       |
| fp8_e4m3fn_t | fp8_e5m2_t   | fp8_e8m0_t | float    | float       |
| fp8_e5m2_t   | fp8_e4m3fn_t | fp8_e8m0_t | float    | float       |
| fp8_e5m2_t   | fp8_e5m2_t   | fp8_e8m0_t | float    | float       |

## 返回值说明

无

## 约束说明

- 不同矩阵对于存储位置的约束：
  - 结果矩阵C只支持位于物理存储位置为L0C Buffer（TPosition:CO1），大小256KB。
  - 左矩阵A只支持位于物理存储位置为L0A Buffer（TPosition:A2），大小64KB。
  - 右矩阵B只支持位于物理存储位置为L0B Buffer（TPosition:B2），大小64KB。
  - Bias矩阵只支持位于物理存储位置为BT Buffer（TPosition:C2），大小4KB。
- 地址约束说明请参考表2。
- 当M、K、N中的任意一个值为0时，表示指令不会执行，该接口将被视为NOP（空操作）。
- K需要是64的倍数。
- 对于fp4场景a/b矩阵的起始地址需要按照512字节对齐，对于fp8场景a/b矩阵的起始地址需要按照1024字节对齐。
- 左矩阵a/b对应的scale矩阵起始地址为：a/b矩阵起始对应地址/16。
- 其他特殊场景约束可参考[Mmad接口约束说明](Mmad.md#约束说明)。

## 调用示例

完整使用样例请参考[MmadMx最佳实践](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_mxfp4_basic_api_high_performance)。
