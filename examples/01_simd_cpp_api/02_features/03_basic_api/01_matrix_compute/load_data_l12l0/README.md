# load_data_l12l0样例

## 概述
本样例介绍了输入为ND格式，B4 / B8 / B16 / B32输入数据类型（具体以int4_t / int8_t / half / float为例），左、右矩阵转置、不转置组合共 14 种矩阵乘法场景中，相关指令的使用方法，着重介绍`LoadData`指令的使用方法，整体流程如下：<br>
（1）矩阵A (矩阵乘的左输入矩阵)和 矩阵B (矩阵乘的右输入矩阵)从 GM -> L1 时，如何调用 `DataCopy`指令配合`Nd2NzParams`结构体参数(本样例中简称：`DataCopyND2NZ`);<br>
（2）矩阵A和矩阵B在不同场景下如何调用`LoadData`配合`LoadData2DParams`结构体参数(本样例中简称: `Load2D`)、`LoadDataWithTranspose` 和 `LoadData`配合`LoadData3DParamsV2`结构体参数(本样例中简称：`Load3DV2`) 三个指令从L1 -> L0A / L0B;<br>
（3）使用`Mmad`指令实现矩阵乘法计算(C = A * B);<br>
（4）使用`Fixpipe`指令将结果矩C从L0C -> GM。<br>
各指令参数配置及执行指令前后各个矩阵数据排布变化，均配合示意图进行了说明。
## 支持的产品
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍
```
├── load_data_l12l0
│   ├── img                         // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   └── load_data_l12l0.asc         // Ascend C样例实现 & 调用样例
```

## 样例描述
一次完整的矩阵乘法涉及的数据搬运过程包括：GM -> L1、L1 -> L0A/L0B、L0C -> GM，其中A、B矩阵在不同存储单元的数据排布格式不同，如[表1](#表1)：<br>

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">数据排布格式</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Zz</td>
    <td align="center">Zn</td>
    <td align="center">Zz</td>
  </tr>
</table>

当输入数据类型分别取int4_t / int8_t / half / float时，通过布尔变量isAtranspose和isBtranspose分别控制A、B矩阵是否转置输入，组合得到13种场景。<br>
此外，A矩阵转置输入、B矩阵不转置输入时，若L1上A、B矩阵为Zz排列，在L1-->L0搬运时可调用`LoadDataWithTranspose`接口; 而当排列为通常情况下的Nz排列时，需要调用`LoadData3DV2`接口完成L1 -> L0的搬运。因此，在原有13种场景的基础上，需要新增一种特殊场景：L1上A、B矩阵为Zz排布，且L1 -> L0搬运调用`LoadDataWithTranspose`接口。

下文将介绍上述14种场景下，A、B矩阵在完整矩阵乘法流程的各个阶段前后数据的排布方式、对齐要求、所调用的指令以及如何配置相应的参数，着重介绍`LoadData`指令。<br>
（1）从L1 -> L0通路，常用的搬运指令有`Load2D`、`LoadDataWithTranspose` 和 `Load3DV2` 三个指令，不同场景和输入数据类型可使用的指令如[表2](#表2);<br>
（2）程序中以loadData2AMode、loadData2BMode分别表示L1->L0A和L1->L0B使用的搬运指令，如[表3](#表3)所示;<br>
（3）以参数scenarioNum来代表上述14种场景，scenarioNum不同取值对应的含义及L1->L0过程中调用的搬运指令，如下[表4](#表4)所示。<br>
所有场景基于相同的矩阵乘规格：[m, n, k] = [40, 50, 70]，核函数名称为"KernelLoadDataL12L0"


<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：L1-->L0，不同场景下可调用的`LoadData`指令</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A不转置输入[m, k]<br>L1->L0A不需要转置</span></td>
    <td align="center">`Load2D`、`Load3DV2`</td>
    <td align="center">`Load2D`、`Load3DV2`</td>
    <td align="center">`Load2D`、`Load3DV2`</td>
    <td align="center">`Load2D`、`Load3DV2`</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">A转置输入[k, m]<br>L1->L0A需要转置</span></td>
    <td align="center">不支持</td>
    <td align="center">`LoadDataWithTranspose`</td>
    <td align="center">`Load2D`、`Load3DV2`、`LoadDataWithTranspose`</td>
    <td align="center">`Load3DV2`、<br>`LoadDataWithTranspose`(L1上数据排布为Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B不转置输入[k, n]<br>L1->L0B需要转置</span></td>
    <td align="center">`LoadDataWithTranspose`</td>
    <td align="center">`LoadDataWithTranspose`</td>
    <td align="center">`Load2D`、`Load3DV2`、`LoadDataWithTranspose`</td>
    <td align="center">`Load3DV2`、<br>`LoadDataWithTranspose`(L1上数据排布为Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B转置输入[n, k]<br>L1->L0B不需要转置</span></td>
    <td align="center">`Load2D`</td>
    <td align="center">`Load2D`</td>
    <td align="center">`Load2D`</td>
    <td align="center">`Load2D`</td>
  </tr>
</table>

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：loadDataMode不同取值的含义</span>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">不同loadDataMode值调用的`LoadData`指令</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">0</span></td>
    <td colspan="3" align="center">`Load2D`</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">1</span></td>
    <td colspan="3" align="center">`LoadDataWithTranspose`</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">2</span></td>
    <td colspan="3" align="center">`Load3DV2`</td>
  </tr>
</table>



<a name="表4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表4：scenarioNum不同取值的含义</span>
  <tr>
    <td ><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">输入数据类型</span></td>
    <td><span style="font-weight: bold;">输出数据类型</span></td>
    <td><span style="font-weight: bold;">A矩阵</span></td>
    <td><span style="font-weight: bold;">B矩阵</span></td>
    <td><span style="font-weight: bold;">A矩阵调用`LoadData`指令类型</span></td>
    <td><span style="font-weight: bold;">B矩阵调用`LoadData`指令类型</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2" >int4_t</td>
    <td rowspan="2" >int32_t</td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load2D`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>不转置</td>
    <td>不转置</td>
    <td>`Load3Dv2`</td>
    <td>`LoadDataWithTranspose`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="3" >int8_t</td>
    <td rowspan="3" >int32_t</td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load2D`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load3Dv2`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>转置</td>
    <td>不转置</td>
    <td>`LoadDataWithTranspose`</td>
    <td>`LoadDataWithTranspose`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td rowspan="5" >half</td>
    <td rowspan="5" >float</td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load2D`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load3Dv2`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">8</span></td>
    <td>转置</td>
    <td>不转置</td>
    <td>`Load2D`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">9</span></td>
    <td>转置</td>
    <td>不转置</td>
    <td>`LoadDataWithTranspose`</td>
    <td>`LoadDataWithTranspose`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">10</span></td>
    <td>转置</td>
    <td>不转置</td>
    <td>`Load3Dv2`</td>
    <td>`Load3Dv2`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">11</span></td>
    <td rowspan="3" >float</td>
    <td rowspan="3" >float</td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load2D`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">12</span></td>
    <td>不转置</td>
    <td>转置</td>
    <td>`Load3Dv2`</td>
    <td>`Load2D`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">13</span></td>
    <td>转置</td>
    <td>不转置</td>
    <td>`Load3Dv2`</td>
    <td>`Load3Dv2`</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">14</span></td>
    <td rowspan="1" >float</td>
    <td rowspan="1" >float</td>
    <td>转置</td>
    <td>不转置</td>
    <td>`LoadDataWithTranspose`</td>
    <td>`LoadDataWithTranspose`</td>
  </tr>
</table>

注：scenarioNum取值1到13时，L1上A、B矩阵均为Nz排布；scenarioNum=14时，L1上A、B矩阵均为Zz排布。



**场景1：输入int4_t数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，int4_t类型，ND格式；B [50, 70]，int4_t类型，ND格式；
- 输出C [40, 50]，int32_t类型，ND格式；
- 实现：使用`Load2D`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿m方向for循环，一次搬运k轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置srcStride、dstGap等参数完成A矩阵L1 -> L0A的搬运及大分形排布格式的变化；同理，B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景2：输入int4_t数据类型，isAtranspose=False，isBtranspose=False**
- 输入A [40, 70]，int4_t类型，ND格式；B [70, 50]，int4_t类型，ND格式；
- 输出C [40, 50]，int32_t类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`LoadDataWithTranspose`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置源操作数和目的操作数的shape信息等(l1H、l1W、kExtension、mExtension)将A矩阵从L1上的Nz转换成L0A中需要的Zz分形排布；通过`LoadDataWithTranspose`完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置，最终实现小分形转置为n格式，大分形排布为Z。

**场景3：输入int8_t数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，int8_t类型，ND格式; B [50, 70]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式；
- 实现：使用`Load2D`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿m方向for循环，一次搬运k轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置srcStride、dstGap等参数完成A矩阵L1 -> L0A的搬运及大分形排布格式的变化；同理，B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景4：输入int8_t数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，int8_t类型，ND格式; B [50, 70]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置源操作数和目的操作数的shape信息等(l1H、l1W、kExtension、mExtension)将A矩阵从L1上的Nz转换成L0A中需要的Zz分形排布；B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵大分形排布格式的变化。

**场景5：输入int8_t数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，int8_t类型，ND格式; B [70, 50]，int8_t类型，ND格式;
- 输出C [40, 50]，int32_t类型，ND格式;
- 实现：使用`LoadDataWithTranspose`将A矩阵从L1搬运到L0A，使用`LoadDataWithTranspose`将B矩阵从L1搬运到L0B;
- 说明：通过`LoadDataWithTranspose`完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；通过`LoadDataWithTranspose`完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置；

**场景6：输入half数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，half类型，ND格式; B [50, 70]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load2D`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿m方向for循环，一次搬运k轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置srcStride、dstGap等参数完成A矩阵L1 -> L0A的搬运及大分形排布格式的变化；同理，B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景7：输入half数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，half类型，ND格式; B [50, 70]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置源操作数和目的操作数的shape信息等(l1H、l1W、kExtension、mExtension)将A矩阵从L1上的Nz转换成L0A中需要的Zz分形排布；B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景8：输入half数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，half类型，ND格式; B [70, 50]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load2D`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿m方向for循环，一次搬运k轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置ifTranspose、srcStride、dstGap等参数将A矩阵从L1搬运到L0A时完成小分形转置及大分形排布格式的变化；同理，B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置ifTranspose、srcStride、dstGap等参数将B矩阵从L1搬运到L0B时完成小分形转置及大分形排布格式的变化。

**场景9：输入half数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，half类型，ND格式; B [70, 50]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`LoadDataWithTranspose`将A矩阵从L1搬运到L0A，使用`LoadDataWithTranspose`将B矩阵从L1搬运到L0B;
- 说明：通过`LoadDataWithTranspose`完成A矩阵从L1到L0A的搬运，搬运过程中会伴随转置；通过`LoadDataWithTranspose`完成B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。

**场景10：输入half数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，half类型，ND格式; B [70, 50]，half类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`Load3DV2`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置转置、源操作数和目的操作数的shape信息等(enTranspose、l1H、l1W、kExtension、mExtension)将A矩阵从L1上搬运至L0A，搬运过程中会伴随转置；B矩阵调用`Load3DV2`默认会开启转置，不需要配置enTranspose，其他配置与A矩阵搬运类似。

**场景11：输入float数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，float类型，ND格式; B [50, 70]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load2D`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：A矩阵沿m方向for循环，一次搬运k轴方向上的CeilDivision(k, fractalShape[1])个分形，通过配置srcStride、dstGap等参数完成A矩阵L1 -> L0A的搬运及大分形排布格式的变化；同理，B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景12：输入float数据类型，isAtranspose=False，isBtranspose=True**
- 输入A [40, 70]，float类型，ND格式; B [50, 70]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`Load2D`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置源操作数和目的操作数的shape信息等(l1H、l1W、kExtension、mExtension)将A矩阵从L1上的Nz转换成L0A中需要的Zz分形排布；B矩阵沿k方向for循环，一次搬运n方向上的CeilDivision(n, fractalShape[0])个分形，通过配置srcStride、dstGap等参数完成B矩阵L1 -> L0B的搬运及大分形排布格式的变化。

**场景13：输入float数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，float类型，ND格式; B [70, 50]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`Load3DV2`将A矩阵从L1搬运到L0A，使用`Load3DV2`将B矩阵从L1搬运到L0B;
- 说明：当`Load3DV2`指令配置N为1，卷积核width和height为1，padding为0，滑动步长为1，卷积核膨胀系数为1，经过image to column展开后的数据排布可看做L1上的Nz分形排布，再通过配置转置、源操作数和目的操作数的shape信息等(enTranspose、l1H、l1W、kExtension、mExtension)将A矩阵从L1上搬运至L0A，搬运过程中会伴随转置；B矩阵调用`Load3DV2`默认会开启转置，不需要配置enTranspose，其他配置与A矩阵搬运类似。

**场景14：输入float数据类型，isAtranspose=True，isBtranspose=False**
- 输入A [70, 40]，float类型，ND格式; B [70, 50]，float类型，ND格式;
- 输出C [40, 50]，float类型，ND格式；
- 实现：使用`LoadDataWithTranspose`将A矩阵从L1搬运到L0A，使用`LoadDataWithTranspose`将B矩阵从L1搬运到L0B;
- 说明：当L1上的排布格式为Zz时，调用`LoadDataWithTranspose`实现A矩阵从L1到L0A的搬运及B矩阵从L1到L0B的搬运，搬运过程中会伴随转置。


为了方便描述，在此对后续常用概念给出定义：
 
（1）fractalShape: 小分形的shape为[16, 32 / sizeof(T)]，其中T表示输入数据类型。注：B4输入数据类型shape为[16, 64]。本样例涉及到数据类型的分形相关信息如[表5](#表5)。

（2）fractalSize: 1个小分形包含的元素个数，具体可参考[表5](#表5)。


（3）fractalNum: 当从L1 -> L0A/L0B需要转置且调用`LoadDataWithTranspose`接口时，该接口一次只能转置一个方块矩阵，对于B8和B32数据类型fractalShape分别为[16,32]和[16,8]，都需要两个连续的小分形合并为一个方块然后转置，因此该参数表示一个方块包含几个小分形，具体可参考[表5](#表5)。


<a name="表5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表5：不同数据类型分形相关信息</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
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

（6）mAlignValue：m轴向mAlignValue对齐，例如mAlignValue=32，代表m轴对齐到32，依次类推还有kAlignValue、nAlignValue。

（7）mAlignL0和mAlignL1：A矩阵分别在L1和L0A上时，m轴对齐后的值。依次类推还有kAlignL0、kAlignL1、nAlignL0、nAlignL1。

（8）srcoffset和dstoffset：在L1上，A/B矩阵外轴方向每循环一次时，LocalTensor的地址偏移量；在L0A/L0B上，A/B矩阵外轴方向每循环一次时,LocalTensor的地址偏移量。
注意，为了方便理解，本次样例全部默认将A矩阵的m轴和B矩阵的k轴作为外轴循环，暂不考虑将m轴与k轴中较长轴作为外轴的场景。

另外，A、B矩阵在L1和L0上在row和col方向上对齐的要求也不相同，现将[表4](#表4)中scenarioNum取1-13对应的13种场景(L1上的排布格式为Nz)的对齐要求总结得到
如下[表6](#表6)、[表7](#表7)：

<a name="表6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表6：A、B矩阵在L1上各个轴的对齐要求(L1排布格式为Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
</table>


<a name="表7"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表7：A、B矩阵在L0上各个轴的对齐要求</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵不转置输入[m, k]，L1->L0A不需要转置</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1->L0A需要转置</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
    <td >kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1->L0B需要转置</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
      <td align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B矩阵转置输入[n, k]，L1->L0B不需要转置</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center" >kAlignValue = fractalShape[1]</td>
  </tr>
</table>

特别的，当scenarioNum=14时，L1上的排布格式为Zz，A、B矩阵在L1和L0上在高度和宽度方向上对齐的要求如[表8](#表8)、[表9](#表9)：
<a name="表8"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表8：scenarioNum=14，A、B矩阵在L1上(Zz排布)各个轴的对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">float (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]</span></td>
    <td align="center" >kAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >mAlignValue = fractalShape[1]*fractalNum</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]</span></td>
    <td align="center" >kAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[1]*fractalNum</td>
  </tr>
</table>

<a name="表9"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表9：scenarioNum=14，A、B矩阵在L0上各个轴的对齐要求</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">float (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A矩阵转置输入[k, m]，L1->L0A需要转置</span></td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >kAlignValue = fractalShape[1]*fractalNum</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B矩阵不转置输入[k, n]，L1->L0B需要转置</span></td>
    <td align="center" >kAlignValue = fractalShape[1]*fractalNum</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[0]</td>
  </tr>
</table>

### 1.整体流程图

矩阵乘整体流程图如下所示：

<p align="center">
  <img src="img/cube.png" width="800">
</p>

<p align="center">
图1：矩阵乘整体流程图
</p>


### 2.GM到L1 (`DataCopy`)
本小节主要介绍A、B矩阵在GM上的数据排布格式为ND，在L1上为Nz时，在GM->L1过程中调用`DataCopy`接口完成数据搬运及格式变换。特殊的，介绍场景14，float输入数据类型，A矩阵转置输入[k, m]，B矩阵不转置输入[k, n]搬运到L1时格式为Zz的数据搬运及格式变换。
#### 2.1. A矩阵GM->L1
B4 / B8 / B16 / B32输入，A矩阵从GM(ND)->L1(Nz)的搬运逻辑相似，按照GM上A矩阵输入不转置[m, k]和转置[k, m]分2小节，不转置输入以half为例，转置输入以int8_t为例进行图示说明，其余数据类型仅dstNzC0Stride参数不同，dstNzC0Stride参数取A矩阵在L1上，row方向的对齐后的长度，具体可参考[表6](#表6)和[表8](#表8)。特殊场景14(L1上的格式为Zz)，以float输入数据类型为例。
##### 2.1.1. A矩阵GM上输入为[m, k]
A矩阵GM输入不转置([m, k])时，half输入数据排布转换如下图所示：
<p align="center">
  <img src="img/GM_L1_FP16_A_input_m_k_to_Nz.png" width="800">
</p>

<p align="center">
图2：A矩阵不转置输入([m,k])，half数据类型下，GM -> L1数据排布示意
</p>

下面将介绍如何配置
[`DataCopy`随路转换ND2NZ搬运](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00127.html)指令的`Nd2NzParams`结构体的成员，各个成员变量的具体含义这里不再赘述。其中需要注意的是，dstNzC0Stride的单位为32B，该参数取值为L1上NZ矩阵的对齐后的行数。

            nd2nzA1Params.ndNum = 1;
            nd2nzA1Params.nValue = m;
            nd2nzA1Params.dValue = k;
            nd2nzA1Params.srcNdMatrixStride = 0;
            nd2nzA1Params.srcDValue = k;

            // 以下这个参数取A矩阵在L1上，row方向的对齐后的长度
            nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);

            nd2nzA1Params.dstNzNStride = 1;
            nd2nzA1Params.dstNzMatrixStride = 0;

##### 2.1.2. A矩阵GM上输入为[k, m]
**(1) L1上的格式为Nz**


A矩阵GM输入转置([k, m])时，float输入数据，L1上排布格式为Nz排布转换如下图所示:
<p align="center">
  <img src="img/GM_L1_FP32_A_transInput_k_m_to_Nz.png" width="800">
</p>

<p align="center">
图3：A矩阵转置输入，float数据类型下，GM -> L1，ND -> Nz
</p>

配置`Nd2NzParams`结构体的成员时，需要注意，源操作数的shape为[k, m]以及dstNzC0Stride的单位为32B，该参数取值为L1上NZ矩阵的对齐后的行数。

    nd2nzA1Params.ndNum = 1;
    nd2nzA1Params.nValue = k;
    nd2nzA1Params.dValue = m;
    nd2nzA1Params.srcNdMatrixStride = 0;
    nd2nzA1Params.srcDValue = m;
    nd2nzA1Params.dstNzNStride = 1;
    nd2nzA1Params.dstNzMatrixStride = 0;
    // 以下这个参数取A矩阵在L1上，row方向的对齐后的长度
    if constexpr (AscendC::IsSameType<T, float>::value) {
      nd2nzA1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
    }

**(2) L1上的格式为Zz**

A矩阵GM输入转置([k, m])时，float输入数据，L1上排布格式为Zz排布转换如下图所示:
<p align="center">
  <img src="img/GM_L1_FP32_A_inputTrans_k_m_to_Zz.png" width="800">
</p>

<p align="center">
图4：A矩阵转置输入，float数据类型下，GM -> L1，ND -> Zz
</p>

如上图4所示，A矩阵转置输入，输入数据类型为float类型时，若想调用`LoadDataWithTranspose`接口实现L1 -> L0A的数据搬运和转置，则要求L1上A矩阵的排布必须是Zz，因此需要在GM -> L1阶段调用`DataCopyND2NZ`指令时通过巧妙地配置`Nd2NzParams`结构体来实现ND2Zz的效果。
`DataCopyND2NZ`指令实现ND2Zz效果的核心思想：将1个ND矩阵沿着高度轴以步长16进行切分，看作CeilDivision(k, 16)个ND矩阵。由于搬运后CeilDivision(k, 16)个NZ矩阵在高度轴方向有且仅有1个分形，因此最终搬运到L1上的A矩阵等效于Zz排列。
                    
      nd2nzA1Params.ndNum = CeilDivision(k, fractalShape[0]);
      nd2nzA1Params.nValue = fractalShape[0];
      nd2nzA1Params.dValue = m;
      nd2nzA1Params.srcNdMatrixStride = fractalShape[0] * m;
      nd2nzA1Params.srcDValue = m;
      nd2nzA1Params.dstNzC0Stride = fractalShape[0];
      nd2nzA1Params.dstNzNStride = 1;
      nd2nzA1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(m, fractalShape[1] * fractalNum);

#### 2.2. B矩阵GM->L1
B4 / B8 / B16 / B32输入，B矩阵从GM(ND)->L1(Nz)的搬运逻辑相似，按照B矩阵GM输入不转置[k, n]和转置[n, k]分2小节，不转置输入以float为例，转置输入以half为例进行图示说明，其余数据类型仅dstNzC0Stride参数不同，dstNzC0Stride参数取B矩阵在L1上，row方向的对齐后的长度，具体可参考[表6](#表6)和[表8](#表8)。特殊场景14，以float输入数据类型为例介绍。
##### 2.2.1. B矩阵GM上输入为[k, n]
**(1) L1上的格式为Nz**


B矩阵GM输入不转置([k, n])时，float输入数据，L1上排布格式为Nz排布转换如下图所示:
<p align="center">
  <img src="img/GM_L1_FP32_B_input_k_n_to_Nz.png" width="800">
</p>

<p align="center">
图5：B矩阵不转置输入，float数据类型下，GM -> L1，ND -> Nz
</p>

配置`Nd2NzParams`结构体的成员时，需要注意，源操作数的shape为[k, n]，dstNzC0Stride的单位为32B，取值为L1上Nz矩阵的对齐后的行数。

          nd2nzB1Params.ndNum = 1;
          nd2nzB1Params.nValue = k;
          nd2nzB1Params.dValue = n;
          nd2nzB1Params.srcNdMatrixStride = 0;
          nd2nzB1Params.srcDValue = n;
          nd2nzB1Params.dstNzNStride = 1;
          nd2nzB1Params.dstNzMatrixStride = 0;
          nd2nzB1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
**(2) L1上的格式为Zz**



B矩阵GM输入不转置([k, n])时，float输入数据，L1上排布格式为Zz排布转换如下图所示:
<p align="center">
  <img src="img/GM_L1_FP32_B_input_k_n_to_Zz.png" width="800">
</p>

<p align="center">
图6：B矩阵不转置输入，float数据类型下，GM -> L1，ND -> Zz
</p>


与2.1.2小节类似，当输入为float，L1上的数据排布格式为Zz时：
需要在GM-->L1阶段调用`DataCopyND2NZ`指令时通过巧妙地配置`Nd2NzParams`结构体来实现ND2ZZ的效果。`DataCopyND2NZ`指令实现ND2Zz效果的核心思想在于将1个ND矩阵沿着高度轴以步长16进行切分，看作CeilDivision(k, 16)个ND矩阵。由于搬运后CeilDivision(k, 16)个Nz矩阵在高度轴方向有且仅有1个分形，因此最终搬运到L1上的B矩阵等效于Zz排列。

          nd2nzB1Params.ndNum = CeilDivision(k, fractalShape[0]);
          nd2nzB1Params.nValue = fractalShape[0];
          nd2nzB1Params.dValue = n;
          nd2nzB1Params.srcNdMatrixStride = fractalShape[0] * n;
          nd2nzB1Params.srcDValue = n;
          nd2nzB1Params.dstNzC0Stride = fractalShape[0];
          nd2nzB1Params.dstNzNStride = 1;
          nd2nzB1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(n, fractalShape[1] * fractalNum);

##### 2.2.2. B矩阵GM上输入为[n, k]
B矩阵GM输入转置([n, k])时，half输入数据，L1排布转换如下图所示
<p align="center">
  <img src="img/GM_L1_FP16_B_transInput_n_k_to_Nz.png" width="800">
</p>

<p align="center">
图7：B矩阵转置，half数据类型下，GM -> L1，ND -> Nz
</p>

下面将介绍如何配置
[`DataCopy`随路转换ND2NZ搬运](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00127.html)指令的`Nd2NzParams`结构体的成员，各个成员变量的具体含义这里不再赘述。其中需要注意的是，dstNzC0Stride的单位为32B，该参数取值为L1上NZ矩阵的对齐后的行数。


            nd2nzB1Params.ndNum = 1;
            nd2nzB1Params.nValue = n;
            nd2nzB1Params.dValue = k;
            nd2nzB1Params.srcNdMatrixStride = 0;
            nd2nzB1Params.srcDValue = k;

            // 以下这个参数取B矩阵在L1上，row方向的对齐后的长度
            nd2nzB1Params.dstNzC0Stride = CeilAlign(n, fractalShape[0]);
            nd2nzB1Params.dstNzNStride = 1;
            nd2nzB1Params.dstNzMatrixStride = 0;
### 3.L1到L0 (`LoadData`)
通常A/B矩阵在L1上的数据排布格式为Nz，在L0A、L0B上分别为Zz、Zn。L1上排布格式为Nz时，在L1->L0过程中可调用`Load2D`、`LoadDataWithTranspose`、`Load3DV2`接口完成数据搬运及格式变换。特殊的，本节展示了L1上排布格式为Zz时，调用`LoadDataWithTranspose`完成L1->L0的数据搬运及格式变换。
#### 3.1. A矩阵L1->L0A
B4 / B8 / B16 / B32这4种输入数据类型在A矩阵从L1搬运到L0A时，转置和不转置场景下可使用的`LoadData`相关指令不同，详见[表2](#表2)。下面将分别介绍这些场景。
##### 3.1.1. A矩阵L1->L0A不转置
L1 -> L0A不转置时仅大分形排布格式变换。该场景下，B4 / B8 / B16 / B32这4种数据类型均可使用`Load2D`接口、`Load3DV2`接口实现数据搬运，参数配置基本相同，仅fractalShape有区别，可参考[表5](#表5)。以half为例展示图示。<br>
**(1) `Load2D`接口**


调用`Load2D`接口图示如下：
<p align="center">
  <img src="img/L1_L0A_FP16_A_Load2D.png" width="800">
</p>

<p align="center">
图8：half数据类型下，L1 -> L0A不转置，调用`Load2D`数据排布示意图
</p>

下面将介绍如何配置
[`Load2D`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00169.html)指令的`LoadData2DParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

如图8所示，以m轴方向作为外轴进行for循环(如图红框部分)，以k轴方向作为内轴来配置loadDataParams.repeatTimes。srcoffset和dstoffset的含义分别是：在L1上，A矩阵m轴方向每循环一次时,LocalTensor的地址偏移量；在L0A上，A矩阵m轴方向每循环一次时,LocalTensor的地址偏移量。

            uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize;
            uint32_t srcOffset = fractalSize;
            // Nz -> Zz
            AscendC::LoadData2DParams loadDataParams;
            loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1]);
            loadDataParams.srcStride = CeilDivision(m, fractalShape[0]);
            // K轴方向相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔
            loadDataParams.dstGap = 0;
            loadDataParams.ifTranspose = false;
            for (int i = 0; i < CeilDivision(m, fractalShape[0]); ++i) {
                AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
            }
**(2) `Load3DV2`接口**


调用`Load3DV2`接口如下所示：
<p align="center">
  <img src="img/L1_L0A_F16_A_Load3DV2.png" width="5000">
</p>

<p align="center">
图9：half数据类型下，L1 -> L0A不转置，调用`Load3DV2`数据排布示意图
</p>

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。当按照如下代码所示的参数配置则可调用一次[`Load3Dv2`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00170.html)指令指，实现L1搬运到L0A时数据排布格式由Nz转换为Zz。根据`Load3Dv2`指令完成img2col的过程，可知 img2col后A矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为 CeilAlign(m, fractalShape[0])；img2col后A矩阵宽度为ho * wo,ci * kh * kw,代入kh=1,kw=1，可知A矩阵的宽度为CeilAlign(k, fractalShape[1])。

            // Load3dV2: Nz -> Zz
            AscendC::LoadData3DParamsV2<T> loadDataParams;
            // 源操作数height
            loadDataParams.l1H = 1;
            // 源操作数width
            loadDataParams.l1W = CeilAlign(m, fractalShape[0]);
            // 源操作数的通道数，
            // img2col的结果矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
            // img2col的结果矩阵宽度为ci * kh * kw,代入kh=1,kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
            loadDataParams.channelSize = CeilAlign(k, fractalShape[1]);
            // 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
            loadDataParams.kExtension = CeilAlign(k, fractalShape[1]);
            // 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。
            loadDataParams.mExtension = CeilAlign(m, fractalShape[0]);
            // 卷积核在源操作数width维度滑动的步长
            loadDataParams.strideW = 1;
            // 卷积核在源操作数height维度滑动的步长
            loadDataParams.strideH = 1;
            // 卷积核width
            loadDataParams.filterW = 1;
            // 卷积核height
            loadDataParams.filterH = 1;
            // 卷积核width膨胀系数
            loadDataParams.dilationFilterW = 1;
            // 卷积核height膨胀系数
            loadDataParams.dilationFilterH = 1;
            loadDataParams.filterSizeW = false;
            loadDataParams.filterSizeH = false;
            loadDataParams.enTranspose = false;
            loadDataParams.fMatrixCtrl = false;

##### 3.1.2. A矩阵L1->L0A转置
L1->L0A时大分形排布格式变换，且小分形需要转置。该场景下，Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持B4数据类型，B8 / B16 / B32这3种数据类型可使用的接口不同，以int8_t、half、float为例，按照数据类型，分小节分别介绍不同数据类型可使用的接口及图示说明。

###### 3.1.2.1 输入数据类型为int8_t

调用`LoadDataWithTranspose`接口如下所示：
<p align="center">
  <img src="img/L1_L0A_B8_A_trans_LoadDataWithTranspose.png">
</p>

<p align="center">
图10：int8_t数据类型下，L1 -> L0A转置，调用`LoadDataWithTranspose`数据排布示意图
</p>

由于小分形转置，可调用`LoadDataWithTranspose`接口。下面将介绍如何配置
[`LoadDataWithTranspose`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_0239.html)指令的`LoadData2dTransposeParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

如图10所示，以m轴方向作为外轴进行for循环，如红框所示，以k轴方向作为内轴来配置loadDataParams.repeatTimes。需要特别注意的是，由于转置时连续两个分形合并为一个方块，因此loadDataParams.repeatTimes=CeilDivision(k, fractalShape[0] * fractalNum)，如蓝色框和绿色框分别代表一个方块。


          // LoadDataWithTranspose: Nz-> Zz
          // 根据以下函数原型，offset的数据类型是uint32_t
          // __aicore__ inline LocalTensor operator[](const uint32_t offset) const
          // dstoffset要根据A矩阵在L0上，宽度方向的对齐来求解
          uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
          // srcoffset要根据A矩阵在L1上，高度方向的对齐来求解
          uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

          AscendC::LoadData2dTransposeParams loadDataParams;
          // 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
          loadDataParams.startIndex = 0;
          // 迭代次数,每次迭代转置一个方块矩阵
          loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
          // 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
          loadDataParams.srcStride = 1;
          // 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
          loadDataParams.dstGap = 0;
          // 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
          loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
          for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
              AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
          }
###### 3.1.2.2 输入数据类型为half

**(1) `Load2D`接口**

调用`Load2D`接口如下所示：
<p align="center">
  <img src="img/L1_L0A_F16_A_trans_Load2D.png">
</p>

<p align="center">
图11：half数据类型下，L1 -> L0A转置，调用`Load2D`数据排布示意图
</p>

下面将介绍如何配置
[`Load2D`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00169.html)指令的`LoadData2DParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

如图11所示，以m轴方向作为外轴进行for循环(如图红框部分)，以k轴方向作为内轴来配置loadDataParams.repeatTimes。srcoffset和dstoffset的含义分别是：在L1上，A矩阵m轴方向每循环一次时，LocalTensor的地址偏移量；在L0A上，A矩阵m轴方向每循环一次时, LocalTensor的地址偏移量。loadDataParams.ifTranspose参数配置为true，表示从L1 -> L0A每个小分形进行转置。

          uint32_t dstOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
          uint32_t srcOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
          AscendC::LoadData2DParams loadDataParams;
          // 迭代次数，每个迭代可以处理512B数据
          loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0]);
          // 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔，单位：512B
          loadDataParams.srcStride = 1;
          // 相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位：512B
          loadDataParams.dstGap = 0;
          // 是否启用转置功能，对每个分形矩阵进行转置，默认为false
          loadDataParams.ifTranspose = true;
          for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
              AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
          }
**(2) `LoadDataWithTranspose`接口**

调用`LoadDataWithTranspose`接口的图示与`Load2D`一致，以m轴方向作为外轴进行for循环(如图11红框部分)，以k轴方向作为内轴来配置loadDataParams.repeatTimes，默认小分形转置。


          // dstoffset要根据A矩阵在L0上，宽度方向的对齐来求解
          uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
          // srcoffset要根据A矩阵在L1上，高度方向的对齐来求解
          uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

          AscendC::LoadData2dTransposeParams loadDataParams;
          // 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
          loadDataParams.startIndex = 0;
          // 迭代次数,每次迭代转置一个方块矩阵
          loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
          // 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
          loadDataParams.srcStride = 1;
          // 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
          loadDataParams.dstGap = 0;
          // 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
          loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
          for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
              AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
          }
**(3) `Load3DV2`接口**

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置，配置loadDataParams.enTranspose参数为true，在搬运过程中完成小分形转置。根据`Load3Dv2`指令完成img2col的过程，可知 img2col后A矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为 CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ho * wo,ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。如图所示：

<p align="center">
  <img src="img/L1_L0A_FP16_A_trans_Load3DV2.png">
</p>

<p align="center">
图12：half数据类型下，L1 -> L0A转置，调用`Load3DV2`数据排布示意图
</p>


          // 源操作数height
          loadDataParams.l1H = 1;
          // 源操作数width
          loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
          // 源操作数的通道数，
          // img2col的结果矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
          // img2col的结果矩阵宽度为ci * kh * kw,代入kh=1,kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
          loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
          // 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
          loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
          // 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。
          loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
          // 卷积核在源操作数width维度滑动的步长
          loadDataParams.strideW = 1;
          // 卷积核在源操作数height维度滑动的步长
          loadDataParams.strideH = 1;
          // 卷积核width
          loadDataParams.filterW = 1;
          // 卷积核height
          loadDataParams.filterH = 1;
          // 卷积核width膨胀系数
          loadDataParams.dilationFilterW = 1;
          // 卷积核height膨胀系数
          loadDataParams.dilationFilterH = 1;
          loadDataParams.filterSizeW = false;
          loadDataParams.filterSizeH = false;
          loadDataParams.enTranspose = true;
          loadDataParams.fMatrixCtrl = false;
          AscendC::LoadData(a2Local, a1Local, loadDataParams);
###### 3.1.2.3 输入数据类型为float
float的分形为[16, 8]，需要将2个[16, 8]的分形拼成1个方型进行转置。当A矩阵在L1上数据排布为Nz时，并不满足`LoadDataWithTranspose`指令两个连续分形合并为方块矩阵的要求，如图13所示。当L1上排布为Nz时，`LoadDataWithTranspose`会读2个连续的小分形，如图左边红色框部分，拼成32*8的分形，不满足转置需要按照方型的要求，此时可使用`Load3DV2`实现L1->L0A转置搬运; 当A矩阵在L1上数据排布为Zz时，`LoadDataWithTranspose`会读2个连续的小分形，如图右边红色框部分，拼成16*16的方型，满足转置要求，可使用`LoadDataWithTranspose`指令实现L1->L0A转置搬运。


<p align="center">
  <img src="img/A矩阵L1上为Nz和Zz的区别.png">
</p>

<p align="center">
图13：float数据类型，L1上数据排布格式为Nz和Zz时，`LoadDataWithTranspose`一次读取数据对比图
</p>

**(1) L1上数据排布为Nz，L1-->L0A调用`Load3DV2`**

如图14所示，调用`Load3DV2`指令时，在写入L0A之前会先分别将A矩阵高度和宽度轴向16、8对齐，在转置搬运过程中`Load3DV2`指令会自动填充虚拟数据保证按照方型转置，在写入L0A时虚拟数据的空间会被跳过，如图白色部分，保证最终写入到L0A的A矩阵是Zz排布。

<p align="center">
  <img src="img/L1_L0A_F32_A_trans_Load3DV2.png">
</p>

<p align="center">
图14：float数据类型下，L1 -> L0A转置，调用`Load3DV2`数据排布示意图
</p>


          // 源操作数height
          loadDataParams.l1H = 1;
          // 源操作数width
          loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
          // 源操作数的通道数，
          // img2col的结果矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1w
          // img2col的结果矩阵宽度为ci * kh * kw,代入kh=1,kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
          loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
          // 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
          loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
          // 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t/int4_t，应为16的倍数；覆盖的情况则无倍数要求。
          loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
          // 卷积核在源操作数width维度滑动的步长
          loadDataParams.strideW = 1;
          // 卷积核在源操作数height维度滑动的步长
          loadDataParams.strideH = 1;
          // 卷积核width
          loadDataParams.filterW = 1;
          // 卷积核height
          loadDataParams.filterH = 1;
          // 卷积核width膨胀系数
          loadDataParams.dilationFilterW = 1;
          // 卷积核height膨胀系数
          loadDataParams.dilationFilterH = 1;
          loadDataParams.filterSizeW = false;
          loadDataParams.filterSizeH = false;
          loadDataParams.enTranspose = true;
          loadDataParams.fMatrixCtrl = false;
          AscendC::LoadData(a2Local, a1Local, loadDataParams);
**(2) L1上数据排布为Zz，L1-->L0A调用`LoadDataWithTranspose`**


如图15所示，当L1上的A矩阵满足Zz排布才能满足`LoadDataWithTranspose`指令要求两个连续分形合并为方块矩阵的要求。

<p align="center">
  <img src="img/L1_L0A_F32_A_trans_LoadDataWithTranspose.png">
</p>

<p align="center">
图15：float数据类型下，L1 -> L0A转置，调用`LoadDataWithTranspose`数据排布示意图
</p>

以m轴方向作为外轴进行for循环(如图15红框部分)，以k轴方向作为内轴来配置loadDataParams.repeatTimes。如图15所示，srcoffset和dstoffset的含义分别是：在L1上，A矩阵m轴方向每循环一次时，LocalTensor的地址偏移量；在L0A上，A矩阵m轴方向每循环一次时,LocalTensor的地址偏移量。配置`LoadData2dTransposeParams`结构体时，需要特别注意的是，由于转置时连续两个分形合并为一个方块，如图蓝色和绿色方框表示2个方块，因此loadDataParams.repeatTimes=CeilDivision(k, fractalShape[1] * fractalNum)。


          // A矩阵在L0A上shape为[m，k]、Zz排布，因此dstoffset要根据A矩阵在K轴方向的对齐来求解
          uint32_t dstOffset = CeilDivision(k, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
          // A矩阵在L1上shape为[K,M]、Zz排布，因此srcoffset为1个分形包含的元素个数
          uint32_t srcOffset = fractalSize * fractalNum;

          AscendC::LoadData2dTransposeParams loadDataParams;
          // 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
          loadDataParams.startIndex = 0;
          // 迭代次数,每次迭代转置一个方块矩阵
          loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1] * fractalNum);
          // 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
          loadDataParams.srcStride = CeilDivision(m, fractalShape[1] * fractalNum);
          // 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
          loadDataParams.dstGap = 1;
          // 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
          loadDataParams.dstFracGap = 0;
          for (int i = 0; i < CeilDivision(m, fractalShape[1] * fractalNum); ++i) {
              AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
          }

#### 3.2. B矩阵L1->L0B
B4 / B8 / B16 / B32这4种输入数据类型在B矩阵从L1搬运到L0B时，转置和不转置场景下可使用的`LoadData`相关指令不同，详见[表2](#表2)。下面分别介绍这些场景。
##### 3.2.1. B矩阵L1->L0B不转置
L1 -> L0B不转置时仅大分形排布格式变换。该场景下，B4 / B8 / B16 / B32这4种数据类型均可使用`Load2D`接口实现数据搬运，参数配置基本相同，仅fractalShape有区别，可参考[表5](#表5)。以half为例展示图示。

<p align="center">
  <img src="img/L1_L0B_F16_B_Load2D.png">
</p>

<p align="center">
图16：half数据类型下，L1 -> L0B不转置，调用`Load2D`数据排布示意图
</p>

[`Load2D`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00169.html)指令的`LoadData2DParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

如图16所示，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes。srcoffset和dstoffset的含义分别是：在L1上，B矩阵K轴方向每循环一次时，LocalTensor的地址偏移量；在L0B上，B矩阵K轴方向每循环一次时，LocalTensor的地址偏移量。由于L1上的B矩阵与L0B上的B矩阵等价，因此srcOffset和dstOffset相同。

        // srcOffset和dstOffset相同
        // n轴向fractalShape[0]对齐
        uint32_t dstOffset = CeilDivision(n, fractalShape[0]) * fractalSize;
        uint32_t srcOffset = CeilDivision(n, fractalShape[0]) * fractalSize;
        // Nz -> Zz
        AscendC::LoadData2DParams loadDataParams;
        loadDataParams.repeatTimes = CeilDivision(n, fractalShape[0]);
        loadDataParams.srcStride = 1;
        // n轴方向相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔
        loadDataParams.dstGap = 0;
        loadDataParams.ifTranspose = false;
        // k轴向fractalShape[1]对齐
        for (int i = 0; i < CeilDivision(k, fractalShape[1]); ++i) {
            AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
        }


##### 3.2.2. B矩阵L1->L0B转置
L1->L0B转置时大分形排布格式变换，且小分形需要转置。该场景下，B4 / B8 / B16 / B32这4种数据类型均可使用的接口不同，按照数据类型以int4_t、int8_t、half、float为例，分小节分别介绍不同数据类型可使用的接口及图示说明。

###### 3.2.2.1 输入数据类型为int4_t/int8_t
输入数据类型为int4_t/int8_t，且L1->L0B需要转置时，可使用的接口都是`LoadDataWithTranspose`，参数配置基本相似，仅fractalShape、fractalNum、fractalSize不同。可参考[表5](#表5)，本节以int8_t为例进行图示介绍

<p align="center">
  <img src="img/L1_L0B_B8_B_trans_LoadDataWithTranspose.png" width="800">
</p>

<p align="center">
图17：int8_t数据类型下，L1 -> L0B转置，调用`LoadDataWithTranspose`数据排布示意图
</p>

由于小分形转置，因此可以调用`LoadDataWithTranspose`接口。下面将介绍如何配置
[`LoadDataWithTranspose`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_0239.html)指令的`LoadData2dTransposeParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

如图17所示，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes。需要特别注意的是，由于转置时连续两个分形合并为一个方块，因此loadDataParams.repeatTimes=CeilDivision(k, fractalShape[0] * fractalNum)。另外，如图17所示，L0A中转置前处于同一方块中的两个分形在L1中是连续的，转置后两个分形依然是连续的，因此前一个分形结束地址与后一个分形起始地址的间隔为0。

            uint32_t dstOffset = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
            uint32_t srcOffset = fractalSize * fractalNum;
            AscendC::LoadData2dTransposeParams loadDataParams;
            loadDataParams.startIndex = 0;
            loadDataParams.repeatTimes = CeilDivision(n, fractalShape[1]);
            loadDataParams.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
            loadDataParams.dstGap = 1;
            loadDataParams.dstFracGap = 0;
            for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
                AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
            }
###### 3.2.2.2 输入数据类型为half
输入数据类型为half，且L1->L0B需要转置时，可使用的接口是`Load2D`、`LoadDataWithTranspose`、`Load3DV2`，下面分别介绍这3种接口的调用方式及图示说明。


**(1) 调用`Load2D`**
<p align="center">
  <img src="img/L1_L0B_FP16_B_trans_Load2D.png" width="800">
</p>

<p align="center">
图18：half数据类型下，L1 -> L0B转置，调用`Load2D`数据排布示意图
</p>

根据[表2](#表2)可知，当B矩阵转置时，b矩阵也转置，当且仅当输入数据类型为half时，可以调用`LoadData`指令完成转置。

如上图18所示，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes。结合图示，并根据前述srcoffset和dstoffset的定义，可以得出如下配置信息。

          uint32_t dstOffset = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
          uint32_t srcOffset = fractalSize * fractalNum;
          // Nz -> Zn
          AscendC::LoadData2DParams loadDataParams;
          loadDataParams.repeatTimes = CeilDivision(n, fractalShape[0] * fractalNum);
          loadDataParams.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
          loadDataParams.dstGap = 0;
          loadDataParams.ifTranspose = true;
          for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
              AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
          }
**(2) 调用`LoadDataWithTranspose`**

调用`LoadDataWithTranspose`接口的图示与`Load2D`一致，以k轴方向作为外轴进行for循环，以n轴方向作为内轴来配置loadDataParams.repeatTimes，默认小分形转置。


          // LoadDataWithTranspose: Nz -> Zn
          uint32_t dstOffset = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
          uint32_t srcOffset = fractalSize * fractalNum;
          AscendC::LoadData2dTransposeParams loadDataParams;
          loadDataParams.startIndex = 0;
          loadDataParams.repeatTimes = CeilDivision(n, fractalShape[1]);
          loadDataParams.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
          loadDataParams.dstGap = 0;
          loadDataParams.dstFracGap = 0;
          for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
              AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
          }

**(3) 调用`Load3DV2`**


<p align="center">
  <img src="img/L1_L0B_FP16_B_trans_Load3DV2.png">
</p>

<p align="center">
图19：half数据类型下，L1 -> L0B转置，调用`Load3DV2`数据排布示意图
</p>

`Load3D`的本质是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置，配置loadDataParams.enTranspose参数为true，在搬运过程中完成小分形转置。根据`Load3Dv2`指令完成img2col的过程，可知 img2col后B矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：B矩阵的高度为 CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ho * wo,ci * kh * kw，代入kh=1，kw=1，可知B矩阵的宽度为CeilAlign(n, fractalShape[1])。



          // Load3dV2: Nz -> Zn
          AscendC::LoadData3DParamsV2<T> loadDataParams;
          loadDataParams.l1H = 1;
          loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
          loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
          loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
          loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
          loadDataParams.strideW = 1;
          loadDataParams.strideH = 1;
          loadDataParams.filterW = 1;
          loadDataParams.filterH = 1;
          loadDataParams.dilationFilterW = 1;
          loadDataParams.dilationFilterH = 1;
          loadDataParams.filterSizeW = false;
          loadDataParams.filterSizeH = false;
          // 对于Load3Dv2接口，当目的地址为L0B时，B矩阵会自动转置
          loadDataParams.enTranspose = true;
          loadDataParams.fMatrixCtrl = false;
          AscendC::LoadData(b2Local, b1Local, loadDataParams);


###### 3.2.2.3 输入数据类型为float
与3.1.2.3小节类似，当输入数据类型为 float 且小分形需要转置时，若 L1 上的数据排布为 Nz 或 Zz，可调用的指令会有所差异。

**(1) L1上数据排布为Nz，L1-->L0B调用`Load3DV2`**

如图20所示，调用`LoadData3DV2`指令时，在写入L0B之前会先分别将B矩阵高度和宽度轴向16、8对齐，在转置搬运过程中`Load3DV2`指令会自动填充虚拟数据保证按照方型转置，在写入L0B时虚拟数据的空间会被跳过，如图白色部分，保证最终写入到L0B的B矩阵是Zn排布。

对齐要求方面，在L1上B矩阵的k轴向fS[0] * fractalNum对齐、n轴向fS[1]对齐；在L0B上B矩阵的k轴向fS[0] * fractalNum对齐、n轴向fS[1]对齐。
<p align="center">
  <img src="img/L1_L0B_FP32_B_trans_Load3DV2.png">
</p>

<p align="center">
图20：float数据类型下，L1 -> L0B转置，调用`Load3DV2`数据排布示意图
</p>

下面将介绍如何配置
[`Load3Dv2`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_00170.html)指令的`LoadData3DParamsV2`结构体的成员，各个成员变量的具体含义这里不再赘述。

根据`Load3Dv2`指令完成img2col的过程，可知 img2col后B矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：B矩阵的高度为 CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ho * wo,ci * kh * kw,代入kh=1,kw=1，可知B矩阵的宽度为CeilAlign(n, fractalShape[1])。需要注意的是 loadDataParams.enTranspose 配置仅仅对A矩阵有效，因此取值为true或者false不会影响功能，B矩阵默认转置。

            loadDataParams.l1H = 1;
            loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
            loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
            loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
            loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
            loadDataParams.strideW = 1;
            loadDataParams.strideH = 1;
            loadDataParams.filterW = 1;
            loadDataParams.filterH = 1;
            loadDataParams.dilationFilterW = 1;
            loadDataParams.dilationFilterH = 1;
            loadDataParams.filterSizeW = false;
            loadDataParams.filterSizeH = false;
            loadDataParams.enTranspose = true;
            loadDataParams.fMatrixCtrl = false;
**(2) L1上数据排布为Zz，L1-->L0B调用`LoadDataWithTranspose`**


如图21所示，当L1上的B矩阵满足Zz排布才能满足`LoadDataWithTranspose`指令要求两个连续分形合并为方块矩阵的要求。

<p align="center">
  <img src="img/L1_L0B_FP32_B_trans_LoadDataWithTranspose.png" width="800">
</p>

<p align="center">
图21：float数据类型下，L1 -> L0B转置，调用`LoadDataWithTranspose`数据排布示意图
</p>

配置`LoadData2dTransposeParams`结构体时，需要特别注意的是，由于转置时连续两个分形合并为一个方块，因此loadDataParams.repeatTimes=CeilDivision(n, fractalShape[1] * fractalNum)。另外，由于在目的操作数中同属一个方块矩阵的两个分形地址不再连续，此时参数loadDataParams.dstFracGap = CeilDivision(n, fractalShape[0]) - 1。

                // B矩阵在L0B上shape为[k, n]、Zn排布，因此dstoffset要根据B矩阵在N轴方向的对齐来求解
                uint32_t dstOffset = CeilDivision(n, fractalShape[0]) * fractalSize * fractalNum;
                // B矩阵在L1上shape为[K,N]、ZZ排布，因此srcoffset要根据B矩阵在N轴方向的对齐来求解
                uint32_t srcOffset = CeilDivision(n, fractalShape[1] * fractalNum) * fractalSize * fractalNum;

                AscendC::LoadData2dTransposeParams loadDataParams;
                // 搬运起始位置为源操作数中第几个方块矩阵(0 为源操作数中第1个方块矩阵)
                loadDataParams.startIndex = 0;
                // 迭代次数,每次迭代转置一个方块矩阵
                loadDataParams.repeatTimes = CeilDivision(n, fractalShape[1] * fractalNum);
                // 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位是方块矩阵的大小
                loadDataParams.srcStride = 1;
                // 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512B
                loadDataParams.dstGap = 0;
                // 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512B
                loadDataParams.dstFracGap = CeilDivision(n, fractalShape[0]) - 1;
                for (int i = 0; i < CeilDivision(k, fractalShape[0]); ++i) {
                    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
                }
### 4.矩阵乘 (`Mmad`)
下面将介绍如何配置
[`Mmad`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_0249.html)指令的`MmadParams`结构体的成员，各个成员变量的具体含义这里不再赘述。

需要注意的是当`Mmad`指令执行时，
矩阵计算单元会从L0A/L0B连续读入多个分形参与矩阵乘计算，读入分形的数量根据`MmadParams`结构体的成员变量m、n、k的取值以及`Mmad`指令对L0A/L0B上A矩阵和B矩阵各个轴的对齐要求来计算的。由于`Mmad`指令，即A矩阵分形为[16, 32]、B矩阵分形为[32, 16]来连续读入分形的，也就是说矩阵计算单元从L0A/L0B连续读入的分形总数目分别为：CeilDivision(m, 16) * CeilDivision(k, 32)、CeilDivision(k, 32) * CeilDivision(n, 16)。


因此当L0A/L0B上对A矩阵和B矩阵在各个轴的实际对齐要求与`Mmad`指令默认的对齐要求不一致时，就可能导致连续读入分形时，错误读入完全由无效数据填充的分形而忽略了包含有效数据的分形。

如下图22所示，以场景5为例，输入数据类型为int8_t，A转置输入、B矩阵不转置输入。由前文内容可知：L0A在m轴和k轴分别向16 * 2、32对齐，L0B在k轴和n轴分别向32、16 * 2对齐，而`Mmad`指令默认在m、k、n三个轴的对齐要求分别是向16、32、16对齐，假设n=70时，按照L0上的对齐要求nAlignL0 = CeilAlign(70, 16 * 2) = 96, 按照`Mmad`指令默认的对齐要求，nAlignL0 = CeilAlign(70, 16) = 80，此时n轴实际对齐要求与`Mmad`指令默认的对齐要求不一致。m轴同理。

以B矩阵n轴为例，如图22左边子图所示，如果设置mmadParams.n = n = 70,就会导致读入编号为5的分形，同时没能将包含有效数据的编号为10的分形。

如图22右边子图所示，如果设置mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum) = 96,此时会读入全部分形，虽然矩阵计算结果中包含了无效数据参与计算的结果，但是在`Fixpipe`指令搬出数据时通过设置fixpipeParams.nSize = n来保证无效数据参与计算的结果不会被搬出。m轴参数配置同理。

<p align="center">
  <img src="img/B_B8_read_invalid_data.png" width="1200">
</p>

<p align="center">
图22：B矩阵GM不转置输入，int8_t数据类型下，n轴实际对齐要求与`Mmad`指令默认的对齐要求不一致
</p>


上述场景类似，如场景13，当输入数据类型为float、A矩阵GM转置输入([k, m])时，k轴实际对齐要求与`Mmad`指令默认的对齐要求也不一致，但是此种场景下的解决方法与上述场景有所不同，需要单独引入mmadParams.kDirectionAlign参数来解决，下面说明原因。

根据矩阵乘法的计算公式可知，k轴作为A、B矩阵公共的维度，此时如果像上述场景那样设置mmadParams.k = CeilAlign(k, fractalShape[1] * fractalNum) = CeilAlign(70, 16) = 80，会导致C矩阵中每个元素的数值都受到多读入的无效数据的影响，并且也不能通过设置fixpipeParams的参数来保证无效数据参与计算的结果不会被搬出。

如下图23所示，mmadParams.kDirectionAlign仅在输入数据类型为float时生效。当A矩阵转置时，该参数需要设置为真，此时L0A上A矩阵在k方向向16对齐，矩阵计算单元从L0A读取数据会跳过填充的无效数据，其余场景下该参数取默认值为假，此时L0A上A矩阵在k方向向8对齐。


<p align="center">
  <img src="img/Mmad_FP32_A_trans_kDirectionAlign.png" width="1000">
</p>

<p align="center">
图23：A矩阵GM转置输入，float数据类型下，k轴实际对齐要求与`Mmad`指令默认的对齐要求不一致
</p>

        AscendC::MmadParams mmadParams;
        // 左矩阵Height
        mmadParams.m = m;
        // 右矩阵width
        mmadParams.n = n;
        if constexpr (AscendC::IsSameType<T, int8_t>::value && AscendC::IsSameType<U, int32_t>::value) {
            if constexpr (!isBtranspose) {
                // mmad默认n轴向16对齐，但是由于b转置过程n轴向2 * 16对齐，填充了一个全部由无效数据的32 * 16的分形，
                // 如果仍然设置mmadParams.n = n，cube单元会多读入无效数据的分形同时有效数据的分形也未被读入。
                // 此时可以通过设置n向32对齐，让此分形参与计算，搬出时跳过无效分形参与计算的得到的分形即可
                mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum);
            }
        }
        // 左矩阵Width、右矩阵Height
        mmadParams.k = k;
        if constexpr (AscendC::IsSameType<T, float>::value && AscendC::IsSameType<U, float>::value) {
            if constexpr (isAtranspose) {
                mmadParams.kDirectionAlign = true;
            }
        }

### 5.L0C到GM (`Fixpipe`)
下面将介绍如何配置
[`Fixpipe`](https://www.hiascend.com/document/detail/zh/canncommercial/850/API/ascendcopapi/atlasascendc_api_07_0251.html)指令的`FixpipeParamsV220`结构体的成员，各个成员变量的具体含义这里不再赘述。其中，fixpipeParams.srcStride的单位是32/sizeof(T)个元素，其含义是源Nz矩阵中相邻Z排布的起始地址偏移(ND矩阵中同一行的元素在源Nz矩阵中处于相邻的Z排布)，该参数的取值是L0C上C矩阵m轴向16对齐后的长度。

        AscendC::FixpipeParamsV220 fixpipeParams;
        fixpipeParams.nSize = n;
        fixpipeParams.mSize = m;

        // 源操作数来源于L0c，因此m只需要向16对齐，与数据类型无关
        // 源NZ矩阵中相邻Z排布的起始地址偏移
        fixpipeParams.srcStride = CeilAlign(m, fractalShape[0]);
        fixpipeParams.dstStride = n;

        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真 模式时，添加`-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 参数 | 说明 | 可选值 | 默认值 |
  |------|------|--------|--------|
  | CMAKE_ASC_RUN_MODE | 运行模式 | npu, sim | npu |
  | CMAKE_ASC_ARCHITECTURES | NPU硬件架构 | dav-2201| dav-2201 |
  | SCENARIO_NUM | 场景编号 | 1-14 | 1 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```