# GetTPipePtr样例

## 概述

本样例基于GetTPipePtr接口获取全局TPipe指针，并通过该指针进行TPipe相关操作。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── get_tpipe_ptr
│   ├── CMakeLists.txt          // 编译工程文件
│   └── get_tpipe_ptr.asc       // Ascend C样例实现 & 调用样例
```

## 样例描述

- 样例功能

  创建TPipe对象时，对象初始化会设置全局唯一的TPipe指针。本样例调用GetTPipePtr接口获取该指针，核函数无需显式传入TPipe指针即可进行TPipe相关操作。下面将展示调用GetTPipePtr接口和不调用的代码片段示例。

  **调用GetTPipePtr接口**

  ```cpp
  class KernelAdd {
      // 没有TPipe成员变量

      __aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, uint32_t totalLength, uint32_t tileNum)
      {
          // 调用GetTPipePtr获取TPipe指针并使用
          GetTPipePtr()->InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(float));
      }
  };

  __global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AddCustomTilingData tiling)
  {
      // 不用显式传入TPipe指针
      KernelAdd op;
      op.Init(x, y, z, tiling.totalLength, tiling.tileNum);
      op.Process();
  }
  ```

  **不调用GetTPipePtr接口（核函数显式传入TPipe指针）**

  ```cpp
  class KernelAdd {
      AscendC::TPipe* pipe;  // 需要TPipe指针成员变量

      __aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, uint32_t totalLength, uint32_t tileNum, AscendC::TPipe* pipeIn)
      {
          pipe = pipeIn;
          pipe->InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(float));
      }
  };

  __global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AddCustomTilingData tiling)
  {
      // 需要显式传入TPipe指针
      AscendC::TPipe pipe;
      KernelAdd op;
      op.Init(x, y, z, tiling.totalLength, tiling.tileNum, &pipe);
      op.Process();
  }
  ```

- 样例规格

  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">核函数名</td>
      <td colspan="4" align="center">get_tpipe_ptr_custom</td>
    </tr>
  </table>

- 样例实现

  - Kernel实现

    - 调用GetTPipePtr接口获取全局TPipe指针。

    - 调用TPipe::InitBuffer接口，为TQue分配内存空间。

    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）。

    - 调用Add接口，对两个输入tensor执行加法操作。

    - 调用DataCopy基础API，将计算结果从UB（Unified Buffer）搬运至GM（Global Memory）。

  - 调用实现

    使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/quick_start.md#prepare&install)，选择对应配置环境变量的命令。
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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 参数 | 说明 | 可选值 | 默认值 |
  |------|------|---------|--------|
  | CMAKE_ASC_RUN_MODE | 运行模式 | npu, cpu, sim | npu |
  | CMAKE_ASC_ARCHITECTURES | NPU硬件架构 | dav-2201, dav-3510 | dav-2201 |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  [Success] Case accuracy is verification passed.
  ```
