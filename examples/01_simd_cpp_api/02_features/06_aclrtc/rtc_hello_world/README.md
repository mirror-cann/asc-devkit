# Aclrtc运行时编译 + printf打印样例

## 概述

本样例展示了如何使用 **Aclrtc（运行时编译）** 接口，在 Host 侧运行时编译核函数源码并执行，核函数通过printf打印输出结果。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── rtc_hello_world
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── rtc_hello_world.cpp      // Host 侧代码（含 RTC 核函数源码字符串）
```

## 样例描述

- 样例实现：
  - Kernel实现  
    核函数源码以字符串形式嵌入 Host 代码中，通过printf打印输出结果。

  - 调用实现  
    调用 aclrtc 接口系列在运行时编译并执行核函数，完整链路如下：

    - **编译阶段**
      1. `aclrtcCreateProg` — 创建编译程序对象，传入核函数源码字符串
      2. `aclrtcAddNameExpr` — 注册需要导出的核函数名（含模板参数，如 `Kernel::add_custom<float>`）
      3. `aclrtcCompileProg` — 执行运行时编译，通过 options 传入 `--npu-arch` 指定 NPU 架构
      4. `aclrtcGetBinDataSize` / `aclrtcGetBinData` — 获取编译产物的二进制大小及数据（device ELF）
      5. `aclrtcGetLoweredName` — 获取核函数编译后的 mangled name，用于后续查找

    - **加载阶段**
      6. `aclrtBinaryLoadFromData` — 将编译产物的二进制加载到设备（通过 `ACL_RT_BINARY_MAGIC_ELF_AICORE` 标记为 AI Core 可执行）
      7. `aclrtBinaryGetFunction` — 从加载的二进制中获取核函数句柄（`funcHandle`）

    - **参数配置阶段**
      8. `aclrtKernelArgsInit` — 初始化核函数参数句柄
      9. `aclrtKernelArgsAppend` — 逐个追加参数（核函数为 `__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z`，对应传入三个 Device 内存指针）
      10. `aclrtKernelArgsFinalize` — 完成参数配置

    - **执行阶段**
      11. `aclrtLaunchKernelWithConfig` — 启动核函数，指定 block 数量、stream 等执行配置

    - **资源清理**
      12. `aclrtcDestroyProg` — 销毁编译程序对象

    数据生成与精度校验均在 Host 侧 C++ 内完成，不依赖外部脚本。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上 CANN 开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
  - 默认路径，root 用户安装 CANN 软件包
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - 默认路径，非 root 用户安装 CANN 软件包
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - 指定路径 install_path，安装 CANN 软件包
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
  Aclrtc 仅支持 NPU 运行模式，不支持 cpu 调试或 sim 仿真模式。

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
