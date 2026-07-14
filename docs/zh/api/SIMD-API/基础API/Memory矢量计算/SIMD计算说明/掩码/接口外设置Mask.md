# 接口外设置Mask

## Counter模式

- **前n个数据计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, 1);
    AscendC::SetMaskNorm();
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图1**  接口外设置Mask，Counter模式前n个数据计算示例  
    ![](../../../../../figures/external_counter_first_n.png)<a id="图1-接口外设置Mask-counter模式前n个数据计算示例"></a>

- **高维切分：连续计算**

    在[Counter计算模式](概述.md#mask-mode)中，Mask寄存器中的值只有低64bit有效，被当做参与计算的元素数量。同时高维切分矢量计算接口的repeatTime参数会被忽略，由系统内部根据元素数量自动计算。

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(130);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask(); 
    ```

    **图2**  接口外设置Mask，Counter模式连续计算示例  
    ![](../../../../../figures/external_counter_continuous.png)<a id="图2-接口外设置Mask-counter模式连续计算示例"></a>

## Normal模式

在[Normal计算模式](概述.md#mask-mode)中，Mask寄存器会作为每个Repeat的掩码使用。只有高维切分支持此模式，在接口上区分为逐bit模式和连续模式。

**注：Normal为默认模式。若未发生模式切换，则无需重复配置该模式。**

- **高维切分：逐bit计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0x69A569A569A569A5, 0x69A569A569A569A5);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图3**  接口外设置Mask，Normal模式高维切分逐bit计算示例  
    ![](../../../../../figures/external_normal_bitwise.png)<a id="图3-接口外设置Mask-normal模式高维切分逐bit计算示例"></a>

- **高维切分：连续计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图4**  接口外设置Mask，Normal模式高维切分连续计算示例  
    ![](../../../../../figures/external_normal_continuous.png)<a id="图4-接口外设置Mask-normal模式高维切分连续计算示例"></a>

> [!NOTE]说明
> 
> 当数据类型为half（操作数为16位）时，每次迭代内能够处理的元素个数mask∈\[0, 128\]。当mask\>128时，仍按一次repeat（128元素）执行，不支持超过128元素的mask控制。
