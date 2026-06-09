# 基于Tensor API实现的Mmad随路量化样例

## 概述

本样例基于Tensor API编程方式实现动态Shape矩阵乘法和随路量化计算。矩阵A不转置，矩阵B转置存储，A/B矩阵数据类型为int8_t，Mmad累加类型为int32_t，输出矩阵C数据类型为half。量化系数quant以uint64_t格式输入，在Fixpipe阶段对L0C中的int32_t累加结果进行缩放并写回GM。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```text
├── mmad_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py            // 真值对比文件
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // 数据读取写出函数
│   ├── mmad_tensor_api.asc             // Ascend C样例实现与调用样例
│   └── README.md                       // 样例说明文档
```

## 样例描述

- 样例功能：

  本样例实现了多核动态Shape Matmul和随路量化功能。

  1. 动态Matmul功能

     样例在Kernel中通过Tensor API构造GM、L1 Buffer、L0A Buffer、L0B Buffer、L0C Buffer上的Tensor对象，使用Copy接口完成GM到L1 Buffer、L1 Buffer到L0 Buffer的分块搬运，并通过Mmad接口完成矩阵乘法。矩阵A按ND布局读取，矩阵B按DN布局读取（输入文件中以转置后的[N, K]保存）。Shape参数（M、N、K、singleCoreM、singleCoreN、singleCoreK）通过在运行时传递到Kernel中，baseM、baseN、baseK和stepM、stepN、stepK、ENABLE_UNITFLAG作为编译期模板参数决定L1 Buffer/L0 Buffer分配大小和分块步进。

  2. Fixpipe随路量化功能/反量化

     对于特定输入输出数据类型，Fixpipe支持将计算结果从L0C Buffer搬出到GM时，通过配置Fixpipe的量化模式和量化参数，对输出C矩阵元素执行数据量化操作。
     Fixpipe提供了两种不同粒度的随路量化，即per_tensor和per_channel。
      1. per_tensor：对整个Tensor进行量化/反量化，Tensor具有唯一的缩放因子。这种方法可以降低模型的存储和计算成本，但会降低模型的精度。
      2. per_channel：对Tensor的每个通道单独进行量化/反量化，同一通道内共享同一缩放因子，通道间缩放因子则各不相同。这种方法可以更好地保留模型的精度，但会增加模型的存储和计算成本。

  3. ENABLE_UNITFLAG

     样例通过`ENABLE_UNITFLAG`模板参数控制unitFlag功能的开关，默认值为true。
     unitFlag具体介绍可见[Mmad unitFlag特性样例](../mmad_unitflag/README.md)。

- 样例规格：

  在核函数直调样例中，样例默认支持的shape为：M=1024、N=1024、K=256。

  <table border="2">
  <caption>表1：样例规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Quant</td></tr>
  <tr><td rowspan="4" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x (矩阵A)</td><td align="left">[M, K]</td><td align="left">int8_t</td><td align="left">ND</td></tr>
  <tr><td align="left">y (矩阵B，转置存储)</td><td align="left">[N, K]</td><td align="left">int8_t</td><td align="left">DN</td></tr>
  <tr><td align="left">quant</td><td align="left">[N]</td><td align="left">uint64_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">z (矩阵C)</td><td align="left">[M, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">mmad_quant_custom</td></tr>
  </table>

  编译期模板参数默认值：

  | 参数    | 默认值 | 说明                    |
  | :------ | :----- | :---------------------- |
  | BASE_M  | 256    | M维度基础分块大小       |
  | BASE_N  | 256    | N维度基础分块大小       |
  | BASE_K  | 64     | K维度基础分块大小       |
  | STEP_M  | 1      | M维度L1 Buffer缓存步进  |
  | STEP_N  | 1      | N维度L1 Buffer缓存步进  |
  | STEP_K  | 4      | K维度L1 Buffer缓存步进  |
  | ENABLE_UNITFLAG  | true      | unitFlag功能的开关         |

  运行时动态参数默认值：

  | 参数         | 默认值 | 说明                       |
  | :----------- | :----- | :------------------------- |
  | m            | 1024   | 矩阵A的行数                |
  | n            | 1024   | 矩阵B的列数                |
  | k            | 256    | 矩阵A的列数/矩阵B的行数    |
  | singleCoreM  | 256    | 单核M维度大小              |
  | singleCoreN  | 128    | 单核N维度大小              |
  | singleCoreK  | 256    | 单核K维度大小              |
  | NUM_BLOCKS   | 32     | Kernel启动的Block数量      |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](https://gitcode.com/cann/asc-devkit/blob/master/docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令，**当前仅支持使用CANN master**。
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
  cmake ..;make -j;             # 编译工程
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理CMake缓存，可在build目录下执行`rm CMakeCache.txt`后重新CMake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | :--- | :----- | :--- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510对应Ascend 950PR/Ascend 950DT |

- 执行结果
  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
