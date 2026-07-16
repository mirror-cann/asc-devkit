# asc_copy_gm2l1样例

## 概述

本样例介绍如何使用 Ascend C API 将输入数据从 GM（Global Memory）搬运到 L1（L1 Buffer），支持多种输入格式（NZ、ND、DN）和向量量化参数搬入等功能。数据搬入后，样例继续完成 L1 到 L0 的搬运、矩阵乘计算和 L0C 到 GM 的结果写回。

本样例仅支持 Ascend 950PR/Ascend 950DT（`dav-3510`），支持 NPU 运行和 NPU 仿真模式，不提供 CPU Debug 模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── asc_copy_gm2l1
│   ├── figures                        // 图示
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── CMakeLists.txt                 // cmake编译文件
│   ├── asc_copy_gm2l1.asc             // C API样例实现及调用代码
│   ├── README.md                       // 样例说明文档
│   └── README_en.md                    // 英文样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的输入场景，`SCENARIO_NUM` 不同取值对应的含义如下表所示。所有场景基于相同的矩阵乘规格：[M, K, N] = [128, 128, 256]，核函数名为 `asc_copy_gm2l1`。

<table>
<caption style="font-weight: normal;">
         <span style="font-weight: bold; font-size: 1.2em;">📌 表1：SCENARIO_NUM不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">SCENARIO_NUM</td><td align="center">输入格式</td><td align="center">输入数据类型</td><td align="center">输出数据类型</td><td align="center">是否使能Vector量化</td></tr>
<tr><td align="center">1</td><td align="center">NZ</td><td align="center">half</td><td align="center">float</td><td align="center">否</td></tr>
<tr><td align="center">2</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">否</td></tr>
<tr><td align="center">3</td><td align="center">DN</td><td align="center">half</td><td align="center">float</td><td align="center">否</td></tr>
<tr><td align="center">4</td><td align="center">ND</td><td align="center">half</td><td align="center">int8_t</td><td align="center">是</td></tr>
</table>

`SCENARIO_NUM` 由 CMake 作为编译期宏传入；Kernel 使用 `if constexpr` 选择对应场景。切换场景后需要重新执行 CMake 和编译。

设备侧搬运和计算均使用异步 C API，通过 `asc_sync_notify` 和 `asc_sync_wait` 建立 `MTE2→MTE1→M→FIX` 的必要依赖；场景 4 还使用 `MTE2→FIX` 保证量化参数就绪，并在量化参数从 L1 搬运到 Fixpipe Buffer 后通过 `asc_sync_pipe(PIPE_FIX)` 等待该搬运完成，再执行最终 Fixpipe 搬出；不使用 `PIPE_ALL` 同步。

**场景1：输入格式NZ，输入数据类型half**

- 输入：A [128, 128] half类型，NZ格式；B [128, 256] half类型，NZ格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `asc_copy_gm2l1` 将 NZ 格式的输入数据从 GM 搬运到 L1
- 说明：输入数据为 NZ 格式，直接搬运到 L1，无需格式转换
<p align="center">
  <img src="figures/asc_copy_gm2l1_nz2nz.png" width="800">
</p>

**场景2：输入格式ND，输入数据类型half**

- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `asc_set_gm2l1_nz_para` 配置 ND2NZ 参数，再使用 `asc_copy_gm2l1_nd2nz` 将 ND 格式的输入数据从 GM 搬运到 L1，并转换为 NZ 格式
- 说明：输入数据为 ND 格式，在搬运过程中自动转换为 NZ 格式
<p align="center">
  <img src="figures/asc_copy_gm2l1_nd2nz.png" width="800">
</p>

**场景3：输入格式DN，输入数据类型half（仅Ascend 950PR/Ascend 950DT支持）**

- 输入：A [128, 128] half类型，DN格式；B [128, 256] half类型，DN格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `asc_set_gm2l1_nz_para` 配置 DN2NZ 参数，再使用 `asc_copy_gm2l1_dn2nz` 将 DN 格式的输入数据从 GM 搬运到 L1，并转换为 NZ 格式
- 说明：输入数据为 DN 格式，在搬运过程中自动转换为 NZ 格式
<p align="center">
  <img src="figures/asc_copy_gm2l1_dn2nz.png" width="800">
</p>

**场景4：输入格式ND，输入数据类型half，使能Vector量化**

- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式；量化参数 [256] uint64_t类型
- 输出：C [128, 256] int8_t类型，ND格式
- 实现：使用 `asc_set_gm2l1_nz_para` 配置 ND2NZ 参数，再使用 `asc_copy_gm2l1_nd2nz` 将输入数据从 GM 搬运到 L1，执行矩阵乘计算；使用 `asc_copy_gm2l1` 和 `asc_copy_l12fb` 将量化参数从 GM 经 L1 搬运到 Fixpipe Buffer；调用 `asc_set_l0c2gm_config` 配置参数地址，并通过 `asc_copy_l0c2gm` 的 `VQF322B8_PRE` 模式进行 Vector 量化输出
- 说明：矩阵乘计算结果为 float 类型，随后通过 Vector 量化转换为 int8_t 类型；C 矩阵的每一列使用一个量化参数。`asc_set_l0c2gm_config` 通过 `quant_pre` 配置 Fixpipe 矢量量化参数地址，该参数以 128B 为地址单位，因此量化参数文件需按 128B 向上对齐，不足部分补 0。本样例的 `[256] uint64_t` 参数共 2048B，已满足该要求。量化参数从 L1 搬运至 Fixpipe Buffer 时，`asc_copy_l12fb` 的 `len_burst` 以 64B 为单位，因此该缓冲区使用 32 个 burst。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上 CANN 开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为 CANN 包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  将 `SCENARIO_NUM` 和 `-scenarioNum` 同时设置为 1 至 4，即可运行对应场景。

  使用 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换运行模式前需清理 CMake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新执行 CMake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | 场景编号 |

- 执行结果

  场景 1-3 的浮点结果满足精度要求，或场景 4 的 int8 结果与真值完全一致时，执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
