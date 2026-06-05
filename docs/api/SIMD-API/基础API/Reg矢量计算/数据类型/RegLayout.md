# RegLayout<a name="ZH-CN_TOPIC_0000002523863827"></a>

```
enum class RegLayout {
    UNKNOWN = -1,
    ZERO,
    ONE,
    TWO,
    THREE
};
```

源操作数与目的操作数类型位宽不同时，可以分为四种情况，用户可以根据实际场景选择对应的RegLayout，其中，mask的有效情况均以输入数据位宽为准。

-   源操作数与目的操作数位宽比为1:2。

    例如，ExpSub接口输入数据类型为half\(b16\)，输出数据类型为float\(b32\)时，mask每2位有效，RegLayout确定源操作数中，每2个half类型数据参与计算的数据的位置。

    ![](../../../../figures/reglayout_1_2.png)

-   源操作数与目的操作数位宽比为2:1。

    例如，Cast接口输入数据类型为float\(b32\)，输出数据类型为half\(b16\)，mask每4位有效，RegLayout确定目的操作数中，每2个half类型数据有效数据的位置。

    ![](../../../../figures/reglayout_2_1.png)

-   源操作数与目的操作数位宽比为1:4。

    例如，Cast接口输入数据类型为int8\_t\(b8\)，输出数据类型为int32\_t\(b32\)，mask每1位有效，RegLayout确定源操作数中，每4个int8\_t类型数据参与计算的数据的位置。

    ![](../../../../figures/reglayout_1_4.png)

-   源操作数与目的操作数位宽比为4:1。

    例如，Cast接口输入数据类型为int32\_t\(b32\)，输出数据类型为uint8\_t\(b8\)，mask每4位有效，RegLayout确定目的操作数中，每4个uint8\_t类型数据有效数据的位置。

    ![](../../../../figures/reglayout_4_1.png)

