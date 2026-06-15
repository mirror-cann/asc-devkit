# MmadBitMode<a name="ZH-CN_TOPIC_000000253823118101"></a>

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

MmadBitMode对于MmadParams结构体构造进行了优化，本接口适用于scalar流水成为性能优化瓶颈的场景，支持基础Mmad/MmadMx计算功能。本接口与Mmad/MmadMx接口的差异在于参数传入的方式不同，本接口传入的是联合结构体MmadBitModeParams。MmadBitModeParams类参数设计思想说明：

联合体（union）是一种特殊的数据结构，允许在相同的内存位置存储不同的数据类型。union的所有成员共享同一块内存空间，大小由最大成员决定，同一时间只能使用一个成员。

位域（bit-field）是一种特殊的类成员，允许精确控制结构体中成员变量所占用的内存位数。结构体中成员变量从上到下对应内存中从低位到高位。

MmadBitModeParams类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数，减少了scalar计算，实现性能提升。

MmadBitModeParams类可以直接使用MmadBitModeParams结构体类型对象初始化：

```cpp
__aicore__ inline MmadBitModeParams(const MmadBitModeParams &mmadParams_);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0直接进行逐bit位修改来修改参数。

## 函数原型

- 不传入bias

  ```cpp
  template <typename T, typename U, typename S>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)
  ```
- 传入bias

  ```cpp
  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)
  ```

## 参数说明

**表1**  参数说明


| 参数名称   | 输入/输出 | 含义                                                                                                                                                                                                                                                                      |
| ---------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst        | 输出      | 目的操作数，结果矩阵c，类型为LocalTensor，支持的物理存储位置为L0C Buffer（TPosition:CO1）。<br><br>LocalTensor的起始地址需要按照1024字节对齐。                                                                                                                            |
| fm         | 输入      | 源操作数，左矩阵a，类型为LocalTensor，支持的物理存储位置为L0A Buffer（TPosition:A2）。<br><br>左矩阵a对应的scale矩阵起始地址为：A矩阵起始对应地址/16。<br><br>对于fp4场景LocalTensor的起始地址需要按照512字节对齐。对于fp8场景LocalTensor的起始地址需要按照1024字节对齐。 |
| filter     | 输入      | 源操作数，右矩阵b，类型为LocalTensor，支持的物理存储位置为L0B Buffer（TPosition:B2）。<br><br>右矩阵b对应的scale矩阵起始地址为：B矩阵起始对应地址/16。<br><br>对于fp4场景LocalTensor的起始地址需要按照512字节对齐。对于fp8场景LocalTensor的起始地址需要按照1024字节对齐。 |
| bias       | 输入      | 源操作数，bias矩阵，类型为LocalTensor，支持的物理存储位置为BT Buffer（TPosition:C2）。<br><br>LocalTensor的起始地址需要按照64字节对齐。                                                                                                                                   |
| mmadParams | 输入      | 矩阵乘相关参数。该参数类型的具体定义请参考\${INSTALL_DIR}/include/ascendc/basic_api/interface/kernel_struct_mm.h，${INSTALL_DIR}请替换为CANN软件安装后文件存储路径。<br><br>MmadBitModeParams参数说明请参考下表。                                                         |

**表2**  MmadBitModeParams类参数说明


| 参数名称       | 含义                                                                                                                                                      |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| config0        | uint64_t类型，与MmadBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | MmadBitModeConfig0位域（bit-field）结构体类型，参数参考[表3](#zh_cn_topic_mmadbitmode_section4_table3)，与config0组成联合体（union）。                    |

**表3**  MmadBitModeConfig0结构体参数说明<a id="zh_cn_topic_mmadbitmode_section4_table3"></a>


| 参数名称       | 含义                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m              | 左矩阵Height，取值范围：m∈[0，4095]。默认值为0。<br><br>该参数是位域结构体的最低位参数，占用12bit，可以使用MmadBitModeParams类对象的SetM()函数设置其值，使用GetM()函数获取其值。                                                                                                                                                                                                                                                     |
| k              | 左矩阵Width、右矩阵Height，取值范围：k∈[0，4095]。默认值为0。<br><br>该参数是位域结构体的第二低位参数，占用12bit，可以使用MmadBitModeParams类对象的SetK()函数设置其值，使用GetK()函数获取其值。                                                                                                                                                                                                                                      |
| n              | 右矩阵Width，取值范围：n∈[0，4095]。默认值为0。<br><br>该参数是位域结构体的第三低位参数，占用12bit，可以使用MmadBitModeParams类对象的SetN()函数设置其值，使用GetN()函数获取其值。                                                                                                                                                                                                                                                    |
| unitFlag       | 预留参数。为后续的功能做保留，开发者暂时无需关注，使用默认值即可。<br><br>该参数是位域结构体的第四低位参数，占用2bit，可以使用MmadBitModeParams类对象的SetUnitFlag()函数设置其值，使用GetUnitFlag()函数获取其值。                                                                                                                                                                                                                     |
| disableGemv    | M = 1时，用于配置Mmad计算是否开启GEMV。当输入为false时，表示开启GEMV；反之，输入为true时，表示关闭GEMV。<br><br>GEMV（General Matrix-Vector Multiplication）表示实现矩阵和向量的乘积，开启GEMV后，Mmad API从L0A Buffer读取数据时，数据将以ND格式进行读取，而不会将其视为ZZ格式。<br><br>该参数是位域结构体的第五低位参数，占用1bit，可以使用MmadBitModeParams类对象的SetDisableGemv()函数设置其值，使用GetDisableGemv()函数获取其值。 |
| cmatrixSource  | 配置C矩阵初始值是否来源于C2（存放Bias的硬件缓冲区）。默认值为false。<br>false：来源于CO1；<br>true：来源于C2。<br><br>注意：带bias输入的接口配置该参数无效，会根据bias输入的位置来判断C矩阵初始值是否来源于CO1还是C2。<br><br>该参数是位域结构体的第六低位参数，占用1bit，可以使用MmadBitModeParams类对象的SetCmatrixSource()函数设置其值，使用GetCmatrixSource()函数获取其值。                                                       |
| cmatrixInitVal | 配置C矩阵初始值是否为0。默认值true。<br>true：C矩阵初始值为0；false：C矩阵初始值通过cmatrixSource参数进行配置。<br><br>该参数是位域结构体的最高位参数，占用1bit，可以使用MmadBitModeParams类对象的SetCmatrixInitVal()函数设置其值，使用GetCmatrixInitVal()函数获取其值。                                                                                                                                                              |

## 数据类型<a id="zh_cn_topic_mmadbitmode_section_datatype"></a>

**表4**  Mmad接口左矩阵、右矩阵、Bias矩阵、结果矩阵支持的精度类型组合


| 左矩阵fm type | 右矩阵filter type | bias type | 结果矩阵dst type |
| ------------- | ----------------- | --------- | ---------------- |
| int8_t        | int8_t            | int32_t   | int32_t          |
| half          | half              | float     | float            |
| float         | float             | float     | float            |
| bfloat16_t    | bfloat16_t        | float     | float            |
| fp8_e4m3fn_t  | fp8_e4m3fn_t      | float     | float            |
| fp8_e4m3fn_t  | fp8_e5m2_t        | float     | float            |
| fp8_e5m2_t    | fp8_e4m3fn_t      | float     | float            |
| fp8_e5m2_t    | fp8_e5m2_t        | float     | float            |
| hifloat8_t    | hifloat8_t        | float     | float            |

**表5**  MmadMx接口左矩阵、右矩阵、Scale矩阵、Bias矩阵、结果矩阵支持的精度类型组合


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
- 地址约束说明请参考表1。
- 当M、K、N中的任意一个值为0时，表示指令不会执行，该接口将被视为NOP（空操作）。
- K需要是64的倍数。
- 对于fp4场景a/b矩阵的起始地址需要按照512字节对齐，对于fp8场景a/b矩阵的起始地址需要按照1024字节对齐。
- 左矩阵a/b对应的scale矩阵起始地址为：a/b矩阵起始对应地址/16。
- 其他特殊场景约束可参考[Mmad接口约束说明](Mmad.md#约束说明)。

## 调用示例

完整矩阵乘流程请参考[Mmad样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad)。MmadBitMode的使用可以参考下面的调用示例。


- 示例一：使用MmadParams构造MmadBitModeParams

```cpp
  constexpr uint32_t M = 32;
  constexpr uint32_t K = 64;
  constexpr uint32_t N = 32;

  // A矩阵，L0A Buffer，元素个数为M*K=2048。
  AscendC::LocalTensor<half> a2Local(AscendC::TPosition::A2, 0, M * K);
  // B矩阵，L0B Buffer，元素个数为K*N=2048。
  AscendC::LocalTensor<half> b2Local(AscendC::TPosition::B2, 0, K * N);
  // C矩阵输出到L0C Buffer，元素个数为M*N=1024。
  AscendC::LocalTensor<float> co1Local(AscendC::TPosition::CO1, 0, M * N);

  AscendC::MmadParams mmParams;
  mmParams.m = M;
  mmParams.k = K;
  mmParams.n = N;
  mmParams.unitFlag = 0;
  mmParams.disableGemv = false;
  mmParams.cmatrixSource = false;
  mmParams.cmatrixInitVal = true;

  // 使用MmadParams构造MmadBitModeParams。
  AscendC::MmadBitModeParams bitModeParams(mmParams);
  AscendC::Mmad(co1Local, a2Local, b2Local, bitModeParams);
```

- 示例二：使用Set函数修改MmadBitModeParams

```cpp

  constexpr uint32_t M = 32;
  constexpr uint32_t K = 64;
  constexpr uint32_t N = 32;

  // A矩阵，L0A Buffer，元素个数为M*K=2048。
  AscendC::LocalTensor<half> a2Local(AscendC::TPosition::A2, 0, M * K);
  // B矩阵，L0B Buffer，元素个数为K*N=2048。
  AscendC::LocalTensor<half> b2Local(AscendC::TPosition::B2, 0, K * N);
  // C矩阵输出到L0C Buffer，元素个数为M*N=1024。
  AscendC::LocalTensor<float> co1Local(AscendC::TPosition::CO1, 0, M * N);

  // 默认构造时config0清零，通过Set函数逐项写入矩阵乘参数。
  AscendC::MmadBitModeParams bitModeParams;
  bitModeParams.SetM(M);
  bitModeParams.SetK(K);
  bitModeParams.SetN(N);
  bitModeParams.SetUnitFlag(0);
  bitModeParams.SetDisableGemv(false);
  bitModeParams.SetCmatrixSource(false);
  bitModeParams.SetCmatrixInitVal(true);

  AscendC::Mmad(co1Local, a2Local, b2Local, bitModeParams);
```
