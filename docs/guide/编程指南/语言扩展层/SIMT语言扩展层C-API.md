# SIMT语言扩展层C API<a name="ZH-CN_TOPIC_0000002509743873"></a>

SIMT编程基于AI Core的硬件能力实现，当前，SIMT语言扩展层支持的C API类别如下：

-   [同步与内存栅栏](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10314.html)：提供内存管理与同步接口，解决不同核内的线程间可能存在的数据竞争以及线程的同步问题。
-   [原子操作](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10375.html)：提供对Unified Buffer或Global Memory上的数据与指定数据执行原子操作的一系列API接口。
-   [Warp函数](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10386.html)：提供对单个Warp内32个线程的数据进行处理的相关操作的一系列API接口。
-   [数学函数](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10309.html)：提供处理数学运算的函数接口集合。
-   [访存函数](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10555.html)：提供开启Cache Hints的Load/Store函数。
-   [地址空间谓词函数](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10812.html)：提供判断输入指针所属地址空间的函数接口集合。
-   [地址空间转换函数](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10816.html)：提供不同地址空间地址值与指针之间相互转换的函数接口集合。
-   [协作组](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_11067.html)：提供更细粒度的线程间协作的API接口。
-   [调测接口](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta3/API/ascendcopapi/atlasascendc_api_07_10425.html)：SIMT VF调试场景下使用的相关接口。
