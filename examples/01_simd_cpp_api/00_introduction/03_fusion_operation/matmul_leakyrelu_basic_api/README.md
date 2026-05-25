# Matmul和LeakyRelu融合计算样例

## 概述

本样例基于静态Tensor编程模式实现Matmul和LeakyRelu融合计算，展示Cube单元和Vector单元协同计算的编程模式。硬件Cube:Vector核数比例为1:2，Cube核完成矩阵乘计算，Vector核完成LeakyRelu激活计算。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── matmul_leakyrelu_basic_api
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py            // 真值对比文件
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // 数据读入写出函数
│   └── matmul_leakyrelu_basic_api.asc  // Ascend C样例实现 & 调用样例
```

## 样例描述

- 样例功能：
  实现Matmul和LeakyRelu融合计算，计算公式如下：

  Matmul计算：
  $$
  C = A \times B
  $$

  LeakyRelu计算：
  $$
  C = \begin{cases}
  C & \text{if } C \geq 0 \\
  C \times 0.001 & \text{if } C < 0
  \end{cases}
  $$

  其中，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]；C为输出矩阵，形状为[M, N]。

- 样例规格：
  本样例参数M = 512, K = 512, N = 1024，调用4个Cube核和8个Vector核完成计算，输入规格如下表所示：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul+LeakyRelu融合</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A（左矩阵）</td><td align="center">[512, 512]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B（右矩阵）</td><td align="center">[512, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[512, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_vec_custom</td></tr>
  </table>

  **分核逻辑**

  输出矩阵C按M和N方向分块，共分为4个分块，每个分块由1个Cube核完成Matmul计算，每个Cube核的计算结果由2个Vector核协同完成LeakyRelu计算。
  分核参数说明：
  - M方向分块数：M / singleCoreM = 512 / 256 = 2
  - N方向分块数：N / singleCoreN = 1024 / 512 = 2
  - 总Cube核数：2 × 2 = 4
  - 总Vector核数：4 × 2 = 8（Cube:Vector = 1:2）
  - 每个Cube核负责singleCoreM\*singleCoreN的Matmul计算，每次计算产生baseM\*baseN的结果
  - 每个Vector核负责singleCoreM/2*singleCoreN的计算，每次计算产生baseM/2\*baseN的结果

- 样例实现：
  - 整体计算流程：

    样例整体流程如下（展示Cube核和Vector核协同计算）：

**Cube核：Matmul矩阵乘**
```
GM(A:ND,half) -> L1(A:Nz,half) -> L0A(A:Nz,half) -
             │                  │                 │
          DataCopy            LoadData            │
          ND->Nz              Nz->Nz              │
                                                  │--->L0C(Nz,float) -> GM(C:ND,half)
                                                  │  │                  │
                                                  │ Mmad             Fixpipe
                                                  │ C=A×B       Nz->ND, float32->half
                                                  │
GM(B:ND,half) -> L1(B:Nz,half) -> L0B(B:Zn,half) -
             │                  │
          DataCopy            LoadData
          ND->Nz              Nz->Zn(转置)
```

  > **注意**：L0A分型在不同产品上有差异：
  > - Ascend 950PR/Ascend 950DT产品：L0A的分型为Nz
  > - Atlas A2/A3系列产品：L0A的分型为Zz

**核间同步**
  ```
  Cube核产生计算结果 -> CrossCoreSetFlag -> Vector核等待(CrossCoreWaitFlag)
  ```

  **Vector核（1个cube核的结果结果传递给2个vector核完成计算）：LeakyRelu激活**
  ```
  GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                │                   │                   │
            DataCopyPad          LeakyRelu          DataCopyPad
      MTE2搬运（baseM/2×baseN）    VEC计算            MTE3写出

  ```

  **流程详解**：

  1. **Cube核计算阶段**：
      - **GM → L1**：使用`DataCopy`将A、B矩阵从GM搬运到L1，完成ND到Nz格式转换
      - **L1 → L0A/L0B**：使用`LoadData`将数据搬运到L0A和L0B，B矩阵需要转置（Nz→Zn）
      - **L0A/L0B → L0C**：使用`Mmad`执行矩阵乘加，累加K轴方向的所有数据块
      - **L0C → GM**：使用`Fixpipe`将结果搬出到GM，完成Nz到ND格式转换和float32到half类型转换

  2. **核间同步**：
      - Cube核完成一个baseM×baseN的Matmul结果后，通过`CrossCoreSetFlag`设置标志
      - Vector核通过`CrossCoreWaitFlag`等待标志，确保Matmul计算完成后才开始LeakyRelu

  3. **Vector核计算阶段**：
      - **GM → UB**：使用`DataCopyPad`将Matmul结果搬运到UB，每个Vector核处理baseM/2×baseN的数据
      - **UB计算**：使用`LeakyRelu`执行激活计算，负值部分乘以0.001
      - **UB → GM**：使用`DataCopyPad`将结果写回GM，完成融合计算

  4. **核数比例**：
      - Cube:Vector核数比例为1:2，每个Cube核产生baseM×baseN的结果
      - 2个Vector核各处理baseM/2×baseN的数据，共同完成一个Matmul块的激活计算

- Cube与Vector协同机制：
  - **核数比例**：Cube:Vector核数比例为1:2，每个Cube核的计算结果由2个Vector核协同完成LeakyRelu计算
  - **核间同步**：Cube核完成Matmul计算后，通过CrossCoreSetFlag通知Vector核开始计算；Vector核通过CrossCoreWaitFlag等待Cube核完成
  - **数据切分**：每个Vector核处理baseM/2 × baseN大小的数据，两个Vector核共同处理一个baseM × baseN的Matmul结果块

  - 约束条件：
    1. baseM/baseK/baseN满足16对齐
    2. baseM/baseK/baseN能被singleCoreM/singleCoreK/singleCoreN整除
    3. singleCoreM/singleCoreK/singleCoreN能被M/K/N整除，不支持非整切场景
    4. Vector核数是Cube核数的2倍

  - 调用实现
    核调用符`__mix__(1, 2)`实现Cube和Vector核的协同调用，其中参数(1, 2)表示Cube:Vector核数比例为1:2。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```