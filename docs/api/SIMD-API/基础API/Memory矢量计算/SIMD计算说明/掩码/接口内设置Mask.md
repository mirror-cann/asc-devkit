# 接口内设置Mask

## Counter模式

- **前n个数据计算**

    采用[Counter模式](概述.md#mask-mode)进行前n个数据计算，接口内部会先调用SetMaskCount，然后根据接口参数count调用SetVectorMask，再进行计算，计算完成后调用ResetMask、SetMaskNorm恢复系统默认Normal模式。

    调用示例：

    ```cpp
    uint64_t count = 128;
    half addsValue = 1.0;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, count);
    ```

    如图1所示，采用Counter模式进行前n个数据计算，通过count参数指定参与计算的元素数量为128，接口内部会自动完成Mask的设置。

    **图1**  Counter模式进行前n个数据计算示例  
    ![](../../../../../figures/counter_first_n_calc.png)<a id="图1-counter模式进行前n个数据计算示例"></a>

- **高维切分：连续计算**

    在Counter模式连续计算中，Mask寄存器中的值只有低64bit有效，被当做参与计算的元素数量。高维切分矢量计算接口的repeatTime参数会被忽略，由系统内部根据元素数量自动计算。

    调用示例：

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskCount();
    uint64_t mask[2] = {130, 0};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    ```

    如图2所示，采用Counter模式下的高维切分连续计算接口，mask数组中仅低64bit有效，表示参与计算的元素总数为130。该模式接口内部的repeatTimes参数无效，系统根据mask值自动计算迭代次数。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图2**  Counter模式进行高维切分连续计算示例  
    ![](../../../../../figures/counter_high_dim_continuous.png)<a id="图2-counter模式进行高维切分连续计算示例"></a>

## Normal模式

在[Normal计算模式](概述.md#mask-mode)中，Mask寄存器会作为每个Repeat的掩码使用。只有高维切分支持此模式，在接口上区分为逐bit模式和连续模式。

**注：Normal为默认模式。若未发生模式切换，则无需重复配置该模式。**

- **高维切分：逐bit计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    uint64_t mask[2] = {0x69A569A569A569A5, 0x69A569A569A569A5};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    如图3所示，采用Normal模式进行逐bit计算时，mask参数以位数组形式控制每个元素是否参与计算，详细展示了第一个DataBlock中元素的计算过程。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图3**  Normal模式高维切分逐bit计算示例  
    ![](../../../../../figures/normal_high_dim_bitwise.png)<a id="图3-normal模式高维切分逐bit计算示例"></a>

- **高维切分：连续计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    uint64_t mask = 128;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    如图4所示，采用Normal模式进行连续计算时，mask参数表示迭代内连续参与计算的元素数量为128。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图4**  Normal模式进行高维切分连续计算示例  
    ![](../../../../../figures/normal_high_dim_continuous.png)<a id="图4-normal模式进行高维切分连续计算示例"></a>

> [!NOTE]说明
> 
> 当数据类型为half（操作数为16位）时，每次迭代内能够处理的元素个数mask∈\[1, 128\]。当mask\>128时，仍按一次repeat（128元素）执行，不支持超过128元素的mask控制。
