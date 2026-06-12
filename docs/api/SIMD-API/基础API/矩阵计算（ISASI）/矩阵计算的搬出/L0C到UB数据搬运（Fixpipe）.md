# L0C Buffer到UB数据搬运（Fixpipe）<a name="ZH-CN_TOPIC_0000002563847576"></a>

## 产品支持情况<a name="zh-cn_topic_0000002549846732_section796754519912"></a>

|产品|是否支持|
|----------|:----------:|
|Ascend 950PR/Ascend 950DT|支持包含FixpipeParamsArch3510参数的接口。|
|Atlas A3 训练系列产品/Atlas A3 推理系列产品|x|
|Atlas A2 训练系列产品/Atlas A2 推理系列产品|x|
|Atlas 200I/500 A2 推理产品|支持包含FixpipeParamsM300参数的接口。|
|Atlas 推理系列产品AI Core|x|
|Atlas 推理系列产品Vector Core|x|
|Atlas 训练系列产品|x|
|Kirin X90|x|
|Kirin 9030|x|

## 功能说明<a name="zh-cn_topic_0000002549846732_section106841136114319"></a>

头文件引用路径为："basic\_api/kernel\_operator\_fixpipe\_intf.h"。

矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至Unified Buffer（UB）中，并且在搬运过程中支持随路格式转换等操作。

## 函数原型<a name="zh-cn_topic_0000002549846732_section82039854412"></a>

L0C Buffer到UB数据搬运提供矩阵搬出的组合接口Fixpipe，接口内包含了设置寄存器与数据搬运能力，对应的接口如下：

<cann-filter npu_type = "950">

针对Ascend 950PR/Ascend 950DT：

- 通路L0C Buffer-\>UB，不开启tensor量化功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- 通路L0C Buffer-\>UB，开启tensor量化功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

</cann-filter>

<cann-filter npu_type = "310b">

针对Atlas 200I/500 A2 推理产品：

- 通路L0C Buffer-\>UB，不开启tensor量化功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300& intriParams)
    ```

- 通路L0C Buffer-\>UB，开启tensor量化功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsM300& intriParams)
    ```

</cann-filter>

## 参数说明<a name="zh-cn_topic_0000002549846732_section16128134420472"></a>

**表 1**  Fixpipe模板参数说明

| 参数名 | 描述 |
|----------|----------|
| T/U | 目的操作数/源操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002549846732_section4219135304818)。 |
| config | Fixpipe相关配置参数，类型为FixpipeConfig。取值如下：<br>&bull; **CFG_ROW_MAJOR（默认取值）**：开启NZ2ND，输出数据格式为ND格式。<br>&bull; CFG_NZ：NZ2NZ，输出数据格式为NZ格式。<br><cann-filter npu_type = "950">&bull; CFG_COLUMN_MAJOR：针对Ascend 950PR/Ascend 950DT，开启NZ2DN，输出数据格式为DN格式。<br></cann-filter><pre>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // 用于用户指定目的地址的位置是否是UB <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // 输出数据格式仍为NZ格式。<br>    ROW_MAJOR, // 开启NZ2ND，输出数据格式为ND格式。<br><cann-filter npu_type = "950">    COLUMN_MAJOR, // 仅Ascend 950PR/Ascend 950DT支持，开启NZ2DN，输出数据格式为DN格式。<br></cann-filter>};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<cann-filter npu_type = "950"><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // 仅Ascend 950PR/Ascend 950DT支持</cann-filter></pre> |
| S | 参数cbufWorkspace的数据类型，即随路量化参数的数据类型。<br>&bull; 当目的操作数、源操作数、cbufWorkspace使用基础数据类型时，模板参数S必须为uint64_t类型，否则编译失败。<br>&bull; 当目的操作数、源操作数、cbufWorkspace使用TensorTrait类型时，模板参数S的LiteType必须为uint64_t类型，否则编译失败。<br>模板参数S后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表 2**  Fixpipe参数说明

| 参数名称 | 输入/输出 | 含义 |
|----------|----------|----------|
| dst | 输出 | 目的操作数，类型为GlobalTensor。数据格式为NZ、ND格式。ND地址需求满足1字节对齐，NZ地址需满足32字节对齐。<cann-filter npu_type = "950"><br>针对Ascend 950PR/Ascend 950DT，还支持数据格式为DN，地址需满足32字节对齐。</cann-filter> |
| src | 输入 | 源操作数，类型为LocalTensor，支持的物理地址为L0C Buffer（TPosition为CO1），为Mmad接口计算的结果。数据格式为NZ格式，地址需要满足6对齐。 |
| intriParams | 输入 | Fixpipe搬运参数，具体定义请参考 "basic_api/interface/kernel_struct_fixpipe.h"。<br>参数说明请参考Fixpipe搬运参数（FixpipeParamsArch3510、FixpipeParamsM300）结构体说明。 |
| cbufWorkspace | 输入 | 量化参数，类型为`LocalTensor<uint64_t>`，支持的物理地址为L1 Buffer（TPosition为C1），地址需满足32字节对齐。<br>&bull; 当quantPre为VDEQF16、VQF322B8_PRE、VREQ8时支持。<br><cann-filter npu_type = "950">&bull; 针对Ascend 950PR/Ascend 950DT，除上述外还有VQF322FP8_PRE、VQF322HIF8_PRE、VQF322HIF8_PRE_HYBRID、VQS322BF16_PRE、VQF322F16_PRE、VQF322BF16_PRE、VQF322F32_PRE支持。<br></cann-filter>quantPre介绍请参考Fixpipe搬运参数结构体中quantPre部分。 |

<cann-filter npu_type = "950">

**表 3**  Fixpipe搬运参数（FixpipeParamsArch3510）结构体说明

| 参数名称 | 数据类型 | 含义 |
|----------|----------|----------|
| nSize | 输入 | 源NZ矩阵在N方向上的大小，取值范围为nSize ∈[0, 4095]。<br>&bull; 若开启channelSplit功能，nSize必须为8的倍数。<br>&bull; 若不开启channelSplit功能，nSize必须为16的倍数。<br>&bull; 若开启NZ2ND功能，nSize*sizeof(T)必须为32的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 输入 | 源NZ矩阵在M方向上的大小，取值范围为mSize∈[0, 65535]。<br>若开启NZ2DN功能，mSize*sizeof(T)必须为32的倍数。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]， 单位为C0_SIZE（16\*sizeof(T)，T为src的数据类型）。 |
| dstStride | 输入 | &bull; 不开启NZ2ND功能：目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为element。<br>&bull; 开启NZ2ND/NZ2DN功能：目的ND矩阵每一行中的元素个数，取值不为0，单位为element。 |
| quantPre | 输入 | QuantMode_t是一个枚举类型，用于控制量化模式，默认值为QuantMode_t::NoQuant，即不开启量化功能。QuantMode_t取值如下：<br>&bull; NoQuant，不开启量化功能<br>&bull; F322F16，float cast成half，cast mode为CAST_RINT模式<br>&bull; F322BF16，float cast成bfloat16_t，cast mode为CAST_RINT模式<br>&bull; DEQF16，int32_t量化成half， scalar量化<br>&bull; VDEQF16，int32_t量化成half，tensor量化<br>&bull; QF322B8_PRE，float量化成uint8_t/int8_t，scalar量化<br>&bull; VQF322B8_PRE，float量化成uint8_t/int8_t，tensor量化<br>&bull; REQ8，int32_t量化成uint8_t/int8_t，scalar量化<br>&bull; VREQ8，int32_t量化成uint8_t/int8_t，tensor量化<br>&bull; QF322FP8_PRE，float量化成fp8_e4m3fn_t，scalar量化<br>&bull; VQF322FP8_PRE，float量化成fp8_e4m3fn_t，tensor量化<br>&bull; QF322HIF8_PRE，float量化成hifloat8_t(Half to Away Round)，scalar量化<br>&bull; VQF322HIF8_PRE，float量化成hifloat8_t(Half to Away Round)，tensor量化<br>&bull; QF322HIF8_PRE_HYBRID，float量化成hifloat8_t(Hybrid Round)，scalar量化<br>&bull; VQF322HIF8_PRE_HYBRID，float量化成hifloat8_t(Hybrid Round)，tensor量化<br>&bull; QS322BF16_PRE，int32_t量化成bfloat16_t，scalar量化<br>&bull; VQS322BF16_PRE，int32_t量化成bfloat16_t，tensor量化<br>&bull; QF322F16_PRE，float量化成half，scalar量化<br>&bull; VQF322F16_PRE，float量化成half，tensor量化<br>&bull; QF322BF16_PRE，float量化成bfloat16_t，scalar量化<br>&bull; VQF322BF16_PRE，float量化成bfloat16_t，tensor量化<br>&bull; QF322F32_PRE，float量化成float，scalar量化，该量化模式精度无法达到双万分之一，可以达到双千分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。<br>&bull; VQF322F32_PRE，float量化成float，tensor量化，该量化模式精度无法达到双万分之一，可以达到双千分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。 |
| deqScalar | 输入 | scalar量化参数，表示单个scale值，quantPre量化模式为scalar量化时需要设置该参数。支持的数据类型为uint64_t。 |
| reluEn | 输入 | 是否开启ReLU的开关：<br>&bull; false：不开启ReLU功能；<br>&bull; true：开启ReLU功能。 |
| unitFlag | 输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&bull; 0（2'b00）：不开启unitFlag；<br>&bull; 1（2'b01）：保留值；<br>&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不会设置寄存器；<br>&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，会将unitFlag关闭。 |
| params | 输入 | params是类型为TransformParams的成员变量，TransformParams结构体是一个基于模板参数的类型选择器，用于在编译时根据不同的CO2Layout布局类型，自动选择对应的参数类型。<br><pre>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};</pre><br>CO2Layout布局类型为ROW_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2ND转换，结构体参数如下：<br><pre>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};</pre>&bull; ndNum：源NZ矩阵的数目，也就是传输ND矩阵的数目，取值范围为ndNum∈[0, 65535]。**注：ndNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&bull; srcNdStride：不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[0, 65535]，单位为C0_SIZE。当ndNum配置为1时，srcNdStride配置为0即可，不生效。<br>&bull; dstNdStride：目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[1, 2^32 -1]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。<br><br>CO2Layout布局类型为COLUMN_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2DN转换，结构体参数如下：<br><pre>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};</pre>&bull; dnNum：传输DN矩阵的数目，取值范围为dnNum∈[0, 65535]。**注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&bull; srcNzMatrixStride：不同源NZ矩阵的偏移（头与头），单位为C0_SIZE。<br>&bull; dstDnMatrixStride：目的相邻DN矩阵起始地址间的偏移，取值范围为dstDnMatrixStride∈[1, 2^32 -1]，单位为element。<br>&bull; srcNzC0Stride：源矩阵NZ分形中相邻行的地址偏移（头与头），单位为C0_SIZE。<br><br>CO2Layout布局类型为NZ时，为普通搬运DMA模式，表示从L0C Buffer到目标位置的正常数据移动。 |
| dualDstCtrl | 输入 | 双目标模式控制。当启用双目标模式控制时，L0C Buffer中的M×N矩阵将被分成两半，并同时写入两个子块（SUB BLOCK）的UB中，其中前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。<br>&bull; 2'b00：单目标模式，将整个矩阵写入通过subBlockId参数配置的目标UB。<br>&bull; 2'b01：双目标模式，按M维度拆分，M / 2 \* N写入每个UB，M必须为2的倍数。<br>&bull; 2'b10：双目标模式，按N维度拆分，M \* N / 2写入每个UB，N须为32的倍数。<br>&bull; 2'b11：保留。<br>dualDstCtrl仅支持在普通搬运模式或NZ2ND搬运场景下使用，不支持随路功能场景。 |
| subBlockId | 输入 | 在启用单目标模式时指示目标UB的编号。 |
| isChannelSplit | 输入 | 是否开启通道拆分的功能。默认为false，不开启该功能。仅在src和dst都为float时才能开启通道拆分，且不能同时开启ChannelSplit和NZ2ND功能。 |

</cann-filter>

<cann-filter npu_type = "310b">

**表 4**  Fixpipe搬运参数（FixpipeParamsM300）结构体说明

| 参数名称 | 数据类型 | 含义 |
|----------|----------|----------|
| nSize | 输入 | 源NZ矩阵在N方向上的大小，取值范围为nSize ∈[1, 4095]。<br>&bull; 若开启channelSplit功能，nSize必须为8的倍数。<br>&bull; 若不开启channelSplit功能，nSize必须为16的倍数。 |
| mSize | 输入 | 源NZ矩阵在M方向上的大小。<br>&bull; 不开启NZ2ND功能，取值范围为mSize∈[1, 65535]。<br>&bull; 开启NZ2ND功能，取值范围为mSize∈[1, 8192]。 |
| srcStride | 输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]， 单位为C0_SIZE(16\*sizeof(T)，T为src的数据类型)。 |
| dstStride | 输入 | &bull; 不开启NZ2ND功能，目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0， 单位为datablock(32字节)。<br>&bull; 开启NZ2ND功能，目的ND矩阵每一行中的元素个数，取值不为0 ，单位为element。 |
| quantPre | 输入 | QuantMode_t是一个枚举类型，用于控制量化模式，默认值为QuantMode_t::NoQuant，即不开启量化功能。QuantMode_t取值如下：<br>&bull; NoQuant，不开启量化功能<br>&bull; F322F16，float cast成half，cast mode为CAST_RINT模式<br>&bull; F322BF16，float cast成bfloat16_t，cast mode为CAST_RINT模式<br>&bull; DEQF16，int32_t量化成half，scalar量化<br>&bull; VDEQF16， int32_t量化成half，tensor量化<br>&bull; QF322B8_PRE，float量化成uint8_t/int8_t，scalar量化<br>&bull; VQF322B8_PRE，float量化成uint8_t/int8_t，tensor量化<br>&bull; REQ8，int32_t量化成uint8_t/int8_t，scalar量化<br>&bull; VREQ8，int32_t量化成uint8_t/int8_t，tensor量化 |
| deqScalar | 输入 | scalar量化参数，表示单个scale值，quantPre量化模式为scalar量化时需要设置该参数。支持的数据类型为uint64_t。 |
| ndNum | 输入 | 源NZ矩阵的数目，也就是传输ND矩阵的数目，取值范围为ndNum∈[1, 65535]。 |
| srcNdStride | 输入 | 不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[1, 512]，单位为数据块（16 \* C0_SIZE）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。 |
| dstNdStride | 输入 | 目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[1, 65535]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。 |
| reluEn | 输入 | 是否开启ReLU的开关：<br>&bull; false：不开启ReLU功能；<br>&bull; true：开启ReLU功能。 |
| unitFlag | 输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出，该功能不适用于在L0C Buffer累加的场景。取值说明如下：<br>&bull; 0（2'b00）：不开启unitFlag；<br>&bull; 1（2'b01）：保留值；<br>&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不会设置寄存器；<br>&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，会将unitFlag关闭。 |
| isChannelSplit | 输入 | 该通路下不生效。 |

</cann-filter>

## 数据类型<a id="zh-cn_topic_0000002549846732_section4219135304818"></a>

**源矩阵与目的矩阵支持的数据类型组合**

<cann-filter npu_type = "950">

针对Ascend 950PR/Ascend 950DT，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（UB） |
| ---------- | ---------- |
| float | int8_t、uint8_t、hifloat8_t、fp8_e4m3fn_t、half、bfloat16_t、float |
| int32_t | int8_t、uint8_t、half、bfloat16_t、int32_t |

</cann-filter>

<cann-filter npu_type = "310b">

针对Atlas 200I/500 A2 推理产品，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（UB） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t、float |
| int32_t | int8_t、uint8_t、half、int32_t |

</cann-filter>

## 返回值说明

无

## 约束说明<a name="zh-cn_topic_0000002549846732_section2045914466492"></a>

- ndNum=0表示不执行，此指令将不被执行并报warning。
- 对于量化输入为float32数据类型的说明如下：
    - 标准的IEEE 754 float32格式为：1bit符号位，8bits指数位，23bits尾数位；当前AI处理器支持的float32格式为：1bit符号位，8bits指数位，10bits尾数位。
    - 如果用户提供的是标准的IEEE 754 float32输入，API内部会处理成处理器支持的float32格式进行计算，此时如果golden数据生成过程中使用的是标准的IEEE 754 float32数据，则可能引入精度不匹配问题，需要修正golden数据的生成，将量化参数的23bits尾数位的低13bits数据位清零再参与量化计算。

<cann-filter npu_type = "950">

## 关键特性说明

以下图片和说明仅适用于Ascend 950PR/Ascend 950DT：

### 不开启NZ2ND随路格式转换

不开启NZ2ND的情况下，参数设置示例（通过Fixpipe接口搬运并去除dummy数据）和解释说明如下：

当M方向上的数据元素个数不是16的倍数时，搬入时会额外读取dummy数据，并在写入目标位置后丢弃这些dummy数据。矩阵块被定义为连续的16\*16的数据块，数据块的个数为M/16向上取整， 矩阵块的长度为M\*16\*sizeof\(T\)，T是数据类型。

**单搬运模式：**

- nSize = 48，表示源NZ矩阵中待搬运矩阵（图中蓝色区域）在N方向上的大小为48个元素。
- mSize = 24，表示源NZ矩阵中待搬运矩阵在M方向上的大小为24个元素。
- srcStride = 64，表示源NZ矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个蓝色Z排布的起始地址与第二个蓝色Z排布的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 40 \* C0，表示目的NZ矩阵中相邻Z排布的起始地址偏移，即下图中第一个蓝色Z排布的起始地址与第二个蓝色Z排布的起始地址之间的间隔为40 \* 16个元素。

**图 1**  不开启NZ2ND参数的单搬运模式设置示意图<a name="fig128961542184620"></a>  

![](../../../../figures/不开启NZ2ND参数的单搬运模式设置示意图.png "不开启NZ2ND参数的单搬运模式设置示意图")

**双目标控制模式：**

在普通搬运模式下启用双目标模式如下图所示，分为按M维度拆分和按N维度拆分，按M维度拆分M必须为2的倍数，按N维度拆分N必须为2的倍数：

N方向切分：

- nSize = 32，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵中待搬运矩阵在M方向上的大小为48个元素。
- srcStride = 64，表示源NZ矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个块Z排布矩阵的起始地址与第二个Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 64 \* C0，表示目的NZ矩阵中相邻Z排布的起始地址偏移，即下图中UB0中Z排布的起始地址与UB1中Z排布的起始地址之间的间隔为64 \* 16个元素。

M方向切分：

- nSize = 32，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- mSize = 24，表示源NZ矩阵中待搬运矩阵在M方向上的大小为24个元素。
- srcStride = 64，表示源NZ矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个块Z排布矩阵的起始地址与第二个Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 40 \* C0，表示目的NZ矩阵中相邻Z排布的起始地址偏移，即下图中UB0（或UB1）中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为40 \*  16个元素。

**图 2**  不开启NZ2ND参数和NZ2DN参数的双目标搬运模式设置示意图<a name="fig6561154491913"></a>  

![](../../../../figures/不开启NZ2ND参数和NZ2DN参数的双目标搬运模式设置示意图.png "不开启NZ2ND参数和NZ2DN参数的双目标搬运模式设置示意图")

### 开启NZ2ND随路格式转换

开启NZ2ND的情况下，参数设置示例和解释说明如下：

- ndNum = 2，表示源NZ矩阵的数目为2。图中蓝色区域为NZ矩阵1，紫色区域为NZ矩阵2。
- nSize = 32，表示源NZ矩阵（图中蓝色区域）在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 64，表示源NZ矩阵中相邻Z排布的起始地址偏移，即下图中第一个蓝色Z排布的起始地址与第二个蓝色Z排布的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 64，表示目的ND矩阵每一行中的元素个数为64。
- srcNdStride = 16，表示不同NZ矩阵起始地址之间的间隔为16 \*  16 \* C0\_Size。
- dstNdStride  = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

**图 3**  开启NZ2ND参数设置示意图<a name="fig79783143556"></a>  

![](../../../../figures/开启NZ2ND参数设置示意图.png "开启NZ2ND参数设置示意图")

**单搬运模式：**

- ndNum = 2，表示源NZ矩阵的数目为2。图中蓝色区域为NZ矩阵1，紫色区域为NZ矩阵2。
- nSize = 32，表示源NZ矩阵（图中蓝色区域）在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 64，表示源NZ矩阵中相邻Z排布的起始地址偏移，即下图中第一个蓝色Z排布的起始地址与第二个蓝色Z排布的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 64，表示目的ND矩阵每一行中的元素个数为64。
- srcNdStride = 256，表示不同NZ矩阵起始地址之间的间隔为256，单位为C0\_Size。
- dstNdStride  = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

**图 4**  开启NZ2ND参数的单搬入模式设置示意图<a name="fig11991024112516"></a>  

![](../../../../figures/开启NZ2ND参数的单搬入模式设置示意图.png "开启NZ2ND参数的单搬入模式设置示意图")

**双目标控制模式：**

- ndNum = 2，表示源NZ矩阵的数目为2。图中红框区域为矩阵1，蓝框区域为矩阵2。
- nSize = 32，表示源NZ矩阵（图中红框区域或蓝框区域）在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 64，表示源NZ矩阵中相邻Z排布的起始地址偏移，即下图红框区域中左侧浅色Z排布矩阵的起始地址与右侧深色Z排布矩阵的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 64，表示目的ND矩阵每一行中的元素个数为64。
- ndNum = 2，表示源NZ矩阵的数目。
- srcNdStride = 240，表示不同NZ矩阵起始地址之间的间隔为240 \* C0\_Size。
- dstNdStride  = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

    **图 5**  开启NZ2ND参数双搬入模式设置示意图<a name="fig8810182815117"></a>  

    ![](../../../../figures/开启NZ2ND参数双搬入模式设置示意图.png "开启NZ2ND参数双搬入模式设置示意图")

### 开启NZ2DN随路格式转换

开启NZ2DN的情况下，参数设置示例和解释说明如下：

**单搬运模式：**

- dnNum = 2，表示源NZ矩阵的数目为2。图中蓝色区域为NZ矩阵1，紫色区域为NZ矩阵2。
- nSize = 32，表示源NZ矩阵（图中蓝色区域）在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 80，表示源NZ矩阵中相邻Z排布的起始地址偏移，即下图中相邻两块蓝色Z排布的起始地址之间的间隔为80 \* C0\_Size。
- dstStride = 80，表示目的DN矩阵每一行中的元素个数为80。
- ndNum = 2，表示源NZ矩阵的数目。
- srcNzMatrixStride = 240，表示不同源NZ矩阵的偏移，即下图中第一个蓝色Z排布的起始地址和第二个紫色Z排布的起始地址之间的间隔为240 \* C0\_Size。
- srcNzC0Stride = 1：表示源矩阵NZ分形相邻行的地址偏移。
- dstDnMatrixStride：表示相邻DN矩阵起始地址间的偏移为48 \* 80 =3840个元素。

**图 6**  开启NZ2DN单搬运模式示意图1<a name="fig19772110476"></a>  

![](../../../../figures/开启NZ2DN单搬运模式示意图1.png "开启NZ2DN单搬运模式示意图1")

- dnNum = 2，表示源NZ矩阵的数目为2。图中蓝色区域为NZ矩阵1，红色区域为NZ矩阵2。
- nSize = 24，表示源NZ矩阵（图中蓝色区域）在N方向上的大小为24个元素。
- mSize = 24，表示源NZ矩阵在M方向上的大小为24个元素。
- srcStride = 80，表示源NZ矩阵中相邻Z排布的起始地址偏移，即下图中相邻两块蓝色Z排布的起始地址之间的间隔为80 \* C0\_Size。
- dstStride = 60，表示目的DN矩阵每一行中的元素个数为60。
- ndNum = 2，表示源NZ矩阵的数目。
- srcNzMatrixStride = 240，表示不同源NZ矩阵的偏移，即下图中第一个蓝色Z排布的起始地址和第二个紫色Z排布的起始地址之间的间隔为240 \* C0\_Size。
- srcNzC0Stride = 2：表示源矩阵NZ分形相邻行的地址偏移。
- dstDnMatrixStride：表示相邻DN矩阵起始地址间的偏移为48 \* 60 = 2880个元素。

**图 7**  开启NZ2DN单搬运模式示意图2<a name="fig769114585716"></a>  

![](../../../../figures/开启NZ2DN单搬运模式示意图2.png "开启NZ2DN单搬运模式示意图2")

</cann-filter>

## 调用示例<a name="zh-cn_topic_0000002549846732_section088124295117"></a>

<cann-filter npu_type = "950">

完整样例请参考[fixpipe\_l0c2ub样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/02_features/01_basic_api/03_matrix_compute/fixpipe_l0c2ub)；

- 示例：通路L0C Buffer-\>UB。输入A矩阵和B矩阵的数据类型为half，输出C矩阵为float，默认配置开启Nz2Nd的格式转换。完整样例可以参考[样例链接](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/02_features/01_basic_api/03_matrix_compute/fixpipe_l0c2ub)。

    ```cpp
    AscendC::LocalTensor<outputType> cUB;
    cUB = AscendC::LocalTensor<outputType>(AscendC::TPosition::VECOUT, 0, cSingleSize);
    
    AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
    fixpipeParams.mSize = baseM;
    fixpipeParams.nSize = baseN;
    fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
    fixpipeParams.dstStride = baseN;
    AscendC::Fixpipe<outputType, l0cType, CFG_ROW_MAJOR_UB>(cUB, c, fixpipeParams);
    ```

</cann-filter>
