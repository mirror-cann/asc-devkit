# Utils API列表

**表1**  C++标准库API列表

| 接口名 | 功能描述 |
| --- | --- |
| [max](CPP标准库/算法/max-267.md) | 比较相同数据类型的两个数中的最大值。 |
| [min](CPP标准库/算法/min-268.md) | 比较相同数据类型的两个数中的最小值。 |
| [abs](CPP标准库/数学函数/abs1.md) | 获取输入数据的绝对值。 |
| [sqrt](CPP标准库/数学函数/sqrt1.md) | 计算输入数据的平方根。 |
| [integer_sequence](CPP标准库/通用工具/integer_sequence.md) | 用于生成一个整数序列。 |
| [tuple](CPP标准库/容器函数/tuple.md) | 允许存储多个不同类型元素的容器。 |
| [get](CPP标准库/容器函数/get1.md) | 从tuple容器中提取指定位置的元素。 |
| [make_tuple](CPP标准库/容器函数/make_tuple.md) | 用于便捷地创建tuple对象。 |
| [is_convertible](CPP标准库/类型特性/is_convertible.md) | 在程序编译时判断两个类型之间是否可以进行隐式转换。 |
| [is_base_of](CPP标准库/类型特性/is_base_of.md) | 在程序编译时判断一个类型是否为另一个类型的基类。 |
| [is_same](CPP标准库/类型特性/is_same.md) | 在程序编译时判断两个类型是否完全相同。 |
| [is_void](CPP标准库/类型特性/is_void.md) | 在程序编译时，检测一个类型是否为void类型。 |
| [is_integral](CPP标准库/类型特性/is_integral.md) | 在程序编译时，检测一个类型是否为整数类型。 |
| [is_floating_point](CPP标准库/类型特性/is_floating_point.md) | 在程序编译时，检测一个类型是否为浮点类型。 |
| [is_array](CPP标准库/类型特性/is_array.md) | 在程序编译时，检测一个类型是否为数组类型。 |
| [is_pointer](CPP标准库/类型特性/is_pointer.md) | 在程序编译时，判断一个类型是否为指针类型。 |
| [is_reference](CPP标准库/类型特性/is_reference.md) | 在程序编译时，检测一个类型是否为引用类型。 |
| [is_const](CPP标准库/类型特性/is_const.md) | 在程序编译时，检测一个类型是否为const限定的类型。 |
| [remove_const](CPP标准库/类型特性/remove_const.md) | 在程序编译时，对传入的模板参数类型移除const限定符。 |
| [remove_volatile](CPP标准库/类型特性/remove_volatile.md) | 在程序编译时，对传入的模板参数类型移除volatile限定符。 |
| [remove_cv](CPP标准库/类型特性/remove_cv.md) | 在程序编译时，对传入的模板参数类型移除const限定符或volatile限定符，或同时移除这两种限定符。 |
| [remove_reference](CPP标准库/类型特性/remove_reference.md) | 在程序编译时，从给定类型中移除引用限定符。 |
| [remove_pointer](CPP标准库/类型特性/remove_pointer.md) | 在程序编译时，从给定类型中移除指针限定符。 |
| [add_const](CPP标准库/类型特性/add_const.md) | 在程序编译时，为指定类型添加const限定符。 |
| [add_volatile](CPP标准库/类型特性/add_volatile.md) | 在程序编译时，为指定类型添加volatile限定符。 |
| [add_cv](CPP标准库/类型特性/add_cv.md) | 在程序编译时，为指定类型添加const和volatile限定符。 |
| [add_pointer](CPP标准库/类型特性/add_pointer.md) | 在程序编译时，为指定类型添加指针限定符。 |
| [add_lvalue_reference](CPP标准库/类型特性/add_lvalue_reference.md) | 在程序编译时，为指定类型添加左值引用限定符。 |
| [add_rvalue_reference](CPP标准库/类型特性/add_rvalue_reference.md) | 在程序编译时，为指定类型添加右值引用限定符。 |
| [enable_if](CPP标准库/类型特性/enable_if.md) | 在程序编译时根据某个条件启用或禁用特定的函数模板、类模板或模板特化。 |
| [conditional](CPP标准库/类型特性/conditional.md) | 在程序编译时根据一个布尔条件从两个类型中选择一个类型。 |
| [integral_constant](CPP标准库/类型特性/integral_constant.md) | 用于封装一个编译时常量整数值，是标准库中许多类型特性和编译时计算的基础组件。 |

**表2**  平台信息获取API列表

| 接口名 | 功能描述 |
| --- | --- |
| [PlatformAscendC](平台信息获取/PlatformAscendC/PlatformAscendC.md) | 在实现Host侧的Tiling函数时，可能需要获取一些硬件平台的信息，来支撑Tiling的计算，比如获取硬件平台的核数等信息。PlatformAscendC类提供获取这些平台信息的功能。 |
| [PlatformAscendCManager](平台信息获取/PlatformAscendCManager.md) | 基于Kernel Launch算子工程，通过基础调用（Kernel Launch）方式调用算子的场景下，可能需要获取硬件平台相关信息，比如获取硬件平台的核数。PlatformAscendCManager类提供获取平台信息的功能。 |

**表3**  Tiling调测API列表

| 接口名 | 功能描述 |
| --- | --- |
| [OpTilingRegistry](Tiling调测/OpTilingRegistry/OpTilingRegistry.md) | OpTilingRegistry类属于context_ascendc命名空间，主要用于加载Tiling实现的动态库，并获取算子的Tiling函数指针以进行调试和验证。 |
| [ContextBuilder](Tiling调测/ContextBuilder/ContextBuilder.md) | ContextBuilder类提供一系列的API接口，支持手动构造类来验证Tiling函数以及KernelContext类用于TilingParse函数的验证。 |

**表4**  Tiling模板编程API列表

| 接口名 | 功能描述 |
| --- | --- |
| [模板参数定义](Tiling模板编程/模板参数定义.md) | 通过该类接口进行模板参数ASCENDC_TPL_ARGS_DECL和模板参数组合ASCENDC_TPL_ARGS_SEL（即可使用的模板）的定义。 |
| [GET_TPL_TILING_KEY](Tiling模板编程/GET_TPL_TILING_KEY.md) | Tiling模板编程时，开发者通过调用此接口自动生成TilingKey。该接口将传入的模板参数通过定义的位宽，转成二进制，按照顺序组合后转成uint64数值，即TilingKey。 |
| [ASCENDC_TPL_SEL_PARAM](Tiling模板编程/ASCENDC_TPL_SEL_PARAM.md) | Tiling模板编程时，开发者通过调用此接口自动生成并配置TilingKey。 |

**表5**  RTC API列表

| 接口名 | 功能描述 |
| --- | --- |
| [aclrtcCompileProg](RTC/aclrtcCompileProg.md) | 编译接口，编译指定的程序。 |
| [aclrtcCreateProg](RTC/aclrtcCreateProg.md) | 通过给定的参数，创建编译程序的实例。 |
| [aclrtcDestroyProg](RTC/aclrtcDestroyProg.md) | 销毁编译程序的实例。 |
| [aclrtcGetBinData](RTC/aclrtcGetBinData.md) | 获取编译后的二进制数据。 |
| [aclrtcGetBinDataSize](RTC/aclrtcGetBinDataSize.md) | 获取编译的二进制数据大小。用于在[aclrtcGetBinData](RTC/aclrtcGetBinData.md)获取二进制数据时分配对应大小的内存空间。 |
| [aclrtcGetCompileLogSize](RTC/aclrtcGetCompileLogSize.md) | 获取编译日志的大小。用于在[aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md)获取日志内容时分配对应大小的内存空间。 |
| [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md) | 获取编译日志的内容，以字符串形式保存。 |

**表6**  log API列表

| 接口名 | 功能描述 |
| --- | --- |
| [ASC_CPU_LOG](log/ASC_CPU_LOG.md) | 提供Host侧打印Log的功能。开发者可以在算子的TilingFunc代码中使用ASC_CPU_LOG_XXX接口来输出相关内容。 |

**表7**  调测接口列表

| 接口名 | 功能描述 |
| --- | --- |
| [printf](调测接口/printf-290.md) | 在算子Kernel侧的实现代码中，需要输出日志信息时，调用printf接口打印相关内容。 |
| [assert](调测接口/assert-291.md) | 本接口在SIMT VF调试场景下提供assert断言功能。在算子Kernel侧的SIMT VF实现代码中，如果assert的内部条件判断不为真，则会输出assert条件，并将输入的信息格式化打印在屏幕上。 |
| [__trap](调测接口/__trap.md) | 在SIMT VF实现代码中调用此接口会中断算子的运行。 |
| [clock](调测接口/clock.md) | 本接口在SIMT VF调试场景中提供Clock时间戳功能，用于记录从程序启动到接口调用时刻所经历的时钟周期数（Cycle Count），便于精确分析执行延迟和性能瓶颈。 |
