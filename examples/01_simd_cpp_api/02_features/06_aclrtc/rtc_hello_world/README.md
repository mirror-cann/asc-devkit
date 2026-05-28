# Aclrtc运行时编译+printf打印样例

## 概述

本样例展示了如何使用**Aclrtc（运行时编译）**接口，在Host侧运行时编译核函数源码并执行，核函数通过printf打印输出结果。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3训练系列产品/Atlas A3推理系列产品
- Atlas A2训练系列产品/Atlas A2推理系列产品

## 目录结构介绍

```
├── rtc_hello_world
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── rtc_hello_world.cpp      // Host侧代码（含RTC核函数源码字符串）
```

## 样例描述

### Kernel实现

核函数源码以字符串形式嵌入Host代码中，通过printf打印输出结果。

### 调用实现

调用aclrtc接口系列在运行时编译并执行核函数，完整链路如下：

#### 编译阶段
1. `aclrtcCreateProg` — 创建编译程序对象，传入核函数源码字符串
2. `aclrtcAddNameExpr` — 注册需要导出的核函数名（含模板参数，如`Kernel::add_custom<float>`）
3. `aclrtcCompileProg` — 执行运行时编译，通过options传入`--npu-arch`指定NPU架构
4. `aclrtcGetBinDataSize`/`aclrtcGetBinData` — 获取编译产物的二进制大小及数据（deviceELF）
5. `aclrtcGetLoweredName` — 获取核函数编译后的mangledname，用于后续查找

#### 加载阶段
1. `aclrtBinaryLoadFromData` — 将编译产物的二进制加载到设备（通过`ACL_RT_BINARY_MAGIC_ELF_AICORE`标记为AICore可执行）
2. `aclrtBinaryGetFunction` — 从加载的二进制中获取核函数句柄（`funcHandle`）

#### 参数配置阶段
1. `aclrtKernelArgsInit` — 初始化核函数参数句柄
2. `aclrtKernelArgsAppend` — 逐个追加参数（核函数为`__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z`，对应传入三个Device内存指针）
3. `aclrtKernelArgsFinalize` — 完成参数配置

#### 执行阶段
1. `aclrtLaunchKernelWithConfig` — 启动核函数，指定block数量、stream等执行配置

#### 资源清理
1. `aclrtcDestroyProg` — 销毁编译程序对象

数据生成与精度校验均在Host侧C++内完成，不依赖外部脚本。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
  - 默认路径，root用户安装CANN软件包
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - 默认路径，非root用户安装CANN软件包
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - 指定路径install_path，安装CANN软件包
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- 样例执行
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  ```

  > [!WARNING] 注意
  Aclrtc仅支持NPU运行模式，不支持cpu调试或sim仿真模式。

- 执行结果  
  执行结果如下，说明执行成功。
  
  ```bash
  [AIV Block 0/8] Hello World!!!
  [AIV Block 1/8] Hello World!!!
  [AIV Block 2/8] Hello World!!!
  [AIV Block 3/8] Hello World!!!
  [AIV Block 4/8] Hello World!!!
  [AIV Block 5/8] Hello World!!!
  [AIV Block 6/8] Hello World!!!
  [AIV Block 7/8] Hello World!!!
  ```
