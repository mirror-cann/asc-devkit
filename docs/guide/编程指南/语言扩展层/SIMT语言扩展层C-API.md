# SIMT语言扩展层C API<a name="ZH-CN_TOPIC_0000002509743873"></a>

SIMT编程基于AI Core的硬件能力实现，当前，SIMT语言扩展层支持的C API类别如下：

-   [同步与内存栅栏](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/同步与内存栅栏/同步与内存栅栏.md)：提供同步与内存栅栏接口，解决线程间可能存在的数据竞争以及线程的同步问题。
-   [原子操作](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/原子操作/原子操作-149.md)：提供对Unified Buffer或Global Memory上的数据与指定数据执行原子操作的一系列API接口。
-   [Warp函数](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/Warp函数/Warp函数.md)：提供对单个Warp内32个线程的数据进行处理的相关操作的一系列API接口。
-   [数学函数](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/数学函数/数学函数.md)：提供处理数学运算的函数接口集合。
-   [访存函数](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/访存函数/访存函数.md)：提供开启Cache Hints的Load/Store函数。
-   [地址空间谓词函数](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/地址空间谓词函数/地址空间谓词函数.md)：提供判断输入指针所属地址空间的函数接口集合。
-   [地址空间转换函数](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/地址空间转换函数/地址空间转换函数.md)：提供不同地址空间地址值与指针之间相互转换的函数接口集合。
-   [协作组](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/SIMT-API/协作组/协作组.md)：提供更细粒度的线程间协作的API接口。
-   [调测接口](https://gitcode.com/cann/asc-devkit/blob/master/docs/api/Utils-API/调测接口/调测接口.md)：SIMT调试场景下使用的相关接口。
