# CPU Debug直调样例说明

## 概述

本样例通过Ascend C编程语言实现了Add算子的CPU Debug调测。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
 
## 目录结构介绍

```
├── 03_cpudebug
│   ├── CMakeLists.txt          // 编译工程文件
│   └── add.asc                 // Ascend C算子实现 & 调用样例
```

## 样例描述

- CPU Debug介绍：  
  CPU Debug功能支持对CPU执行过程中的运行状态进行调试，主要通过GDB工具实现。GDB调试支持设置断点、查看寄存器和内存状态、单步执行、查看调用栈等常用调试操作。

- 算子介绍：  
  Add算子具体功能描述可参考[Add算子详情](../../00_introduction/01_vector/add_tpipe_tque/README.md)章节。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
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
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  ./add
  ```
  请根据实际测试的 NPU 硬件架构选择对应的 `CMAKE_ASC_ARCHITECTURES` 参数
  - 编译选项说明
    | 选项 | 说明 |
    |------|------|
    | `CMAKE_ASC_RUN_MODE` | 指定为`cpu`, 开启CPU域编译 |
    | `CMAKE_ASC_ARCHITECTURES` | 指定NPU架构版本号，CMake会根据该值配置对应的CPU调试依赖库。<br>`dav-2201` 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，`dav-3510` 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明执行成功。
  ```bash
  [Success] Case accuracy is verification passed.
  ```
- 进入gdb模式调试  
  编译生成的CPU域可执行程序支持通过gdb进行调试。gdb支持设置断点、查看寄存器和内存状态、单步执行、查看调用栈等常用调试操作。在上述指令中"./add"前加入"gdb --args"，再次执行指令即可进入gdb模式。
  ```bash
  gdb --args ./add
  ```
  CPU Debug通过为每个核函数启动单独的子进程来模拟NPU的执行逻辑，因此使用gdb调试时，需要设置`follow-fork-mode`让gdb跟踪子进程，才能在核函数内部断点调试。进入gdb后，先设置跟踪子进程模式：

  ```text
  (gdb) set follow-fork-mode child
  ```

  然后按需进行调试，常用操作：

  ```text
  # 在核函数入口处设置断点
  (gdb) break Compute

  # 运行程序
  (gdb) run

  # 单步执行
  (gdb) next

  # 打印变量值
  (gdb) print xLocal.GetValue(0)

  # 继续执行到下一个断点
  (gdb) continue
  ```
