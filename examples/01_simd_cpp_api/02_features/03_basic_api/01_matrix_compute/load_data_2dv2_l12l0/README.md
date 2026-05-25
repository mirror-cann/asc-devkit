# load_data_2dv2_l12l0样例

## 概述

本样例介绍了输入为ND格式，B8 / B16 / B32输入数据类型（具体以int8_t / half / float为例），左、右矩阵转置、不转置组合共 7 种矩阵乘法场景中使用 `LoadData` 接口配合LoadData2DParamsV2结构体参数(本样例中简称：`Load2Dv2`接口)将矩阵数据从 L1 搬运到 L0A/L0B 的方法。
- 使用 `LoadData2DParamsV2` 参数结构体
- L0A 数据排布为 **Nz 格式**
- L0B 数据排布为 **Zn 格式**
- 支持转置功能，通过 `ifTranspose` 参数控制
- 支持 int8_t、half、float 三种数据类型

## 支持的产品
- Ascend 950PR/Ascend 950DT

## 目录结构

```
├── load_data_2dv2_l12l0
│   ├── figures                          // 图示
│   ├── scripts
│   │   ├── gen_data.py                  // 输入数据和真值数据生成脚本
│   │   └── verify_result.py             // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── data_utils.h                     // 数据读入写出函数
│   └── load_data_2dv2_l12l0.asc         // Ascend C样例实现 & 调用样例
```

## 样例描述

### 样例功能

样例整体流程如下：

```
GM(ND) -> L1(Nz) -> L0A(Nz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │         │                  │          │       
    DataCopy  Load2Dv2            Mmad        Fixpipe 
```

**步骤详解**：

1. **GM → L1**：使用 `DataCopy` 实现 ND 到 Nz 格式转换
2. **L1 → L0A/L0B**：使用 `Load2Dv2` 接口搬运，通过 `ifTranspose` 控制转置
3. **矩阵乘**：使用 `Mmad` 接口执行矩阵乘法
4. **L0C → GM**：使用 `Fixpipe` 接口搬出结果


所有场景基于相同的矩阵乘规格：[m, n, k] = [40, 50, 70]，核函数名称为"KernelLoadDataL12L0Load2Dv2"。以参数scenarioNum来代表上述7种场景，scenarioNum不同取值对应的含义如下，如下[表1](#表1)所示：<br>

<a name="表1"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：scenarioNum不同取值的含义</span>
  <tr>
    <td ><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">输入数据类型</span></td>
    <td><span style="font-weight: bold;">输出数据类型</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">是否多搬多算</span></td>
    <td><span style="font-weight: bold;">L1->L0调用接口方式</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2" >int8_t</td>
    <td rowspan="2" >int32_t</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>true</td>
    <td>false</td>
    <td>是</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="2" >half</td>
    <td rowspan="2" >float</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td rowspan="2" >float</td>
    <td rowspan="2" >float</td>
    <td>false</td>
    <td>true</td>
    <td>否</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td>Load2Dv2</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td rowspan="1" >int8_t</td>
    <td rowspan="1" >int32_t</td>
    <td>true</td>
    <td>false</td>
    <td>否</td>
    <td>for循环+Load2Dv2</td>
  </tr>
</table>

注：scenarioNum取值7时，L1->L0A的搬运采用for循环+Load2Dv2, 其余场景均调用一次Load2dV2。


**场景1：输入int8_t数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，int8_t类型，ND格式; B [50, 70]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式；
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：L1->L0A不需要转置，通过配置mStep、kStep、srcStride、srcStride等参数完成A矩阵L1 -> L0A的搬运；同理，L1->L0B不需要转置，B矩阵通过配置Step、kStep、srcStride、srcStride等参数完成L1 -> L0B的搬运及大分形排布格式的变化。

**场景2：输入int8_t数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，int8_t类型，ND格式; B [70, 50]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式;
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：A矩阵调用一次Load2DV2指令，通过配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；B矩阵调用一次Load2DV2指令，配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。该场景m方向多搬多算脏数据分形，在L0C搬出到GM时，通过fixpipeParam.mSize = m来保证无效数据参与计算的结果不会被搬出。

**场景3：输入half数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，half类型，ND格式; B [50, 70]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：L1->L0A不需要转置，通过配置mStep、kStep、srcStride、srcStride等参数完成A矩阵L1 -> L0A的搬运；同理，L1->L0B不需要转置，B矩阵通过配置Step、kStep、srcStride、srcStride等参数完成L1 -> L0B的搬运及大分形排布格式的变化。

**场景4：输入half数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，half类型，ND格式; B [70, 50]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：A矩阵调用一次Load2DV2指令，通过配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；B矩阵调用一次Load2DV2指令，配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。

**场景5：输入float数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，float类型，ND格式; B [50, 70]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：L1->L0A不需要转置，通过配置mStep、kStep、srcStride、srcStride等参数完成A矩阵L1 -> L0A的搬运；同理，L1->L0B不需要转置，B矩阵通过配置Step、kStep、srcStride、srcStride等参数完成L1 -> L0B的搬运及大分形排布格式的变化。

**场景6：输入float数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，float类型，ND格式; B [70, 50]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用一次Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：A矩阵调用一次Load2DV2指令，通过配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；B矩阵调用一次Load2DV2指令，配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。

**场景7：输入int8_t数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，int8_t类型，ND格式; B [70, 50]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式;
- 实现：使用for循环+Load2DV2将A矩阵从L1搬运到L0A，使用一次Load2DV2将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿k方向for循环，调用多次Load2DV2指令，一次从L1上搬运k轴方向上的2个分形 * m轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；B矩阵调用一次Load2DV2指令，配置mStep、kStep、srcStride、srcStride，并配置ifTranspose为true，完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。该场景通过for循环配合dstStride参数在写入L0A时跳过转置时多读的m方向脏数据分形，使矩阵计算m方向没有多余脏数据分形参与计算。

为了方便描述，在此对后续常用概念给出定义：
 
（1）fractalShape: 小分形的shape为[16, 32 / sizeof(T)]，其中T表示输入数据类型。本样例涉及到数据类型的分形相关信息如[表2](#表2)。

（2）fractalSize: 1个小分形包含的元素个数，具体可参考[表2](#表2)。


（3）fractalNum: 当从L1 -> L0A/L0B需要转置时，Load2DV2接口内部会按照小方块矩阵转置，对于B8和B32数据类型fractalShape分别为[16,32]和[16,8]，都需要两个连续的小分形合并为一个方块然后转置，因此该参数表示一个方块包含几个小分形，具体可参考[表2](#表2)。


<a name="表2"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
    <td align="center">[16, 8]</td>
    <td align="center">128</td>
    <td align="center">2</td>
  </tr>
</table>


（4）CeilAlign：向上对齐操作，例如m=30时，CeilAlign(30, 16)=32，表示将m轴向16对齐，对齐后m轴长度为32。



      __aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
          return (size + alignValue - 1) / alignValue * alignValue;
      }

（5）CeilDivision：向上取整除法，一般用于求解向上对齐后的循环次数。

（6）mAlignValue：m轴向mAlignValue对齐，例如mAlignValue=32，代表m轴对齐到32，依次类推还有nAlignValuem表示轴向nAlignValue对齐，kaAlignValue表示A矩阵k轴向kaAlignValue对齐，kbAlignValue表示B矩阵k轴向kbAlignValue对齐。

（7）mAlignL0和mAlignL1：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有nAlignL0、nAlignL1、kaAlignL0、kaAlignL1、kbAlignL0、kbAlignL1。


另外，A、B矩阵在L1和L0上在row和col方向上对齐的要求也不相同，现将[表2](#表2)中scenarioNum取1-6对应的6种场景(L1上的排布格式为Nz)的对齐要求总结得到
如下[表3](#表3)、[表4](#表4)：

<a name="表3"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：A、B矩阵在L1上各个轴对齐要求(L1排布格式为Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kaAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >mAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kbAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >nAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >nAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>


<a name="表4"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1->L0A不需要转置</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1->L0A需要转置</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[1]</td>
    <td >kaAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1->L0B需要转置</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[1]</td>
      <td align="center">kbAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >nAlignValue = fractalShape[0]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1->L0B不需要转置</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>


特别的，当scenarioNum=7时，A矩阵用for循环+Load2DV2实现L1->L0A的搬运，L0A仅写入有效数据向分形对齐后的数据，A、B矩阵在L1和L0上在高度和宽度方向上对齐的要求如[表5](#表5)、[表6](#表6)：
<a name="表5"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：scenarioNum=7，A、B矩阵在L1上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td align="center" >kaAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td align="center" >kbAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="表6"></a>
<table border="2">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：scenarioNum=7，A、B矩阵在L0上各个轴对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1->L0A需要转置</span></td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1->L0B需要转置</span></td>
    <td align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
</table>

### 样例实现
A/B矩阵在L1上的数据排布格式为Nz，在L0A、L0B上分别为Nz、Zn。在L1->L0过程中调用Load2DV2接口完成数据搬运及格式变换。
#### A矩阵 L1->L0A 不转置

L1 -> L0A不转置时数据搬运，不存在大分形和小分形排布格式变化。该场景下，B8 / B16 / B32这3种数据类型使用Load2DV2接口参数配置基本相同，仅fractalShape有区别，可参考[表2](#表2)。以int8_t为例展示图示。<br>


<p align="center">
  <img src="figures/B8_A_l1_l0A_Load2dv2.png">
</p>

<p align="center">
图1: int8_t数据类型下，L1 -> L0A不转置，调用Load2DV2数据排布示意图
</p>


如图1所示，从L1上A矩阵的起始地址开始搬运，m为row，配置mstep参数，k为col，配置kStep参数，调用一次指令完成整个A矩阵从L1->L0A的搬运。

```cpp
mAlignL1 = CeilAlign(m, fractalShape[0]);
kaAlignL1 = CeilAlign(k, fractalShape[1]);
mAlignL0 = CeilAlign(m, fractalShape[0]);
kaAlignL0 = CeilAlign(k, fractalShape[1]);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(mAlignL1, fractalShape[0]); // row
loadDataParams.kStep = CeilDivision(kaAlignL1, fractalShape[1]); // col
loadDataParams.srcStride = CeilDivision(mAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

#### A矩阵 L1->L0A 转置

L1->L0A时大分形排布格式变换，且小分形需要转置。以int8_t、half、float为例，按照数据类型，分小节分别介绍B8 / B16 / B32这3不同数据类型转置场景图示说明。

##### B8输入数据类型
B8输入数据类型分形为16 * 32，L1->L0转置时会按照方型将row方向的2个16 * 32的分形拼成1个32 * 32的方型进行转置。本样例中以int8_t输入数据类型为例，m为40，若调用一次Load2DV2指令完成L1->L0A的搬运+转置，则在写入L0A上时m方向会多写1个分形的无效数据，矩阵计算Mmad在计算时m方向也多算1个分形的无效数据，在Fixpipe搬出时仅搬出有效数据即可；若调用for循环+Load2DV2指令完成L1->L0A的搬运+转置，在写入时可跳过m方向的无效数据分形。下面分别介绍这2种场景。<br>
**调用一次Load2DV2**


调用一次Load2DV2完成L1->L0A搬运+转置的图示如下：


<p align="center">
  <img src="figures/B8_A_l1_l0A_trans_load2dv2.png">
</p>

<p align="center">
图2: int8_t数据类型下，L1 -> L0A转置，调用一次Load2DV2数据排布示意图
</p>


本样例m=40，如图2所示，在L1上mAlignL1 = CeilAlign(m, fractalShape[1])=64。mAlignL1 - m = 24 > 16，调用一次Load2DV2完成L1->L0A搬运时，m方向会搬运1个无效的分形，如图红框所示。该场景矩阵计算Mmad在计算时m方向也多算1个分形的无效数据，即 mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)，在Fixpipe搬出时仅搬出有效数据(fixpipeParams.mSize = m)。


```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum);
mAlignL1 = CeilAlign(m, fractalShape[1]);
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum);
kaAlignL0 = CeilAlign(k, fractalShape[1]);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```


**for循环+Load2Dv2**


for循环调用多次Load2DV2完成L1->L0A搬运+转置的图示如下：

<p align="center">
  <img src="figures/B8_A_l1_l0A_trans_for_load2dv2.png">
</p>

<p align="center">
图3: int8_t数据类型下，L1 -> L0A转置，for循环调用多次Load2DV2数据排布示意图
</p>


本样例m=40，如图3所示，在L1上mAlignL1 = CeilAlign(m, fractalShape[1])=64。mAlignL1 - m = 24 > 16，此时，沿k方向for循环，调用多次Load2DV2指令，一次从L1上搬运k轴方向上的2个分形 * m轴方向上的CeilDivision(k, fractalShape[1])个分形，如图红框所示。dstStride配置为m方向有效数据向小分形fractalShape[0]对齐，则在写入L0A时跳过转置时多读的m方向脏数据分形，使矩阵计算m方向没有多余脏数据分形参与计算。


```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum);
mAlignL1 = CeilAlign(m, fractalShape[1]);
mAlignL0 = CeilAlign(m, fractalShape[0]);
kaAlignL0 = CeilAlign(k, fractalShape[1]);
// 输入为int8类型，A矩阵[k,m]转置输入，L1->L0A需要转置
// for循环调用Load2DV2，以k轴方向做for循环，每次循环在L1的k方向搬运2个分形，在L0A上跳过m方向尾脏数据分形，m方向多搬运数据不超过1个分形
uint16_t L0ALoopNum = CeilDivision(kaAlignL0, fractalShape[0] * fractalNum);
loadDataParams.mStep = INT8_M_STEP_ALIGN;
loadDataParams.kStep = CeilDivision(mAlignL0, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
uint32_t dstOffset = 0;
for (uint16_t loopIdx = 0; loopIdx < L0ALoopNum; ++loopIdx) {
    loadDataParams.mStartPosition = INT8_M_STEP_ALIGN * loopIdx;
    AscendC::LoadData(a2Local[dstOffset], a1Local, loadDataParams);
    dstOffset += CeilAlign(mAlignL0, fractalShape[0]) * fractalShape[1];
}
```

##### B16输入数据类型
B16输入数据类型分形为16 * 16，一个分形即一个方型，L1->L0转置时会按照小分形进行转置，调用一次Load2DV2即可完成L1->L0A数据搬运及转置。本样例以half为例，调用一次Load2DV2完成L1->L0A搬运+转置的图示如下：


<p align="center">
  <img src="figures/B16_A_l1_l0A_trans_load2dv2.png">
</p>

<p align="center">
图4: half数据类型下，L1 -> L0A转置，调用一次Load2DV2数据排布示意图
</p>

如图4所示，从L1上A矩阵的起始地址开始搬运，k为row，配置mstep参数，m为col，配置kStep参数，配合ifTranspose=true，调用一次指令完成整个A矩阵从L1->L0A的搬运+转置。

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum);
mAlignL1 = CeilAlign(m, fractalShape[1]);
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum);
kaAlignL0 = CeilAlign(k, fractalShape[1]);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```


##### B32输入数据类型
B32输入数据类型分形为16 * 8，L1->L0转置时会按照方型将col方向的2个16 * 8的分形拼成1个16 * 16的方型进行转置。本样例以float数据类型为例，调用一次Load2DV2完成L1->L0A搬运+转置的图示如下:

<p align="center">
  <img src="figures/B32_A_l1_l0A_trans_load2dv2.png">
</p>

<p align="center">
图5: float数据类型下，L1 -> L0A转置，调用一次Load2DV2数据排布示意图
</p>


本样例中m为40，由于转置时会将col方向2个分形拼成方型进行转置（kStep必须是2的倍数），L1上col方向（m方向）会多一个无效分形数据以满足指令要求。调用一次Load2DV2指令完成L1->L0A的搬运+转置，写入L0A上时k方向会多写1个分形的无效数据。由于L0A上排布格式为Nz，k方向多余的无效分形数据在尾部，在进行矩阵计算Mmad时配置k方向mmadParams.k = k即可仅让有效数据参与矩阵计算。


```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0]);
mAlignL1 = CeilAlign(m, fractalShape[1] * fractalNum);
mAlignL0 = CeilAlign(m, fractalShape[0]);
kaAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```


#### B矩阵 L1->L0B 不转置

L1 -> L0B不转置时数据搬运，仅存在大分形格式变化。该场景下，B8 / B16 / B32这3种数据类型使用Load2DV2接口参数配置基本相同，仅fractalShape有区别，可参考[表2](#表2)。以float为例展示图示。<br>


<p align="center">
  <img src="figures/B32_B_l1_l0B_load2dv2.png">
</p>

<p align="center">
图6: float数据类型下，L1 -> L0B不转置，调用一次Load2DV2数据排布示意图
</p>


如图6所示，从L1上B矩阵的起始地址开始搬运，n为row，配置mstep参数，k为col，配置kStep参数，调用一次指令完成整个B矩阵从L1->L0B的搬运。

```cpp
nAlignL1 = CeilAlign(n, fractalShape[0]);
kbAlignL1 = CeilAlign(k, fractalShape[1]);
kbAlignL0 = CeilAlign(k, fractalShape[1]);
nAlignL0 = CeilAlign(n, fractalShape[0]);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(nAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(kbAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(nAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

#### B矩阵 L1->L0B 转置

L1->L0B时大分形排布格式变换，且小分形需要转置。以int8_t、half、float为例，按照数据类型，分小节分别介绍B8 / B16 / B32这3不同数据类型转置场景图示说明。

##### B8输入数据类型
B8输入数据类型分形为16 * 32，L1->L0转置时会按照方型将row方向的2个16 * 32的分形拼成1个32 * 32的方型进行转置。本样例中以int8_t为例，调用一次Load2DV2指令完成L1->L0B的搬运+转置，如下图所示。


<p align="center">
  <img src="figures/B8_B_l1_l0B_trans_load2dv2.png">
</p>

<p align="center">
图7: int8_t数据类型下，L1 -> L0B转置，调用一次Load2DV2数据排布示意图
</p>


本样例中k=70，由于转置时会将row方向2个分形拼成方型进行转置（指令参数mStep必须是2的倍数），L1上row方向（k方向）会多一个无效分形数据以满足指令要求。调用一次Load2DV2指令完成L1->L0B的搬运+转置。

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum);
nAlignL1 = CeilAlign(n, fractalShape[1]);
kbAlignL0 = CeilAlign(k, fractalShape[1]);
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### B16输入数据类型

B16输入数据类型分形为16*16，一个分形即一个方型，L1->L0转置时会按照小分形进行转置，调用一次Load2DV2即可完成L1->L0B数据搬运及转置。本样例以half为例，调用一次Load2DV2完成L1->L0B搬运+转置的图示如下：


<p align="center">
  <img src="figures/B16_B_l1_l0B_trans_load2dv2.png">
</p>

<p align="center">
图8: half数据类型下，L1 -> L0B转置，调用一次Load2DV2数据排布示意图
</p>


如图8所示，从L1上B矩阵的起始地址开始搬运，k为row，配置mstep参数，N为col，配置kStep参数，配合ifTranspose=true，调用一次指令完成整个B矩阵从L1->L0B的搬运+转置。


```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum);
nAlignL1 = CeilAlign(n, fractalShape[1]);
kbAlignL0 = CeilAlign(k, fractalShape[1]);
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### B32输入数据类型
B32输入数据类型分形为16 * 8，L1->L0转置时会按照方型将col方向的2个16 * 8的分形拼成1个16 * 16的方型进行转置。本样例中以float为例，调用一次Load2DV2指令完成L1->L0B的搬运+转置，如下图所示。


<p align="center">
  <img src="figures/B32_B_l1_l0B_trans_load2dv2.png">
</p>

<p align="center">
图9: float数据类型下，L1 -> L0B转置，调用一次Load2DV2数据排布示意图
</p>


本样例中n=50，由于转置时会将col方向2个分形拼成方型进行转置（指令参数kStep必须是2的倍数），L1上col方向（n方向）会多一个无效分形数据以满足指令要求。调用一次Load2DV2指令完成L1->L0B的搬运+转置，写入L0B上时k方向会多写1个分形的无效数据。由于L0B上排布格式为Zn，k方向多余的无效分形数据在尾部，在进行矩阵计算Mmad时配置mmadParams.k = k即可仅让有效数据参与矩阵计算。


```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0]);
nAlignL1 = CeilAlign(n, fractalShape[1] * fractalNum);
kbAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum);
nAlignL0 = CeilAlign(n, fractalShape[0]);
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```


## 编译运行
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
  - 默认路径，root用户安装CANN软件包
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - 默认路径，非root用户安装CANN软件包
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - 指定路径install_path，安装CANN软件包
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- 样例执行
  ```bash
  SCENARIO=1
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO ..;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO output/output.bin output/golden.bin
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO ..;make -j;   # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO ..;make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`7` | 场景编号：不同数据类型和转置组合 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
