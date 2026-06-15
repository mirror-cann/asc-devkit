# 基于Tensor API实现的Batch Matmul带Bias样例

## 概述

本样例基于Tensor API编程方式实现带Bias的Batch Matmul计算，展示使用`MakeTensor`构造GM、L1 Buffer、L0A Buffer、L0B Buffer、L0C Buffer和BiasTable Buffer上的Tensor对象，通过`Slice`按batch维度切分输入输出，并通过Tensor API `Copy`和`Mmad`完成分批矩阵乘加计算的方法。

本样例输入矩阵A、矩阵B和Bias的数据类型均为`half`，输出矩阵C的数据类型为`half`。矩阵A和矩阵B均不转置，计算逻辑为：

```text
C[b] = A[b] * B[b] + Bias[b], b = 0, 1, ..., B - 1
```

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT</cann-filter> | CANN 9.1.0以上 |

> **说明：** 该样例依赖尚未正式发布的CANN特性，请使用最新的CANN master包。

## 目录结构介绍

```text
├── batch_matmul_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本文件。
│   │   └── verify_result.py            // 真值对比文件。
│   ├── CMakeLists.txt                  // 编译工程文件。
│   ├── data_utils.h                    // 数据读取写出函数。
│   ├── batch_matmul_tensor_api.asc     // Ascend C样例实现与调用样例。
│   └── README.md                       // 样例说明文档。
```

## 样例描述

- 样例功能：

  本样例实现了带Bias的Batch Matmul功能。

  1. Batch Matmul功能

      Batch Matmul是普通矩阵乘法在batch维度上的扩展。输入矩阵A的shape为`[B,M,K]`，输入矩阵B的shape为`[B,K,N]`，输出矩阵C的shape为`[B,M,N]`。每个batch内独立完成一次矩阵乘，不同batch之间不会互相计算。

  2. Tensor API Tensor描述

      样例通过`MakeTensor`和`MakeFrameLayout`分别描述GM、L1 Buffer、L0A Buffer、L0B Buffer、L0C Buffer和BiasTable Buffer上的Tensor对象。GM上的A、B、C和Bias使用`NDExtLayoutPtn`描述，L1 Buffer上的A、B使用`NZLayoutPtn`描述，L0A Buffer使用`NZLayoutPtn`描述，L0B Buffer使用`ZNLayoutPtn`描述，L0C Buffer使用`NZLayoutPtn`描述。

  3. batch维度切分

      样例通过`L1BatchSize`和`L0BatchSize`两级编译期模板参数控制每次进入L1 Buffer以及L0A Buffer、L0B Buffer、L0C Buffer的batch数量。外层循环按`L1BatchSize`从GM侧切分batch数据，内层循环按`L0BatchSize`继续切分到L0A Buffer、L0B Buffer、L0C Buffer侧，避免一次缓存全部batch数据造成片上存储空间超限。

  4. Bias功能

      每个batch拥有独立Bias，Bias的shape为`[B,1,N]`。样例先将Bias构造为Tensor对象，再通过Tensor API `CopyL12BT`搬入BiasTable Buffer，并在调用`Mmad`时作为Bias操作数参与矩阵乘加计算。

  5. Tensor API接口使用

      样例使用`MakeCopy`构造Tensor API Copy对象，使用`MakeMmad`构造Tensor API Mmad对象。通过`CopyGM2L1`、`CopyL12L0A`、`CopyL12L0B`、`CopyL12BT`、`CopyL0C2GM`和`Mmad`完成Batch Matmul带Bias的完整计算链路。

- 样例规格：

  在核函数直调样例中，样例默认支持的shape为：`B=128`、`M=32`、`K=32`、`N=32`。

  <table border="2" align="center">
  <caption>表1：样例规格表</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Batch Matmul with Bias</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x1_gm（矩阵A）</td><td align="center">[B, M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">x2_gm（矩阵B）</td><td align="center">[B, K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[B, 1, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output（矩阵C）</td><td align="center">[B, M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">batch_matmul_tensor_api</td></tr>
  </table>

  编译期模板参数默认值：

  | 参数 | 默认值 | 说明 |
  | :--- | :--- | :--- |
  | B | 128 | batch数量 |
  | M | 32 | 矩阵A的行数 |
  | K | 32 | 矩阵A的列数、矩阵B的行数 |
  | N | 32 | 矩阵B的列数 |
  | L1BatchSize | 32 | 每次搬入L1 Buffer参与处理的batch数量 |
  | L0BatchSize | 4 | 每次搬入L0A Buffer、L0B Buffer、L0C Buffer参与计算的batch数量 |

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
  mkdir -p build && cd build;                                                     # 创建并进入build目录。
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                            # 编译工程，默认NPU模式。
  python3 ../scripts/gen_data.py                                                  # 生成测试输入数据。
  ./demo                                                                          # 执行编译生成的可执行程序，执行样例。
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin         # 验证输出结果是否正确，确认算法逻辑正确。
  ```

    使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
    
  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # NPU仿真模式。
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` |`dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
