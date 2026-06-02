# Aclrtc运行时编译+printf打印样例

## 概述

本样例展示了如何使用AscendC的 **Aclrtc（运行时编译）** 模式，在Host侧动态编译核函数源码并执行。核函数为一个简单的`hello_world`函数，通过printf打印输出结果，核函数源码以字符串形式嵌入Host代码中，通过aclrtc API在运行时编译并执行。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3训练系列产品/Atlas A3推理系列产品
- Atlas A2训练系列产品/Atlas A2推理系列产品

## 支持的CANN软件版本

- \>= CANN 9.0.0

## 目录结构介绍

```
├── rtc_hello_world
│   ├── CMakeLists.txt           // 编译工程文件
│   └── rtc_hello_world.cpp      // Host侧代码（含RTC核函数源码字符串）
```

## 样例描述

### Kernel实现

核函数源码以字符串形式嵌入Host代码中，通过printf打印输出结果。

### 调用实现

本样例的核心流程分为两个阶段：**编译阶段**（aclrtc 运行时编译核函数源码为 deviceELF）和 **执行阶段**（aclrt 加载 deviceELF 到设备并启动核函数）。

**1. 编译阶段：aclrtc 接口**

- `aclrtcCreateProg` — 创建编译程序对象，传入核函数源码字符串
- `aclrtcCompileProg` — 执行运行时编译，通过options传入`--npu-arch`指定NPU架构
- `aclrtcGetCompileLogSize` / `aclrtcGetCompileLog` —（可选）编译失败时获取错误日志
- `aclrtcGetBinDataSize` / `aclrtcGetBinData` — 获取编译产物的二进制大小及数据（deviceELF）
- `aclrtcDestroyProg` — 销毁编译程序对象

**2. 执行阶段：关键 aclrt 接口**

- `aclrtBinaryLoadFromData` — 将编译产物加载到设备
- `aclrtBinaryGetFunction` — 通过核函数名`hello_world`获取核函数句柄
- `aclrtKernelArgsInit` / `aclrtKernelArgsFinalize` — 配置核函数参数（本样例核函数无参数，跳过`aclrtKernelArgsAppend`）
- `aclrtLaunchKernelWithConfig` — 启动核函数，指定8个block执行
- `aclrtSynchronizeDevice` — 等待Device侧执行完成

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  ```

- 编译选项说明

| 选项 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

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
