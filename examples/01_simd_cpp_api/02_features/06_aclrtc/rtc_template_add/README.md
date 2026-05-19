# Aclrtc运行时编译 + 静态Tensor编程Add样例

## 概述

本样例展示了如何使用 Ascend C 的 **Aclrtc（运行时编译）** 模式，在 Host 侧动态编译核函数源码并执行。核函数内部采用 **静态 Tensor 编程范式**（LocalMemAllocator + SetFlag/WaitFlag 事件同步），将核函数源码以字符串形式嵌入 Host 代码中，通过 aclrtc API 在运行时编译并执行。

与传统自定义算子工程不同，Aclrtc 无需提前编译算子包（.run），适合快速验证和原型开发。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── rtc_template_add
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── data_utils.h                     // 数据生成、精度校验工具函数 + CHECK_ACL 宏
│   └── rtc_template_add.cpp      // Host 侧代码（含 RTC 核函数源码字符串）
```

## 样例描述

- 样例功能：  
  Add 计算公式：

  ```
  z = x + y
  ```

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">Kernel::add_custom&ltfloat&gt</td></tr>
  </table>

- 样例实现：

  - Kernel实现  
    核函数源码以字符串形式嵌入 Host 代码中，通过 LocalMemAllocator 接口完成内存管理，通过 SetFlag/WaitFlag 接口完成事件同步，实现数据搬运、计算和结果搬出的完整流程。

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
      9. `aclrtKernelArgsAppend` — 逐个追加参数（核函数为 `GM_ADDR x, GM_ADDR y, GM_ADDR z`，对应传入三个 Device 内存指针）
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程，支持dav-3510, dav-2201
  ./main                                                                    # 执行编译生成的可执行程序，执行样例
  ```

- 编译选项说明

| 选项 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

  > [!WARNING] 注意
  Aclrtc 仅支持 NPU 运行模式，不支持 cpu 调试或 sim 仿真模式。

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
