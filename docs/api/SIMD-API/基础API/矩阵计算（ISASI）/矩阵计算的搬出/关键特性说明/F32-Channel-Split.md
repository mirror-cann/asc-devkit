# F32 Channel Split<a name="ZH-CN_TOPIC_0000002569070989"></a>

**特性说明：**

矩阵计算的搬出过程中对于目标类型为f32，如果开启了Channel Split，硬件就能够将16×16的分形矩阵转换为16×8的小z分形矩阵，此时每个16×16的分形矩阵将被拆分为2个独立的16×8的小z分形矩阵。

对于DataCopy接口，可以通过配置**DataCopyCO12DstParams结构体参数**定义中channelSplit为true开启。

对于Fixpipe接口，可以通过配置[Fixpipe搬运参数结构体说明](../L0C到GM数据搬运（Fixpipe）.md)中channelSplit为true开启。

**特性约束：**

<cann-filter npu-type = "A3,910b">

针对如下产品型号：

<cann-filter npu-type = "A3">

Atlas A3 训练系列产品/Atlas A3 推理系列产品；

</cann-filter>

<cann-filter npu-type = "910b">

Atlas A2 训练系列产品/Atlas A2 推理系列产品；

</cann-filter>

当Fixpipe指令的输入和输出数据类型都为float，NZ输出到GM，并且不开启NZ2ND与unitFlag时，才能够开启Channel Split（通道拆分）功能。

</cann-filter>

**使用示例：**

完整示例请参考[fixpipe_l0c2gm样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm)场景7。

如下图所示，当开启Channel Split功能后，shape为\[64,32\]的源操作数将会被拆分为16个独立的16x8分形矩阵。

**图1** F32 Channel Split示意图1<a name="zh-cn_topic_0000002515660950_fig222295694112"></a>  

![](../../../../../figures/Fixpipe_Channel_Split.png)

当开启Channel Split功能后，nSize（源NZ矩阵在N方向上的大小）必须为8的倍数。如下图所示，shape为\[64,24\]的源操作数将会被拆分为3列16x8分形矩阵。

**图2** F32 Channel Split示意图2<a name="zh-cn_topic_0000002515660950_fig5809145844116"></a>  

![](../../../../../figures/Fixpipe_Channel_Split_with_dirty.png)
