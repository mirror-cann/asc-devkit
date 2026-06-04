# Matmul 模板参数MatmulCallbackFunc直调样例
## 概述
Matmul API模板参数MatmulCallbackFunc的自定义使用方式。MatmulCallbackFunc用于配置左矩阵从Global Memory拷贝到A1(L1 Buffer)、右矩阵从Global Memory拷贝到B1(L1 Buffer)、计算结果从CO1（L0C Buffer）拷贝到Global Memory的自定义函数，本样例以Global Memory自定义搬运到A1(L1 Buffer)的回调函数为例，介绍该模板参数如何使用。

## 支持的产品
- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
## 目录结构介绍
```
├── matmul_callback
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   └── matmul_callback.asc     // Ascend C样例实现 & 调用样例
```
## 样例描述
- 样例功能：  
  Matmul样例对输入的A、B矩阵做矩阵乘和加bias偏置。将自定义的左矩阵搬运函数CustomDataCopyInA作为参数传递给Matmul的模板参数MatmulCallbackFunc，实现左矩阵从Global Memory到A1(L1 Buffer)的自定义搬运，本样例以输入A矩阵为例，实现callback回调功能，对于输入B矩阵、输出C矩阵的callback回调功能也可以参考该样例的实现。

- 样例规格：  
  本样例中：M = 2560, N = 128, K = 512。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  </tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_callback_custom</td></tr>
  </table>

- 样例实现： 
  - 计算逻辑：本样例的A矩阵为非连续搬运，每两个基本块需要跳转一次地址（即第一块与第二块连续排布，第二块与第三块间存在地址偏移，第三块与第四块连续排布，后续以此类推）。编写自定义回调函数前，需要确定切分后的SingleM、SingleK、baseM、baseK的大小及base块的分布。本样例中在Tiling侧设置单核计算量SingleShape：SingleM=128、SingleK=512、SingleN=128，然后在调测阶段调用GetBaseM、GetBaseK接口，打印出参数信息baseM=128、baseK=128，由此得知每个单核上有4个base块，用于Kernel侧的地址偏移计算，从而编写回调函数的搬运。变量offsetListGlobal保存单核上A矩阵的第0个base块和第2个base块的首地址，每个单核需要传入2个地址。
  - Kernel关键步骤
    - 自定义左矩阵搬运函数CustomDataCopyInA，通过矩阵起始地址、偏移地址等，实现对左矩阵基本块baseM * baseK从Global Memory到逻辑位置A1(L1 Buffer)的搬运。
    - 将自定义的CustomDataCopyInA传递给模板参数MatmulCallBackFunc，创建Matmul对象。
      ```cpp
      AscendC::Matmul<
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>,
        CFG_NORM, AscendC::MatmulCallBackFunc<nullptr, CustomDataCopyInA, nullptr>> matmulObj;
      ```

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 支持的CANN软件版本

- \>= CANN 9.0.0

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
  mkdir -p build && cd build;    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py    # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```