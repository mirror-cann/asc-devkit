# Matmul和LeakyRelu融合计算样例

## 概述

本样例基于静态Tensor编程模式实现Matmul和LeakyRelu融合计算，展示Cube单元和Vector单元协同计算的编程模式。Cube核完成矩阵乘计算，Vector核完成LeakyRelu激活计算。

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
  本样例参数M = 512, K = 128, N = 128，调用2个Cube核和4个Vector核完成计算，输入规格如下表所示：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul+LeakyRelu融合</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A（左矩阵）</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B（右矩阵）</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_vec_custom</td></tr>
  </table>

  **分核逻辑**：

  输出矩阵C按M方向分为2个分块，N方向不分块，使用2个Cube核完成Matmul计算；每个Cube核对应2个Vector核，因此使用4个Vector核完成LeakyRelu计算。每个Cube核产生一个baseM×baseN结果块，对应2个Vector核各处理baseM/2×baseN的数据。
  - M方向分块数：M / singleCoreM = 512 / 256 = 2
  - N方向分块数：N / singleCoreN = 128 / 128 = 1
  - 总Cube核数：2 × 1 = 2
  - 总Vector核数：2 × 2 = 4

- 样例实现：
  - **整体计算流程**：

    样例整体流程如下（展示Cube核和Vector核协同计算）：

      **1）Cube核：Matmul矩阵乘**
      ```
      GM(A:ND,half) -> L1(A:Nz,half) -> L0A(A:Nz,half) -
                  │                  │                 │
                DataCopy            LoadData            │
                ND->Nz              Nz->Zz/Nz           │
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


      **2）核间同步**

      ```
      Cube核:  Fixpipe -> GM(C:ND,half)
                          │
                          │ CrossCoreSetFlag / CrossCoreWaitFlag
                          ▼
      Vector核: GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                            │                   │                   │
                        DataCopyPad          LeakyRelu          DataCopyPad
                  MTE2搬运（baseM/2×baseN）    VEC计算            MTE3写出
      ```

      **Vector核（1个Cube核的结果由2个Vector核处理）：LeakyRelu激活**<br>
      上图中，Vector核需要等待对应Cube核完成结果写回后，再读取GM中的Matmul结果并执行LeakyRelu激活。

      **3）流程详解**：

      1. **Cube核计算阶段**：
          - **LocalTensor创建**：使用`LocalMemAllocator`创建A1、B1、A2、B2和CO1位置的`LocalTensor`。其中A、B矩阵在L1中的临时空间由同一个L1 allocator按申请顺序分配，避免手动维护L1地址偏移
          - **GM → L1**：使用`DataCopy`将A、B矩阵从GM搬运到L1，完成ND到Nz格式转换
          - **L1 → L0A/L0B**：使用`LoadData`将数据搬运到L0A和L0B，B矩阵需要转置（Nz→Zn）
          - **L0A/L0B → L0C**：使用`Mmad`执行矩阵乘加，累加K轴方向的所有数据块
          - **L0C → GM**：使用`Fixpipe`将结果搬出到GM，完成Nz到ND格式转换和float32到half类型转换

      2. **核间同步**：
          - Cube核完成一个baseM×baseN的Matmul结果后，通过`CrossCoreSetFlag`设置标志
          - Vector核通过`CrossCoreWaitFlag`等待标志，确保Matmul计算完成后才开始LeakyRelu

      3. **Vector核计算阶段**：
          - **LocalTensor创建**：使用UB allocator创建`VECCALC`位置的`LocalTensor`，用于承载当前Vector核处理的半块结果
          - **GM → UB**：使用`DataCopyPad`将Matmul结果搬运到UB，每个Vector核处理baseM/2×baseN的数据
          - **UB计算**：使用`LeakyRelu`执行激活计算，负值部分乘以0.001
          - **UB → GM**：使用`DataCopyPad`将结果写回GM，完成融合计算

  - **约束条件**：
    1. baseM/baseK/baseN满足16对齐
    2. baseM/baseK/baseN能被singleCoreM/singleCoreK/singleCoreN整除
    3. singleCoreM/singleCoreK/singleCoreN能被M/K/N整除，不支持非整切场景
    4. Vector核数是Cube核数的2倍

  - **调用实现**：
    核调用符`__mix__(1, 2)`实现Cube和Vector核的协同调用，其中参数`(1, 2)`表示Cube:Vector核数比例为1:2。

- **接口参数说明**

  以下结构体均以花括号`{}`方式传参，各字段含义如下（字段顺序与API文档保持一致，实际struct声明中部分字段顺序可能不同）：

  **`AscendC::Nd2NzParams`** — `DataCopy`接口使用，描述ND→Nz格式转换参数：
  ```cpp
  struct Nd2NzParams {
      int32_t  ndNum;              // 传输ND矩阵的数目，[0, 4095]
      uint16_t nValue;             // ND矩阵的行数，[0, 16384]
      int32_t  dValue;             // ND矩阵的列数，[0, 65535]
      int32_t  srcNdMatrixStride;  // 相邻ND矩阵起始地址偏移，单位：元素，[0, 65535]
      int32_t  srcDValue;          // 同一ND矩阵相邻行偏移，单位：元素，[1, 65535]
      uint16_t dstNzC0Stride;      // 目的Nz中同源行转换后多行相邻偏移，单位：C0_SIZE(32B)，[1, 16384]
      uint16_t dstNzNStride;       // 目的Nz中Z型矩阵相邻行偏移，单位：C0_SIZE(32B)，[1, 16384]
      int32_t  dstNzMatrixStride;  // 目的Nz中相邻Nz矩阵起始地址偏移，单位：元素，[1, 65535]
  };
  ```
  例如搬运A矩阵时`{1, baseM, baseK, 0, K, baseM, 1, 0}`，将baseM×baseK的ND数据转为Nz格式。

  **`AscendC::LoadData2DParams`** — `LoadData`接口使用，描述Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：
  ```cpp
  struct LoadData2DParams {
      int32_t startIndex;   // 分形矩阵ID（0为第1个），单位：512B，[0, 65535]
      int32_t repeatTimes;  // 迭代次数，每个迭代处理512B，[1, 255]
      int32_t srcStride;    // 相邻迭代源分形起始地址间隔，单位：512B，[0, 65535]
      int32_t sid;          // 预留，配置为0
      int32_t dstGap;       // 目的端相邻迭代分形间隔，单位：512B，[0, 65535]
      bool    ifTranspose;  // 是否转置每个分形，默认false
      bool    addrMode;     // 地址更新方式，false=递增，true=递减，默认false
  };
  ```
  例如：Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品中，L0A上的排布格式为Zz，搬运A矩阵时`{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0}`；<br>
  搬运B矩阵时`ifTranspose=true`，完成Nz到Zn的转置搬运。

  **`AscendC::LoadData2DParamsV2`** — `LoadData`接口使用，描述Ascend 950PR/Ascend 950DT产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：
  ```cpp
  struct LoadData2DParamsV2 {
      uint32_t mStartPosition;  // M方向起始位置，单位：512B
      uint32_t kStartPosition;  // K方向起始位置，单位：512B
      uint16_t mStep;           // M方向搬运分形数
      uint16_t kStep;           // K方向搬运分形数
      int32_t  srcStride;       // 源端相邻K方向分形间隔，单位：512B
      uint16_t dstStride;       // 目的端相邻K方向分形间隔，单位：512B
      bool     ifTranspose;     // 是否转置每个分形，默认false
      uint8_t  sid;             // 预留，配置为0
  };
  ```
  Ascend 950PR/Ascend 950DT产品中，L0A上的排布格式为Nz，搬运A矩阵时使用`{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0}`，一次完成A矩阵Nz到Nz搬运；搬运B矩阵时使用`{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0}`，一次完成B矩阵Nz到Zn搬运。

  **`AscendC::MmadParams`** — `Mmad`接口使用，描述矩阵乘参数：
  ```cpp
  struct MmadParams {
      uint16_t m;               // 左矩阵Height（M维），[0, 4095]
      uint16_t n;               // 右矩阵Width（N维），[0, 4095]
      uint16_t k;               // 左矩阵Width/右矩阵Height（K维），[0, 4095]
      uint16_t unitFlag;        // Mmad与Fixpipe细粒度并行控制，默认0
      bool     cmatrixSource;   // C矩阵初始值来源，false=CO1，true=C2，默认false
      bool     cmatrixInitVal;  // C矩阵初始值是否为0，默认true
  };
  ```
  例如`{baseM, baseN, baseK, 0, false, true}`，计算baseM×baseN输出块并在K方向累加baseK长度。

  **`AscendC::FixpipeParamsV220`** — `Fixpipe`接口使用，描述L0C到GM的数据搬运和精度转换参数：
  ```cpp
  struct FixpipeParamsV220 {
      int32_t     nSize;        // 源Nz矩阵N方向大小，[1, 4095]
      uint16_t    mSize;        // 源Nz矩阵M方向大小（Nz2ND时[1, 8192]）
      uint16_t    srcStride;    // 源Nz相邻Z排布起始偏移，单位：C0_SIZE，[0, 65535]
      int32_t     dstStride;    // Nz2ND时目的ND矩阵每行元素数，单位：element
      bool        reluEn;       // 是否使能ReLU
      QuantMode_t quantPre;     // 量化模式，F322F16表示float→half
      uint64_t    deqScalar;    // scalar量化参数，单个scale值
      int32_t     ndNum;        // 源Nz矩阵数目，[1, 65535]
      int32_t     srcNdStride;  // 不同Nz矩阵起始地址间隔，单位：16×C0_SIZE，[1, 512]
      int32_t     dstNdStride;  // 目的相邻ND矩阵偏移，单位：element，[1, 65535]
      int32_t     unitFlag;     // Mmad与Fixpipe并行控制
  };
  ```
  例如`{baseN, baseM, baseM, N, false, F322F16, 0, 1, 0, 0, 0}`，将L0C中的baseM×baseN float32结果转为half并写回GM。

  **`AscendC::DataCopyExtParams`** — `DataCopyPad`接口使用，描述GM与UB之间按块搬运的参数：
  ```cpp
  struct DataCopyExtParams {
      uint16_t blockCount;  // 连续传输数据块个数
      uint32_t blockLen;    // 每个数据块长度，单位：Byte
      uint32_t srcStride;   // 源端相邻数据块间隔，单位：Byte
      uint32_t dstStride;   // 目的端相邻数据块间隔，单位：Byte
      uint32_t rsv;         // 预留字段，配置为0
  };
  ```
  例如GM搬运到UB时`{static_cast<uint16_t>(baseM / 2), blockLen, srcStride, 0, 0}`，每个Vector核读取baseM/2行结果。

  **`AscendC::DataCopyPadExtParams<half>`** — `DataCopyPad`接口使用，描述尾块补齐参数：
  ```cpp
  template <typename T>
  struct DataCopyPadExtParams {
      bool isPad;       // 是否开启补齐
      uint8_t leftPad;  // 左侧补齐长度
      uint8_t rightPad; // 右侧补齐长度
      T paddingValue;   // 补齐值
  };
  ```
  例如`{true, 0, 0, 0}`表示开启补齐能力，但左右两侧均不额外补齐。

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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT，Atlas A3 训练系列产品/Atlas A3 推理系列产品，Atlas A2 训练系列产品/Atlas A2 推理系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
