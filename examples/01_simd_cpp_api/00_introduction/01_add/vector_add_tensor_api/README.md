# VectorAdd算子直调样例（Tensor API）

## 概述

本样例介绍基于 Tensor API 的 Add 算子核函数直调方法，算子支持单核运行。与传统的 Ascend C Vector API 不同，本样例使用 `tensor_api` 提供的 `MakeTensor`、`MakeCopy`、`Transform` 等接口完成数据的搬运与计算，展示了更高层次的张量编程模型。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```
├── vector_add_tensor_api
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── run.sh                   // 一键编译运行脚本
│   └── vector_add.asc           // Ascend C算子实现 & 调用样例（含数据生成与结果验证）
```

## 算子描述

- 算子功能：
  Add算子实现了两个数据相加，返回相加结果的功能。对应的数学表达式为：
  ```
  z = x + y
  ```
- 算子规格：
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">1 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">1 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">z</td><td align="center">1 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>
- 算子实现：
  本样例使用 Tensor API 编程模型，核心流程如下：

  1. 通过 `MakeTensor` + `MakeFrameLayout<NDExtLayoutPtn>` 创建 GM 和 UB 侧的张量对象。
  2. 通过 `MakeCopy(CopyGM2UB{})` / `MakeCopy(CopyUB2GM{})` 创建拷贝原子操作，使用 `Copy` 接口完成 GM 与 UB 之间的数据搬运。
  3. 使用 `Transform<Add>` 对两个输入张量执行逐元素加法运算。
  4. 通过 `asc_sync_notify` / `asc_sync_wait` 进行流水线同步，确保搬运与计算之间的依赖关系。

  主机侧程序在 `main` 函数中生成随机输入数据并计算 golden 结果，核函数执行后将 Device 侧结果拷贝回 Host，逐元素对比验证正确性。

  - 调用实现
    使用内核调用符 `<<<>>>` 调用核函数，并传入 Device 侧内存地址和数据长度。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

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

- 方式一：使用 run.sh 一键执行
  ```bash
  bash run.sh
  ```

- 方式二：手动编译运行
  ```bash
  mkdir -p build && cd build   # 创建并进入build目录
  cmake .. && make -j4         # 编译工程
  ./demo                       # 执行样例（内含随机数据生成与结果验证）
  ```

  执行结果如下，说明精度对比成功。
  ```
  CompareResult passed, all 256 elements are correct.
  ```
