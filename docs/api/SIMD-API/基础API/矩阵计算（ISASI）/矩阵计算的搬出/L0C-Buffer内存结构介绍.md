# L0C Buffer内存结构介绍<a id="ZH-CN_TOPIC_0000002538071268"></a>

为提升数据访问效率和吞吐量，L0C Buffer采用了等容量的内存模块（Bank）结构设计。内存空间被划分为多个Bank，并以Bank为单位进一步分组形成多个Bank Group（BG）；每个Bank Group包含若干Bank，每个Bank都是一个可独立操作的存储单元子模块。

<cann-filter npu-type = "A3,910b">

以如下产品型号为例：

<cann-filter npu-type = "A3">

Atlas A3 训练系列产品/Atlas A3 推理系列产品；

</cann-filter>

<cann-filter npu-type = "910b">

Atlas A2 训练系列产品/Atlas A2 推理系列产品；

</cann-filter>

L0C Buffer的总容量为128K字节，由16个Bank组成。每个Bank的容量为8K字节，由128行组成，每行的宽度为64字节。这16个Bank被进一步组织为16个Bank Group。每个Bank Group包含1个Bank。

L0C Buffer的内存结构如[图1](#zh-cn_topic_0000002555148561_fig1715315610160)所示，L0C Buffer的地址编码格式：

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // bit位顺序为从高位到低位，共17位
// BANK_DEPTH表示地址在Bank中的行数，占用7位bit位，取值范围为[0, 127]
// BANK表示地址所在Bank的编号，占用4位bit位，取值范围为[0, 15]
// BANK_WIDTH表示地址在Bank一行上的偏移量，占用6位bit位，取值范围为[0, 63]
```

**图1** L0C Buffer内存结构图<a id="zh-cn_topic_0000002555148561_fig1715315610160"></a>  

![](../../../../figures/L0C_Memory_Structure.png)

</cann-filter>
