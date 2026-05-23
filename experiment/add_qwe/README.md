# SIMT编程模式实现Add算子样例

## 概述

样例基于Ascend C SIMT编程方式实现Add算子，实现两个输入张量逐元素相加得到输出张量的功能，展示SIMT编程的基本流程。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.0.0

## 目录结构介绍

```text
├── 01_add
│   ├── CMakeLists.txt      // cmake编译文件
│   ├── add.asc             // SIMT实现add调用样例
│   └── README.md
```

## 算子描述

- 算子功能：  
  本算子实现了形状为48 * 256的两个张量x，y相加得到算子输出z。第i个元素的计算公式为：
  
  ```
  z[i] = x[i] + y[i]
  ```

- 算子规格：  
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">48 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">48 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">z</td><td align="center">48 * 256</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- 数据切分：  
  * 核数：48核
  * 每核线程数：256线程
  * 单线程处理：1个元素
  * 总处理能力：48×256=12288

- 算子实现：  
  算子的实现流程为从输入x（Global Memory上的指针）中获取指定索引的数据。基于上述数据切分，首先计算线程应处理数据的索引，然后通过加法运算符计算得到输出值。

- 调用实现：  
  使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
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
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```




  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy is verification passed.
  ```

## 体验任务

参考add算子，实现减法、乘法、除非算子功能（任选一个即可），可以拷贝add样例代码，并按照以下修改点修改：

- 修改kernel实现：
  ```
  // 源代码中的32行 改为对应实现代码， 比如 z[idx] = x[idx] - y[idx];
  z[idx] = x[idx] + y[idx]; // add 实现
  ```

- 修改生成golden数据代码

  ```
  // 源代码中的125行 改为对应实现代码， 比如 golden[i] = x[i] + y[i];
  golden[i] = x[i] + y[i]; // add golden生成
  ```

- 样例执行  

  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```
执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy is verification passed.
  ```

删除 build临时编译路径（`rm -rf build`）, 并让支撑人员帮忙提交代码。
