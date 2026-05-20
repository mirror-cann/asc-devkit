# pattern_transformation兼容性样例

## 概述

本样例基于基础的mmad样例，演示了 L1 Buffer -> L0A Buffer 通路的分形转换逻辑，通过编译时宏隔离不同硬件实现。

- 在Atlas A2/A3 系列产品中，L0A Buffer的数据排布为Zz分形，从L1 Buffer搬运到L0A Buffer时需要做Nz2Zz的分形转换。
- 在Ascend 950PR/Ascend 950DT中，L0A Buffer的数据排布变更为Nz分形，与L1 Buffer一致，不再需要做分形转换。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── pattern_transformation
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   └── pattern_transformation.asc         // AscendC算子实现 & 调用样例
```

## 样例规格

<table>
<caption>样例规格表</caption>
<tr><td rowspan="1" align="center">类别</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="3" align="center">样例输入</td></tr>
<tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">pattern_transformation</td></tr>
</table>

## 样例实现

<table>
<tr><th align="left">步骤</th><th align="left">操作</th><th align="left">功能</th><th align="left">Atlas A2/A3 分形变化</th><th align="left">Ascend 950PR/950DT 分形变化</th></tr>
<tr><td align="left">1</td><td align="left">CopyIn</td><td align="left">将矩阵A和B从GM搬运到L1 Buffer</td><td align="left">ND -> Nz</td><td align="left">ND -> Nz</td></tr>
<tr><td align="left">2</td><td align="left">DataLoadA</td><td align="left">将矩阵A从L1 Buffer搬运到L0A Buffer</td><td align="left">Nz -> Zz（LoadData2DParams）</td><td align="left">Nz -> Nz（LoadData2DParamsV2）</td></tr>
<tr><td align="left">3</td><td align="left">DataLoadB</td><td align="left">将矩阵B从L1 Buffer搬运到L0B Buffer</td><td align="left">Nz -> Zn（LoadData2DParams）</td><td align="left">Nz -> Zn（LoadData2DParamsV2）</td></tr>
<tr><td align="left">4</td><td align="left">Compute</td><td align="left">执行矩阵乘法Mmad计算</td><td align="left">不涉及</td><td align="left">不涉及</td></tr>
<tr><td align="left">5</td><td align="left">CopyOut</td><td align="left">将计算结果从L0C Buffer搬运到GM</td><td align="left">Nz -> ND</td><td align="left">Nz -> ND</td></tr>
</table>

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 默认npu模式
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201对应Atlas A2/A3系列，dav-3510对应Ascend 950PR/950DT |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```