# L0C到GM数据搬运（Fixpipe）<a id="ZH-CN_TOPIC_0000002569070977"></a>

## 产品支持情况<a id="zh-cn_topic_0000002542828493_section796754519912"></a>

| 产品 | 是否支持 |
| ---------- | :----------: |
| <cann-filter npu-type = "950">Ascend 950PR/Ascend 950DT | 支持包含FixpipeParamsV220/FixpipeParamsArch3510参数的接口。 </cann-filter> |
| <cann-filter npu-type = "A3">Atlas A3 训练系列产品/Atlas A3 推理系列产品 | 仅支持包含FixpipeParamsV220参数的接口。 </cann-filter> |
| <cann-filter npu-type = "910b">Atlas A2 训练系列产品/Atlas A2 推理系列产品 | 仅支持包含FixpipeParamsV220参数的接口。 </cann-filter> |
| <cann-filter npu-type = "310b">Atlas 200I/500 A2 推理产品 | 仅支持包含FixpipeParamsM300参数的接口。 </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品AI Core | x </cann-filter> |
| <cann-filter npu-type = "310p">Atlas 推理系列产品Vector Core | x </cann-filter> |
| <cann-filter npu-type = "910">Atlas 训练系列产品 | x </cann-filter> |
| <cann-filter npu-type = "x90">Kirin X90 | √ </cann-filter> |
| <cann-filter npu-type = "9030">Kirin 9030 | √ </cann-filter> |

## 功能说明<a id="zh-cn_topic_0000002542828493_section106841136114319"></a>

头文件路径为：basic_api/kernel_operator_fixpipe_intf.h、basic_api/kernel_operator_data_copy_intf.h。

矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至Global Memory（GM）中，并且在搬运过程中支持随路格式转换等操作。

<cann-filter npu-type = "A3,910b">

以如下产品型号为例：

<cann-filter npu-type = "A3">

Atlas A3 训练系列产品/Atlas A3 推理系列产品；

</cann-filter>

<cann-filter npu-type = "910b">

Atlas A2 训练系列产品/Atlas A2 推理系列产品；

</cann-filter>

下图展示了随路量化、随路ReLU、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为非量化模式，仅为Cast，其余为随路scalar/tensor量化模式。

**图 1** L0C2GM流程图<a id="zh-cn_topic_0000002542828493_fig542810249417"></a>  

![](../../../../figures/L0C2GM_Function_Combination.png)

</cann-filter>

## 函数原型<a id="zh-cn_topic_0000002542828493_section82039854412"></a>

为了方便用户使用，减少学习理解成本，针对用户经常使用的场景组合，提供了矩阵搬出的组合接口Fixpipe，接口内包含了设置寄存器与数据搬运能力，对应的接口如下：

**传入FixpipeParamsArch3510参数的接口：**

- 通路L0C Buffer（CO1）-\>GM，不开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- 通路L0C Buffer（CO1）-\>GM，开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**传入FixpipeParamsV220参数的接口：**

- 通路L0C Buffer（CO1）-\>GM，不开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- 通路L0C Buffer（CO1）-\>GM，开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

**传入FixpipeParamsM300参数的接口：**

- 通路L0C Buffer（CO1）-\>GM，不开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300& intriParams)
    ```

- 通路L0C Buffer（CO1）-\>GM，开启随路[tensor量化](./关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsM300& intriParams)
    ```

## 参数说明<a id="zh-cn_topic_0000002542828493_section16128134420472"></a>

**表 1** Fixpipe模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T/U | 目的操作数/源操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002542828493_section4219135304818)。 |
| config | Fixpipe相关配置参数，类型为FixpipeConfig。取值如下：<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR（默认取值）**：开启NZ2ND，输出数据格式为ND格式。<br>&nbsp;&nbsp;&bull; CFG_NZ：NZ2NZ，输出数据格式为NZ格式。<br><cann-filter npu-type = "950">&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR：针对Ascend 950PR/Ascend 950DT，开启NZ2DN，输出数据格式为DN格式。<br></cann-filter><pre>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // 用于用户指定目的地址的位置是否是UB <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // 输出数据格式仍为NZ格式。<br>    ROW_MAJOR, // 开启NZ2ND，输出数据格式为ND格式。<br><cann-filter npu-type = "950">    COLUMN_MAJOR, // 仅Ascend 950PR/Ascend 950DT支持，开启NZ2DN，输出数据格式为DN格式。<br></cann-filter>};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<cann-filter npu-type = "950"><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // 仅Ascend 950PR/Ascend 950DT支持</cann-filter></pre> |
| S | 参数cbufWorkspace的数据类型，即随路量化参数的数据类型。<br>&nbsp;&nbsp;&bull; 当目的操作数、源操作数、cbufWorkspace使用基础数据类型时，模板参数S必须为uint64_t类型，否则编译失败。<br>&nbsp;&nbsp;&bull; 当目的操作数、源操作数、cbufWorkspace使用TensorTrait类型时，模板参数S的LiteType必须为uint64_t类型，否则编译失败。<br>模板参数S后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表 2** Fixpipe参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为GlobalTensor。数据格式为NZ、ND格式。ND地址需要满足1字节对齐，NZ地址需满足32字节对齐。<cann-filter npu-type = "950"><br>针对Ascend 950PR/Ascend 950DT，还支持数据格式为DN，地址需满足32字节对齐。</cann-filter> |
| src | 输入 | 源操作数，类型为LocalTensor，支持的物理地址为L0C Buffer（TPosition为CO1），为Mmad接口计算的结果。数据格式为NZ格式，地址需要满足6对齐。 |
| intriParams | 输入 | Fixpipe搬运参数，具体定义请参考 "basic_api/interface/kernel_struct_fixpipe.h"。<br>参数说明请参考Fixpipe搬运参数（FixpipeParamsArch3510、FixpipeParamsV220、FixpipeParamsM300）结构体说明。 |
| cbufWorkspace | 输入 | 量化参数，类型为`LocalTensor<uint64_t>`，支持的物理地址为L1 Buffer（TPosition为C1），地址需满足32字节对齐。<br>&nbsp;&nbsp;&bull; 当quantPre为VDEQF16、VQF322B8_PRE、VREQ8时支持。<br><cann-filter npu-type = "950">&nbsp;&nbsp;&bull; 针对Ascend 950PR/Ascend 950DT，除上述外还有VQF322FP8_PRE、VQF322HIF8_PRE、VQF322HIF8_PRE_HYBRID、VQS322BF16_PRE、VQF322F16_PRE、VQF322BF16_PRE、VQF322F32_PRE支持。<br></cann-filter>quantPre介绍请参考Fixpipe搬运参数结构体中quantPre部分。 |

**表 3** Fixpipe搬运参数（FixpipeParamsArch3510）结构体说明

| 参数名称 | 数据类型 | 含义 |
| ---------- | ---------- | ---------- |
| nSize | 输入 | 源NZ矩阵在N方向上的大小，取值范围为nSize ∈[0, 4095]。<br>&nbsp;&nbsp;&bull; 若开启channelSplit功能，nSize必须为8的倍数。<br>&nbsp;&nbsp;&bull; 若不开启channelSplit功能，nSize必须为16的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 输入 | 源NZ矩阵在M方向上的大小，取值范围为mSize∈[0, 65535]。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]， 单位为C0_Size（16\*sizeof(T)，T为src的数据类型）。 |
| dstStride | 输入 | &bull; 不开启NZ2ND功能：目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为element。<br>&nbsp;&nbsp;&bull; 开启NZ2ND/NZ2DN功能：目的ND矩阵每一行中的元素个数，取值不为0，单位为element。 |
| quantPre | 输入 | QuantMode_t是一个枚举类型，用于控制量化模式，默认值为QuantMode_t::NoQuant，即不开启量化功能。QuantMode_t取值如下：<br>&nbsp;&nbsp;&bull; NoQuant，不开启量化功能；<br>&nbsp;&nbsp;&bull; F322F16，float cast成half，cast mode为CAST_RINT模式；<br>&nbsp;&nbsp;&bull; F322BF16，float cast成bfloat16_t，cast mode为CAST_RINT模式；<br>&nbsp;&nbsp;&bull; DEQF16，int32_t量化成half，scalar量化；<br>&nbsp;&nbsp;&bull; VDEQF16，int32_t量化成half，tensor量化；<br>&nbsp;&nbsp;&bull; QF322B8_PRE，float量化成uint8_t/int8_t，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322B8_PRE，float量化成uint8_t/int8_t，tensor量化；<br>&nbsp;&nbsp;&bull; REQ8，int32_t量化成uint8_t/int8_t，scalar量化；<br>&nbsp;&nbsp;&bull; VREQ8，int32_t量化成uint8_t/int8_t，tensor量化；<br>&nbsp;&nbsp;&bull; QF322FP8_PRE，float量化成fp8_e4m3fn_t，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322FP8_PRE，float量化成fp8_e4m3fn_t，tensor量化；<br>&nbsp;&nbsp;&bull; QF322HIF8_PRE，float量化成hifloat8_t(Half to Away Round)，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322HIF8_PRE，float量化成hifloat8_t(Half to Away Round)，tensor量化；<br>&nbsp;&nbsp;&bull; QF322HIF8_PRE_HYBRID，float量化成hifloat8_t(Hybrid Round)，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322HIF8_PRE_HYBRID，float量化成hifloat8_t(Hybrid Round)，tensor量化；<br>&nbsp;&nbsp;&bull; QS322BF16_PRE，int32_t量化成bfloat16_t，scalar量化；<br>&nbsp;&nbsp;&bull; VQS322BF16_PRE，int32_t量化成bfloat16_t，tensor量化；<br>&nbsp;&nbsp;&bull; QF322F16_PRE，float量化成half，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322F16_PRE，float量化成half，tensor量化；<br>&nbsp;&nbsp;&bull; QF322BF16_PRE，float量化成bfloat16_t，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322BF16_PRE，float量化成bfloat16_t，tensor量化；<br>&nbsp;&nbsp;&bull; QF322F32_PRE，float量化成float，scalar量化，该量化模式精度无法达到双万分之一，可以达到双千分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。<br>&nbsp;&nbsp;&bull; VQF322F32_PRE，float量化成float，tensor量化，该量化模式精度无法达到双万分之一，可以达到双千分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。 |
| deqScalar | 输入 | scalar量化参数，表示单个scale值，quantPre量化模式为scalar量化时需要设置该参数。支持的数据类型为uint64_t。 |
| reluEn | 输入 | 是否开启ReLU的开关：<br>&nbsp;&nbsp;&bull; false：不开启ReLU功能；<br>&nbsp;&nbsp;&bull; true：开启ReLU功能。 |
| unitFlag | 输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 1（2'b01）：保留值；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不会设置寄存器；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，会将unitFlag关闭。 |
| params | 输入 | params是类型为TransformParams的成员变量，TransformParams结构体是一个基于模板参数的类型选择器，用于在编译时根据不同的CO2Layout布局类型，自动选择对应的参数类型。<br><pre>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};</pre><br>CO2Layout布局类型为ROW_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2ND转换，结构体参数如下：<br><pre>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};</pre>&bull; ndNum：源NZ矩阵的数目，也就是传输ND矩阵的数目，取值范围为ndNum∈[0, 65535]。**注：ndNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&nbsp;&nbsp;&bull; srcNdStride：不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[0, 65535]，单位为C0_SIZE。当ndNum配置为1时，srcNdStride配置为0即可，不生效。<br>&nbsp;&nbsp;&bull; dstNdStride：目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[1, 2^32 -1]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。<br><br>CO2Layout布局类型为COLUMN_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2DN转换，结构体参数如下：<br><pre>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};</pre>&bull; dnNum：传输DN矩阵的数目，取值范围为dnNum∈[0, 65535]。**注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&nbsp;&nbsp;&bull; srcNzMatrixStride：不同源NZ矩阵的偏移（头与头），单位为C0_SIZE。<br>&nbsp;&nbsp;&bull; dstDnMatrixStride：目的相邻DN矩阵起始地址间的偏移，取值范围为dstDnMatrixStride∈[1, 2^32 -1]，单位为element。<br>&nbsp;&nbsp;&bull; srcNzC0Stride：源矩阵NZ分形中相邻行的地址偏移（头与头），单位为C0_SIZE。<br><br>CO2Layout布局类型为NZ时，为普通搬运DMA模式，表示从L0C Buffer到目标位置的正常数据移动。 |
| dualDstCtrl | 输入 | 双目标模式控制。当启用双目标模式控制时，L0C Buffer中的M×N矩阵将被分成两半，并同时写入两个子块（SUB BLOCK）的UB中，其中前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。<br>&nbsp;&nbsp;&bull; 2'b00：单目标模式，将整个矩阵写入通过subBlockId参数配置的目标UB。<br>&nbsp;&nbsp;&bull; 2'b01：双目标模式，按M维度拆分，M / 2 \* N写入每个UB，M必须为2的倍数。<br>&nbsp;&nbsp;&bull; 2'b10：双目标模式，按N维度拆分，M \* N / 2写入每个UB，N须为32的倍数。<br>&nbsp;&nbsp;&bull; 2'b11：保留。<br>dualDstCtrl仅支持在普通搬运模式或NZ2ND搬运场景下使用，不支持随路功能场景。 |
| subBlockId | 输入 | 在启用单目标模式时指示目标UB的编号。 |
| isChannelSplit | 输入 | 是否开启通道拆分的功能。默认为false，不开启该功能。仅在src和dst都为float时才能开启通道拆分，且不能同时开启ChannelSplit和NZ2ND功能。 |

**表 4** Fixpipe搬运参数（FixpipeParamsV220）结构体说明

| 参数名称 | 数据类型 | 含义 |
| --- | --- | --- |
| nSize | 必选输入 | 源NZ矩阵在N方向上的大小。取值范围nSize∈[0, 4095]，nSize必须为16的倍数。<br>对于目的矩阵NZ输出：输出类型为float类型时，若开启[channelSplit功能](./关键特性说明/F32-Channel-Split.md)，nSize必须为8的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 必选输入 | 源NZ矩阵在M方向上的大小。<br>&nbsp;&nbsp;&bull; 不开启随路NZ2ND功能（[NZ2NZ搬运](./关键特性说明/NZ2NZ.md)），取值范围为mSize∈[0, 65535]。<br>&nbsp;&nbsp;&bull; 开启随路[NZ2ND](./关键特性说明/NZ2ND.md)功能，取值范围为mSize∈[0, 8192]。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 必选输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]，单位为C0_Size（16*sizeof(T)），T为src的数据类型，其值应填成mSize对16向上取整。 |
| dstStride | 必选输入 | &bull; 不开启NZ2ND功能（[NZ2NZ搬运](./关键特性说明/NZ2NZ.md)）：目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为datablock（32字节）。<br>&nbsp;&nbsp;&bull; 开启随路[NZ2ND](./关键特性说明/NZ2ND.md)功能：目的ND矩阵每一行中的元素个数，取值不为0，单位为element。 |
| [quantPre](./关键特性说明/随路量化.md) | 可选输入 | 用于控制量化模式，QuantMode_t类型，默认值为`QuantMode_t::NoQuant`，具体定义如下：<br>&nbsp;&nbsp;&bull; float/int32_t输出此需配置为`QuantMode_t::NoQuant`;<br>&nbsp;&nbsp;&bull; half/bfloat16_t输出，此参数需配置为`QuantMode_t::F322F16`/`QuantMode_t::F322BF16`;<br>注：此参数需要用户手动配置，不会自动推导配置对应量化模式。<br><pre>enum QuantMode_t<br>{<br>    NoQuant,      // 不开启量化功能<br>    F322F16,      // Float32_2_Float16：float cast成half，cast mode为CAST_RINT模式<br>    F322BF16,     // Float32_2_BFloat16：float cast成bfloat16_t，cast mode为CAST_RINT模式<br>    DEQF16,       // DeQuant_Float16：int32_t量化成half，scalar量化<br>    VDEQF16,      // Vector_DeQuant_Float16：int32_t量化成half，tensor量化<br>    QF322B8_PRE,  // Quant_Float32_2_B8：float量化成int8_t/uint8_t，scalar量化<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8：float量化成int8_t/uint8_t，tensor量化<br>    REQ8,         // ReQuant_int8：int32_t量化成int8_t/uint8_t，scalar量化<br>    VREQ8,        // Vector_ReQuant_int8：int32_t量化成int8_t/uint8_t，tensor量化<br>};</pre> |
| deqScalar | 可选输入 | scalar量化参数，表示单个scale值，quantPre量化模式为[随路量化](./关键特性说明/随路量化.md)时需要设置该参数。支持的数据类型为`uint64_t`。 |
| [reluEn](./关键特性说明/随路relu.md) | 可选输入 | 是否开启NormReLU的开关：<br>&nbsp;&nbsp;&bull; `false`：不开启NormReLU功能， 默认为`false`；<br>&nbsp;&nbsp;&bull; `true`：开启NormReLU功能。 |
| unitFlag | 可选输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 1（2'b01）：保留值；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>参数设置方案和特性细节可参考：Mmad计算中关键特性说明的[UnitFlag](../Mmad计算/关键特性说明/UnitFlag.md)章节。 |
| [channelSplit](./关键特性说明/F32-Channel-Split.md) | 可选输入 | 仅在L0C Buffer(CO1) -> GM NZ float输出时生效，是否开启通道拆分的功能。默认为`false`，不开启该功能。 |
| ndNum | 可选输入 | L0C Buffer上源NZ矩阵的数目，也就是传输ND矩阵的数目，默认为1，取值范围为ndNum∈[0, 65535]，[NZ2ND](./关键特性说明/NZ2ND.md)多个NZ非连续矩阵的个数。<br>**注：ndNum=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcNdStride | 可选输入 | 不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[0, 512]，单位为数据块16*C0_Size。当ndNum配置为1时，srcNdStride配置为0即可，不生效。 |
| dstNdStride | 可选输入 | 目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[0, 65535]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。 |

**表 5** Fixpipe搬运参数（FixpipeParamsM300）结构体说明

| 参数名称 | 数据类型 | 含义 |
| ---------- | ---------- | ---------- |
| nSize | 输入 | 源NZ矩阵在N方向上的大小，取值范围为nSize ∈[0, 4095]。<br>&nbsp;&nbsp;&bull; 若开启channelSplit功能，nSize必须为8的倍数。<br>&nbsp;&nbsp;&bull; 若不开启channelSplit功能，nSize必须为16的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 输入 | 源NZ矩阵在M方向上的大小。<br>&nbsp;&nbsp;&bull; 不开启NZ2ND功能，取值范围为mSize∈[0, 65535]。<br>&nbsp;&nbsp;&bull; 开启NZ2ND功能，取值范围为mSize∈[0, 8192]。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]， 单位为C0_Size(16\*sizeof(T)，T为src的数据类型)。 |
| dstStride | 输入 | &bull; 不开启NZ2ND功能，目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0， 单位为datablock(32字节)。<br>&bull; 开启NZ2ND功能，目的ND矩阵每一行中的元素个数，取值不为0 ，单位为element。 |
| quantPre | 输入 | QuantMode_t是一个枚举类型，用于控制量化模式，默认值为QuantMode_t::NoQuant，即不开启量化功能。QuantMode_t取值如下：<br>&nbsp;&nbsp;&bull; NoQuant，不开启量化功能；<br>&nbsp;&nbsp;&bull; F322F16，float cast成half，cast mode为CAST_RINT模式；<br>&nbsp;&nbsp;&bull; F322BF16，float cast成bfloat16_t，cast mode为CAST_RINT模式；<br>&nbsp;&nbsp;&bull; DEQF16，int32_t量化成half，scalar量化；<br>&nbsp;&nbsp;&bull; VDEQF16，int32_t量化成half，tensor量化；<br>&nbsp;&nbsp;&bull; QF322B8_PRE，float量化成uint8_t/int8_t，scalar量化；<br>&nbsp;&nbsp;&bull; VQF322B8_PRE，float量化成uint8_t/int8_t，tensor量化；<br>&nbsp;&nbsp;&bull; REQ8，int32_t量化成uint8_t/int8_t，scalar量化；<br>&nbsp;&nbsp;&bull; VREQ8，int32_t量化成uint8_t/int8_t，tensor量化。 |
| deqScalar | 输入 | scalar量化参数，表示单个scale值，quantPre量化模式为scalar量化时需要设置该参数。支持的数据类型为uint64_t。 |
| ndNum | 输入 | 源NZ矩阵的数目，也就是传输ND矩阵的数目，取值范围为ndNum∈[1, 65535]。 |
| srcNdStride | 输入 | 不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[1, 512]，单位为数据块（16 \* C0_Size）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。 |
| dstNdStride | 输入 | 目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[1, 65535]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。 |
| reluEn | 输入 | 是否开启ReLU的开关：<br>&nbsp;&nbsp;&bull; false：不开启ReLU功能；<br>&nbsp;&nbsp;&bull; true：开启ReLU功能。 |
| unitFlag | 输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出，该功能不适用于在L0C Buffer累加的场景。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 1（2'b01）：保留值；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不会设置寄存器；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，会将unitFlag关闭。 |
| isChannelSplit | 输入 | 是否开启通道拆分的功能。默认为false，不开启该功能。仅在src和dst都为float时才能开启通道拆分，且不能同时开启ChannelSplit和NZ2ND功能。 |

## 数据类型<a id="zh-cn_topic_0000002542828493_section4219135304818"></a>

**源矩阵与目的矩阵支持的数据类型组合**

<cann-filter npu-type = "950">

针对Ascend 950PR/Ascend 950DT，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | int8_t、uint8_t、hifloat8_t、fp8_e4m3fn_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、bfloat16_t、int32_t。 |

</cann-filter>

<cann-filter npu-type = "A3">

针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、int32_t。 |

</cann-filter>

<cann-filter npu-type = "910b">

针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、int32_t。 |

</cann-filter>

<cann-filter npu-type = "310b">

针对Atlas 200I/500 A2 推理产品，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、int32_t。 |

</cann-filter>

<cann-filter npu-type = "x90">

针对Kirin X90，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | int8_t、half。 |
| int32_t | int8_t、half、int32_t。 |

</cann-filter>

<cann-filter npu-type = "9030">

针对Kirin 9030，Fixpipe接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（GM） |
| ---------- | ---------- |
| float | half |
| int32_t | half |

</cann-filter>

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002542828493_section2045914466492"></a>

- 对于量化输入为float32数据类型的说明如下：
    - 标准的IEEE 754 float32格式为：1bit符号位，8bits指数位，23bits尾数位；当前AI处理器支持的float32格式为：1bit符号位，8bits指数位，10bits尾数位。
    - 如果用户提供的是标准的IEEE 754 float32输入，API内部会处理成处理器支持的float32格式进行计算，此时如果golden数据生成过程中使用的是标准的IEEE 754 float32数据，则可能引入精度不匹配问题，需要修正golden数据的生成，将量化参数的23bits尾数位的低13bits数据位清零再参与量化计算。
- 源矩阵NZ格式地址要求6对齐，目的矩阵ND格式地址要求满足1字节对齐，NZ格式地址需要满足32字节对齐。
- 当搬出的mSize、nSize或者ndNum中的任意一个值为0时，该指令不会被执行。ndNum=0时会报warning。
- cbufWorkspace量化Tensor位于L1 Buffer，地址要求32字节对齐。
- 量化和ReLU参数不能为INF/NAN和非规格化数。
- 目标数据不能有重叠。如果对目的地址有重叠写入，硬件不会报告任何警告和错误，也不保证重叠数据的写入顺序。
- unitFlag特性开启需要配合Mmad同时开启。

<cann-filter npu-type = "A3,910b">

- 针对如下产品型号，特殊值/边界值约束说明如下：

    <cann-filter npu-type = "A3">

    Atlas A3 训练系列产品/Atlas A3 推理系列产品；

    </cann-filter>
    
    <cann-filter npu-type = "910b">

    Atlas A2 训练系列产品/Atlas A2 推理系列产品；

    </cann-filter>

    对于浮点类型INF/NAN输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式：

    - 非饱和模式：CTRL\[48\]设置成1'b1，INF/NAN保持原输出。
    - 饱和模式：CTRL\[48\]设置成1'b0，INF输出会被饱和为±MAX，NaN输出会被饱和为0。

        ```cpp
        // 设置CTRL[48]为0，开启浮点数饱和模式
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    对于整数类型只有饱和模式。

</cann-filter>

## 调用示例<a id="zh-cn_topic_0000002542828493_section088124295117"></a>

<cann-filter npu-type = "950,A3,910b">

Fixpipe完整样例请参考[fixpipe\_l0c2gm样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/01_matrix_compute/fixpipe_l0c2gm)；

- 示例：通路L0C Buffer-\>GM。输入A矩阵和B矩阵的数据类型为half，输出C矩阵为float，配置开启NZ2ND的格式转换。

    ```cpp
        AscendC::GlobalTensor<outputType> cGM;
        cGM.SetGlobalBuffer((__gm__ outputType *)c);
    
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        AscendC::FixpipeParamsV220 fixpipeParams;
        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
    #endif
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        fixpipeParams.dstStride = baseN;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, c, fixpipeParams);
    ```

</cann-filter>
