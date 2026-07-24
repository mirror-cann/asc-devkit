# data_copy_ub2l1样例

## 概述

本样例使用Ascend C C API将矩阵数据从UB（Unified Buffer）搬运到L1（L1 Buffer），随后完成L1 Buffer到L0A Buffer和L0B Buffer的数据搬运、Mmad矩阵乘计算，并通过Fixpipe将L0C Buffer中的计算结果搬运到GM（Global Memory）。本样例适用于Ascend 950PR/Ascend 950DT（`dav-3510`），可在NPU运行模式或NPU仿真模式下执行。

## 本样例支持的产品和CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 输出数据与真值数据比对脚本
│   ├── CMakeLists.txt                 // CMake编译文件
│   ├── data_utils.h                   // 数据读写辅助函数
│   ├── data_copy_ub2l1.asc            // C API样例实现及调用代码
│   ├── README.md                      // 中文样例说明文档
│   └── README_en.md                   // 英文样例说明文档
```

## 场景详细说明

通过编译参数`SCENARIO_NUM`选择输入场景。两个场景的矩阵乘规格均为[M, K, N] = [128, 128, 128]，核函数名称为`data_copy_ub2l1`。

**表1：SCENARIO_NUM取值**

| SCENARIO_NUM | 输入格式 | 输入数据类型 | 输出数据类型 | UB到L1处理 |
|---|---|---|---|---|
| 1 | Nz | half | float | 连续搬运 |
| 2 | ND | half | float | 先在UB中将数据重排为Nz格式，再连续搬运 |

`SCENARIO_NUM`由CMake作为编译期宏传入，核函数通过`if constexpr`选择对应场景。切换场景后，需要重新编译。

**场景1：输入格式Nz，输入数据类型half**

- 输入：A [128, 128]，`half`类型，Nz格式；B [128, 128]，`half`类型，Nz格式
- 输出：C [128, 128]，`float`类型，ND格式
- 实现：AIV侧先通过`asc_copy_gm2ub`将A、B从GM搬入UB；`asc_sync_notify`和`asc_sync_wait`建立MTE2到MTE3依赖后，使用`asc_copy_ub2l1`将Nz格式的数据连续搬运到L1

**场景2：输入格式ND，输入数据类型half**

- 输入：A [128, 128]，`half`类型，ND格式；B [128, 128]，`half`类型，ND格式
- 输出：C [128, 128]，`float`类型，ND格式
- 实现：AIV侧先将A、B从GM搬入UB；在MTE2到V同步后，`copy_ub_nd_to_nz`按C0列块调用`asc_copy_ub2ub`，将ND格式的数据重排为Nz格式的数据。A和B使用独立的UB临时区，避免两次重排和后续UB到L1搬运之间发生读写覆盖。V到MTE3同步完成后，使用`asc_copy_ub2l1`将Nz格式的数据连续搬运到L1

### 数据流与同步

1. AIV侧调用`asc_copy_gm2ub`，通过MTE2将数据从GM搬运到UB。
2. 在场景1中，AIV侧通过`asc_sync_notify`和`asc_sync_wait`建立MTE2到MTE3同步后，调用`asc_copy_ub2l1`将Nz格式数据从UB连续搬运到L1。在场景2中，AIV侧先通过`asc_sync_notify`和`asc_sync_wait`建立MTE2到V同步，再调用`copy_ub_nd_to_nz`，由其按C0列块调用`asc_copy_ub2ub`将UB中的ND格式数据重排为Nz格式数据；随后建立V到MTE3同步，并调用`asc_copy_ub2l1`将数据从UB连续搬运到L1。
3. AIV侧调用`asc_sync_block_arrive`通知AIC侧，AIC侧再通过`asc_sync_block_wait`等待L1数据就绪。
4. AIC侧调用`asc_copy_l12l0a`和`asc_copy_l12l0b_transpose`，将L1 Buffer中的Nz格式数据分别搬运到L0A Buffer和L0B Buffer；再通过`asc_sync_notify`和`asc_sync_wait`建立MTE1到M同步，调用`asc_mmad`完成矩阵乘计算。
5. AIC侧通过`asc_sync_notify`和`asc_sync_wait`建立M到FIX同步后，调用`asc_set_l0c2gm_nz2nd`配置Nz格式到ND格式的转换，再通过Fixpipe接口`asc_copy_l0c2gm`将L0C Buffer中的计算结果搬运到GM。

上述过程按需建立流水依赖；核函数结束时调用`asc_sync_pipe(PIPE_ALL)`，确保全部流水完成。

## 编译运行

在样例根目录下执行以下步骤编译并运行样例。

- 配置环境变量

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：**`${install_path}`为CANN软件包安装目录；root用户默认目录为`/usr/local/Ascend`，非root用户默认目录为`${HOME}/Ascend`。

- 运行样例

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据和真值数据
  ./demo                           # 执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # 验证输出结果
  ```

  `SCENARIO_NUM`与`-scenarioNum`必须设置为相同值，取值为1或2。

  使用NPU仿真模式时，在CMake命令中增加`-DCMAKE_ASC_RUN_MODE=sim`：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
  ```

  > **注意：**切换运行模式或场景前，需要清理CMake缓存。可在build目录中执行`rm CMakeCache.txt`后重新执行CMake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号 |

- 执行结果

  精度比对成功时，输出如下：

  ```bash
  test pass!
  ```
