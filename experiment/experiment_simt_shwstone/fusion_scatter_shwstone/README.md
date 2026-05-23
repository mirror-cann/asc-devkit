# SIMT编程实现Scatter算子样例

## 概述

样例基于Ascend C SIMT编程实现一维Scatter算子，将输入数组中的元素按照`index`指定的位置离散写入输出数组。样例支持配置不同的一维输入长度和输出长度，默认构造`input_length = 10`、`output_length = 50`的稀疏写入场景。该样例重点展示SIMT编程适合处理离散访存、不规则访存这类场景：每个线程可以按照独立的`index`计算自己的写回地址，直接表达“按元素分散写入”的并行逻辑。

对于Scatter这类算子，输出地址通常不是连续递增的，而是由输入索引动态决定。SIMT编程可以让一个线程负责一个或多个元素，线程内部完成索引读取、边界判断和离散写回，避免把不规则地址映射强行整理成连续访问流程。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构

```
├── basic_scatter
│   ├── CMakeLists.txt
│   ├── scatter.asc             # SIMT实现scatter调用样例
|   └── README.md
```

## 算子描述

- 算子功能：  
  scatter算子将一维输入数组`input`中的元素按照`index`指定的位置写入一维输出数组`output`。对输入数组中的第`i`个元素，算子输出计算公式为：
  
  ```
  output[index[i]] = input[i]
  ```
  
  输出数组初始值为0。本样例构造的`index`不会让多个线程写入同一输出位置，避免并发写冲突影响结果校验。默认用例中`input`只写入长度为50的`output`中的10个离散位置，未被写入的位置保持初始值0。

- SIMT适用场景说明：  
  本样例的核心访问模式是离散写：输入数组`input`按线性位置读取，输出数组`output`按`index[i]`指定的位置写入。不同线程处理不同的`i`，每个线程的输出地址可能相隔较远，也可能呈现完全不规则分布。SIMT线程模型能够直接描述这种“每个线程独立计算访存地址并完成写回”的逻辑，因此适合用于Scatter、Gather、索引更新、稀疏数据访问等不规则访存场景。

- 算子规格：  
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">scatter</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">input_length，默认10</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">input_length，默认10</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">output</td><td align="center">output_length，默认50</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">scatter_custom</td></tr>
  </table>

- 数据切分：  
  * 最大线程数：2048线程/Block
  * Block数量：根据input元素数量和AI Vector Core数量自动计算
  * 单线程处理：通过grid-stride循环处理一个或多个input元素

- 算子实现：  
  scatter算子的实现流程为先将output初始化为0，再从Global Memory读取input和index。核函数通过grid-stride循环分配元素，每个线程根据当前线性位置读取`index[i]`，独立计算离散写地址，并将`input[i]`写入`output[index[i]]`。host侧根据`input_length`和`output_length`生成无重复的稀疏`index`，要求`input_length <= output_length`，以保证样例校验不受并发写冲突影响。这种实现保留了离散访存的原始形态，便于观察和理解SIMT在不规则地址访问场景下的编程方式。

- 调用实现：  
  使用内核调用符<<<>>>调用核函数。

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
    
- 样例执行  
```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake ..; make -j;            # 编译工程
  ./scatter                     # 执行默认样例，input_length=10，output_length=50
  ./scatter 8 32                # 可选：执行指定shape样例，参数为input_length output_length
```
  执行结果如下，说明精度对比成功。
  ```
  [INFO] input_length=10, output_length=50
  Output: 0 10 0 0 0 0 20 0 0 0 0 30 0 0 0 0 40 0 0 0 ...
  Golden: 0 10 0 0 0 0 20 0 0 0 0 30 0 0 0 0 40 0 0 0 ...
  [Success] Case accuracy is verification passed.
  ```
