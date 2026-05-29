# GELU样例

## 概述

GELU 是神经网络中常用的激活函数。本样例演示如何使用 Ascend C 实现 GELU 近似计算，输入和输出的 shape 均为 `[256, 32]`，数据类型为 `float`，格式为 `ND`。

本样例固定使用 2 个 Vector 核，仅按 M 方向分核。每个核处理 `128 x 32` 个元素。

样例实现采用 RegBase API 和 VF 调用方式，在寄存器中完成 GELU 计算。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

``` 
├── gelu
│   ├── scripts
│   │   ├── gen_data.py     // 输入数据和真值数据生成脚本
│   │   └── verify_result.py // 输出结果和真值数据校验脚本
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── data_utils.h        // 数据读入写出函数
│   └── gelu.asc            // Ascend C样例实现 & 调用样例
```

## 样例描述

**样例功能**：

Gelu 近似公式计算为：

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right)
$$

对公式进行简化，采用如下方式展开为具体的向量运算步骤：

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$


**样例规格**：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">GELU</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[256, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[256, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gelu_custom</td></tr>
  </table>


**样例实现**：
- 分核逻辑：
  - 总核数固定为 2 个 Vector 核
  - 仅按 M 方向分核，`totalM = 256`，`singleCoreM = 128`
  - 每个核负责 `128 x 32` 大小的数据块，不做 N 方向切分
  - 这样划分后，前 128 行由第 1 个核处理，后 128 行由第 2 个核处理

- Kernel 实现：<br>
  本样例使用 RegBase API 和 `asc_vf_call` 调用 VF 函数，在寄存器中完成 GELU 计算。
  计算过程可概括为三层：
  - GM：全局内存，用于存放输入和输出数据
  - UB：片上缓冲区，用于存放当前核需要处理的数据
  - 寄存器：用于执行 GELU 的逐步计算

    - GELU 计算步骤如下：
      1. 计算 `x^2`
      2. 计算 `x^3`
      3. 计算 `-0.071405 * x^3`
      4. 计算 `-1.595769 * x`
      5. 将两项相加，得到指数项
      6. 计算指数函数 `exp(...)`
      7. 将结果加 1
      8. 用 `x` 除以上述结果，得到最终输出

    当前样例的单核工作量为 `4096` 个元素，核内不再继续切块，一次完成单核数据搬运、计算和写回。

- 调用实现
  使用内核调用符 `<<<>>>` 调用核函数，启动 2 个 Vector 核执行计算。每个核仅处理自己负责的数据范围，不发生数据重叠。

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
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  ```

  新版本 CANN 包中，dav-3510 架构的 NPU 仿真临时依赖 `tools/simulator/dav_3510/camodel` 目录。本样例的 CMake 工程会在仿真模式下自动添加该目录的链接搜索路径和运行时库搜索路径，后续包侧修复默认路径后可删除这段兼容说明。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：Ascend 950PR/Ascend 950DT |


- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
