# SuperKernel开发<a name="ZH-CN_TOPIC_0000002355094528"></a>

SuperKernel是一种算子的二进制融合技术，与源码融合不同，它聚焦于内核函数 \(Kernel\) 的二进制的调度方案，展开深度优化，于已编译的二进制代码基础上融合创建一个超级Kernel函数（SuperKernel），以调用子函数的方式调用多个其他内核函数，也就是子Kernel。相对于单算子下发，SuperKernel技术可以减少任务调度等待时间和调度开销，同时利用Task间隙资源进一步优化算子头开销。

>[!NOTE]说明 
>- SuperKernel仅适用于静态图场景。
>- SuperKernel适用于如下型号：
>    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
>    - Ascend 950PR/Ascend 950DT，该型号下SuperKernel特性是试验特性，在后续版本中会调整或改进，不保证后续兼容性，请开发者在使用过程中关注后续版本更新。

## 自定义算子支持SuperKernel<a name="section20981216853"></a>

自定义算子支持SuperKernel与普通算子在开发流程上并无显著差异，但需注意一些**特定约束**（详见下文）。当前SuperKernel特性仅支持在Pytorch框架使用，所以完成[算子入图（GE图）开发](基本开发流程.md)开发后，还需要参考《PyTorch图模式使用指南\(TorchAir\)》中的“自定义算子入图”章节，完成Pytorch入图。同时，TorchAir提供标定SuperKernel范围的能力，用户可根据实际业务需求对融合范围内的算子进行标记和优化配置。具体内容请参考[《Ascend Extension for PyTorch》](https://www.hiascend.com/document/redirect/pytorchuserguide)中“Pytorch图模式使用(TorchAir) >  max-autotune模式功能 > 图内标定SuperKernel范围”章节。

开发时的**特定约束**说明如下：

-   自定义算子若进行全核同步，需注意子Kernel（即该算子）启动的核数与SuperKernel的核数一致。若子Kernel启动的核数少于SuperKernel的核数，全核同步会等待所有核完成，导致卡住超时。

    注：SuperKernel启动核数为子Kernel的最大启动核数。假设SuperKernel包括算子a（启动核数为4）和算子b（启动核数为2），此时SuperKernel启动核数为4。

    -   使用[SyncAll](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0204.html)时，为了解决该问题，可以通过在标定SuperKernel范围时开启feed-sync-all功能，此时系统会在SuperKernel内子Kernel的其余核中插入SyncAll指令，防止卡住超时。
    -   若使用[CrossCoreSetFlag](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0273.html)和[CrossCoreWaitFlag](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0274.html)硬同步接口实现全核同步，仅支持子Kernel启动核数与SuperKernel核数相同。

-   若自定义算子的Kernel类型设置为KERNEL\_TYPE\_MIX\_AIC\_1\_1，因为SuperKernel会根据启动核数等信息调整SuperKernel的启动比例，此时需特别注意该算子也可以适应SuperKernel的1:2启动比例，确保AIC与AIV之间的硬同步操作正确执行。比如：算子内部使用了AIC与AIV之间的硬同步接口（CrossCoreSetFlag和CrossCoreWaitFlag），不要单独指定某些AIV核调用硬同步接口，使所有AIV核均调用硬同步接口，防止因为硬同步数量不匹配而导致卡死超时；使用Matmul高阶API时，算子逻辑应保证仅有一个AIV0核调用Matmul接口，防止启动两个AIV核之后出现AIV1核消息无法接收导致卡死超时。

-   在开发自定义算子时，开发者必须确保所有对GM的标量读写操作都按需正确插入[DataCacheCleanAndInvalid](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0177.html)指令：在单算子编译场景下，毕昇编译器自动在算子末尾添加DataCacheCleanAndInvalid指令，刷新整个DCache（数据缓存）。在SuperKernel中，子Kernel被当做普通函数处理，编译器不会自动插入该指令来确保数据缓存一致性，开发者需要自行保证避免因容错机制改变而导致错误。

    出于性能考虑，SuperKernel场景下Cache刷新机制如下：

    如果开发者调用GlobalTensor的GetValue和SetValue接口对GM进行标量读写，SuperKernel编译时会自动在两个接口内部插入DataCacheCleanAndInvalid指令刷新单个Cache Line，保证一定的数据缓存一致性。不会在子Kernel调用前后插入DataCacheCleanAndInvalid。

    如果开发者通过GlobalTensor的()运算符接口来获取值(读值时会插入DataCacheCleanAndInvalid指令刷新单个Cache Line保证数据缓存一致性)，但通过该接口直接改写了GlobalTensor对应位置的值时，则不会自动插入DataCacheCleanAndInvalid指令，开发者需要自行保证数据缓存一致性。比如:
    ```c++
    AscendC::GlobalTensor<float> xGm;
    xGm.SetGlobalBuffer((__gm__ float *)(addr), length); // addr 为GM地址，length为对应GM长度
    xGm(0) = (float)(1.0); // 在获取值时，SuperKernel能够保证自动刷新cache line，但是写值时，相当于普通变量直接赋值，SuperKernel无法插入DataCacheCleanAndInvalid，需要用户保证数据缓存的一致性。
    ```

    同时，需要额外注意的是，过多调用GetValue和SetValue，在SuperKernel场景下会导致性能劣化，开发者需要尽量减少该接口调用。对于劣化过多的算子，SuperKernel提供了编译选项dcci-before-kernel-start、dcci-after-kernel-start、dcci-disable-on-kernel，可以关闭指定算子内GetValue/SetValue中自动插入的缓存刷新指令以提升模型性能，最终由模型用户决定是否在SuperKernel调用该算子前或后插入整个DCache刷新，编译选项具体内容请参考[图内标定SuperKernel范围](https://www.hiascend.com/document/detail/zh/Pytorch/710/modthirdparty/torchairuseguide/torchair_00035.html)中编译选项说明。

    特别地，对于Tiling下沉场景，通常会涉及二进制复用优化，无法在线选择上述的Cache刷新机制，SuperKernel框架统一在每个子Kernel调用前后都插入DataCacheCleanAndInvalid指令，刷新整个DCache。不会在GetValue和SetValue自动进行缓存刷新。

    Cache刷新机制示意图如下图所示：

    ![](../../../../figures/图1-AI-Core内部并行计算架构抽象示意图-35.png)

-   在子Kernel中调用[GetBlockNum](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0184.html)接口获取核数时，无论是否融合SuperKernel，获取的核数保持不变，不受SuperKernel启动核数的影响。因此，在使用该接口时，开发者无需特别关注SuperKernel的启动核数，使用方法和开发普通算子时一样。
-   针对Atlas A3 训练系列产品/Atlas A3 推理系列产品中，在不使能SuperKernel场景下，[TPipe::Destroy](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_0112.html)接口内部最后会插入AscendC::PipeBarrier<PIPE\_ALL\>\(\)指令，额外保障多个TPipe之间的流水同步；模型中绝大部分算子只会使用一个TPipe对象，在对象析构时会调用Destroy，为不阻塞[SetNextTaskStart](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_00087.html)和[WaitPreTaskEnd](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_00088.html)性能提升，SuperKernel场景下默认关闭了TPipe::Destroy中插入的AscendC::PipeBarrier<PIPE\_ALL\>\(\)指令，所以当算子需要多个TPipe对象并手动调用Destroy函数时，开发者需自行保障TPipe对象间流水的同步。

## 性能优化建议<a name="section4710047496"></a>

-   任务间同步

    此外，开发者在进行Kernel侧编程时，可以通过调用[SetNextTaskStart](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_00087.html)和[WaitPreTaskEnd](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta2/API/ascendcopapi/atlasascendc_api_07_00088.html)两个任务间接口，进一步提升性能。

    -   调用SetNextTaskStart后的指令可以和后续其他的子Kernel实现并行，提升整体性能。如[图1](#fig37581010773)所示，SuperKernel按序调用子Kernel，为保证子Kernel之间数据互不干扰，会在子Kernel间插入算子间同步进行保序，子Kernel<sub>N-1</sub>调用该接口后，之后的指令会和后续子Kernel<sub>N</sub>实现并行。

        **图 1**  通过SetNextTaskStart实现并行示意图<a name="fig37581010773"></a>  
        ![](../../../../figures/通过SetNextTaskStart实现并行示意图.png "通过SetNextTaskStart实现并行示意图")

    -   调用WaitPreTaskEnd前的指令可以和前序其他的子Kernel实现并行，提升整体性能。如[图2](#fig99271836191110)所示，SuperKernel按序调用子Kernel，为保证子Kernel之间数据互不干扰，会在子Kernel间插入算子间同步进行保序，子Kernel<sub>N+1</sub>调用该接口之前的指令会和前序子Kernel<sub>N</sub>实现并行。

        **图 2**  通过WaitPreTaskEnd实现并行示意图<a name="fig99271836191110"></a>  
        ![](../../../../figures/通过WaitPreTaskEnd实现并行示意图.png "通过WaitPreTaskEnd实现并行示意图")

-   Tiling下沉场景，可以通过--op\_relocatable\_kernel\_binary编译选项，开启二进制复用优化，提升编译性能，具体可参考[LINK](../Aclnn算子工程化开发/算子包编译/算子工程编译.md)。
