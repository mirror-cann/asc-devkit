# Matmul和LeakyRelu融合样例

## 概述

本样例采用高阶API实现Matmul与LeakyRelu激活函数融合计算，实现矩阵运算单元和向量运算单元的融合计算。

## 支持的产品

- Ascend 950PR/Ascend 950DT
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
- Atlas A2 训练系列产品/Atlas A2 推理系列产品

## 目录结构介绍

```
├── matmul_leakyrelu_high_level_api
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_leakyrelu.asc    // Ascend C样例实现 & 调用样例
│   └── scripts
│       ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│       └── verify_result.py    // 真值对比文件
```

## 样例描述

- 样例功能：
  本样例使用Ascend C Matmul高阶API实现矩阵乘、Bias加法和LeakyRelu激活函数的融合计算。Matmul高阶API封装了矩阵计算过程中的数据搬运、Cube计算调度、基础流水同步等细节，开发者主要需要完成矩阵规格配置、tiling生成、输入Tensor设置、Vector侧融合计算和结果写回。

  MatmulLeakyRelu的计算公式如下：
  ```
  C = A * B + Bias
  C = C > 0 ? C : C * 0.001
  ```
  其中，A矩阵形状为`[M, K]`，B矩阵形状为`[K, N]`，Bias形状为`[N]`，输出C矩阵形状为`[M, N]`。矩阵乘结果会先加上Bias，再对每个输出元素执行LeakyRelu激活。

- 样例规格：
  本样例参数`M = 512, N = 16, K = 64`。代码中A矩阵K轴使用`Ka`表示，B矩阵K轴使用`Kb`表示，本样例`Ka = Kb = K = 64`。输入A、B为`half`类型，Bias和输出C为`float`类型，格式均为`ND`。输入输出规格如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">MatmulLeakyRelu</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Bias</td><td align="center">[N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_leakyrelu_custom</td></tr>
  </table>

  样例采用`__mix__(1, 2)`声明核函数，表示核函数按Cube:Vector为1:2的比例执行。Host侧以`numBlocks = 1`启动核函数，用于启动1个MIX组合，对应1个AIC和2个AIV，与`SetDim(2)`生成的2个AIV逻辑核切分结果对应。当前tiling只按M轴分核，N轴和K轴不分核，每个AIV逻辑核负责输出矩阵C中`singleCoreM = 256`行、N方向完整16列的数据。

- 样例实现：
  - Tiling生成流程
    - `GenerateTiling`中固定设置矩阵规格：`M = 512`、`N = 16`、`K = 64`。
    - 创建`matmul_tiling::MultiCoreMatmulTiling`对象`tilingApi`，用于生成多核Matmul所需的tiling参数。
    - `SetDim(2)`表示tiling侧配置2个AIV逻辑核参与Matmul计算发起。分离架构MIX场景下，Matmul高阶API由AIV侧发起，调用`Iterate`时由AIV通知AIC执行矩阵计算。
    - Host侧kernel launch的`numBlocks`表示Kernel直调时启动的MIX组合数量。本样例使用`__mix__(1, 2)`，`numBlocks = 1`对应1个AIC和2个AIV，`SetDim(2)`生成2个AIV逻辑核的切分结果。
    - `SetAType`、`SetBType`、`SetCType`、`SetBiasType`分别设置A、B、C、Bias的数据来源位置、数据格式和数据类型。本样例A/B/Bias位于GM，A/B为`half`，C/Bias为`float`；C类型设置为`VECIN`，表示Matmul中间结果输出到Vector侧可读取的位置，便于后续执行LeakyRelu激活。
    - `SetOrgShape(M, N, K)`设置原始完整矩阵形状，`SetShape(M, N, K)`设置本次实际参与Matmul计算的`m/n/k`。本样例计算完整矩阵乘，因此两者都设置为`M/N/K`。
    - `EnableBias(true)`表示Matmul计算带Bias输入。
    - `SetFixSplit(128, 16, 64)`固定Matmul基础分块大小。本样例`singleCoreM = 256`、`baseM = 128`，因此Kernel侧每个AIV逻辑核需要2次`Iterate`处理自身负责的`256 * 16`输出区域。
    - `SetBufferSpace(-1, -1, ubSize - usedUb)`设置Matmul可使用的L1/L0C/UB空间大小。L1和L0C传入`-1`表示使用当前AI处理器对应buffer的默认大小；`usedUb = baseM * baseN * sizeof(float)`用于扣除LeakyRelu需要使用的一块UB缓存。
    - `GetTiling(tilingData)`生成最终tiling结果。如果返回`-1`表示tiling计算失败，tiling结果不可继续使用。

  - Kernel侧整体思路
    - `matmul_leakyrelu_custom`是一个`__global__ __mix__(1, 2)`核函数，运行时同时包含Cube侧和Vector侧计算逻辑。
    - Kernel入参中的A、B、Bias、C均使用`__gm__ uint8_t*`表示GM上的字节地址，Kernel内部固定A/B为`half`、C/Bias为`float`，并转换为对应的`GlobalTensor`使用。本样例调用`matmul_leakyrelu_custom`时不传模板参数。
    - workspace参数使用`__kfc_workspace__`修饰，表示Host侧申请并传入的系统workspace。Kernel侧注册Matmul对象时，通过`GetSysWorkSpacePtr()`将该workspace传递给`REGIST_MATMUL_OBJ`。
    - 创建`GlobalTensor`对象`aGlobal`、`bGlobal`、`cGlobal`、`biasGlobal`，分别表示GM中的A、B、C、Bias数据。`GlobalTensor`只描述GM上的地址和元素个数，真正的数据搬运、L1/L0切分由Matmul高阶API结合tiling完成。
    - 当前样例只按M轴分核。`SetDim(2)`生成2个AIV逻辑核的Matmul分核参数；Host侧`numBlocks = 1`启动1个MIX组合，对应2个AIV侧逻辑任务。
    - Kernel侧通过`GetBlockIdx()`获取当前AIV侧逻辑任务编号，并计算当前AIV逻辑核在M方向的分片编号。Matmul高阶API内部负责处理AIV发起、AIC执行以及AIV获取结果的协同过程。
    - A矩阵和C矩阵需要按照AIV逻辑核编号在M方向偏移，B矩阵和Bias在各个逻辑核上复用起始地址。
    - 创建`TQue<AscendC::TPosition::VECIN, 1>`管理Matmul输出到Vector侧的中间结果缓存，创建`TQue<AscendC::TPosition::VECOUT, 1>`管理Vector侧LeakyRelu输出缓存，并通过`pipe.InitBuffer`分别为单个`baseM * baseN`输出分块申请片上缓存。
    - 创建Matmul高阶API对象`matmulObj`。`MatmulType`模板参数描述A/B/C/Bias在Kernel侧的位置、格式和数据类型，其中C使用`VECIN`，表示Matmul结果输出到Vector侧可读取的位置。
    - 通过`REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling)`注册Matmul对象。注册时会把`TPipe`、系统workspace、Matmul对象和对应`TCubeTiling`交给Matmul高阶API内部使用。
    - 调用`SetTensorA`、`SetTensorB`、`SetBias`绑定当前逻辑块需要读取的A、B、Bias输入。

  - Matmul与LeakyRelu融合流程
    - `while (matmulObj.template Iterate<true>())`用于执行当前逻辑核负责的Matmul输出分块。本样例保持`baseM = 128`不变，每个AIV逻辑核需要2次`Iterate<true>()`完成自身`256 * 16`输出区域。
    - `mmOutQueue.AllocTensor<float>()`从Vector输入队列申请LocalTensor缓存，用于承接Matmul输出分块。
    - `matmulObj.template GetTensorC<true>(mmOutLocal, false, true)`从Matmul对象获取当前分块的C中间结果。由于C类型设置为`VECIN`，该中间结果可以作为Vector侧LeakyRelu输入。
    - `AscendC::LeakyRelu(reluOutLocal, mmOutLocal, static_cast<float>(0.001), tiling.baseM * tiling.baseN)`对当前分块执行逐元素LeakyRelu激活，并将结果写入`reluOutLocal`。
    - `mmOutQueue.EnQue`和`mmOutQueue.DeQue`用于建立Matmul结果写入和后续LeakyRelu读取之间的数据依赖；`reluOutQueue.EnQue`和`reluOutQueue.DeQue`用于建立Vector计算和搬出之间的数据依赖。
    - `AscendC::DataCopy`将激活后的`baseM * baseN`分块搬运到GM输出矩阵C的对应位置。
    - `matmulObj.End()`通知Matmul对象当前核计算结束，释放Matmul高阶API内部状态。

  - 分核和地址偏移说明
    - 本样例`SetDim(2)`按2个AIV逻辑核生成tiling，tiling结果按M轴切分：`M = 512`、`singleCoreM = 256`，因此输出C矩阵被分成2个M轴分片。
    - Host侧`numBlocks = 1`启动1个MIX组合，对应2个AIV侧逻辑任务，与`SetDim(2)`配置的AIV逻辑核数对应。
    - 第0个AIV逻辑核处理C矩阵第0到255行，第1个AIV逻辑核处理第256到511行。
    - A矩阵形状为`[512, 64]`，每一行长度为`tiling.Ka = 64`，所以A矩阵偏移为`blockIdx * tiling.singleCoreM * tiling.Ka`。其中`blockIdx`表示通过`GetBlockIdx()`获取的AIV侧逻辑任务编号。
    - B矩阵形状为`[64, 16]`，对应代码中的`[Kb, N]`。不同逻辑块负责C矩阵的不同行，但每个输出元素都需要完整K轴累加，因此各个逻辑块都从`bGlobal[0]`读取完整B矩阵。
    - Bias形状为`[16]`，对应N方向的16列，各个逻辑块复用同一份Bias数据。
    - C矩阵形状为`[512, 16]`，每一行长度为`tiling.N = 16`，所以C矩阵逻辑块起始偏移为`blockIdx * tiling.singleCoreM * tiling.N`。
    - 每个逻辑块内部包含`singleCoreM / baseM = 2`个`baseM * baseN`输出分块，激活后的结果根据当前`Iterate`轮次写回当前C矩阵分片中的对应M轴位置。
    - 由于M轴正好被2个逻辑块均分，且N轴等于`baseN`，代码中没有尾块处理逻辑。

  - 调用实现
    使用内核调用符`<<<>>>`调用核函数。调用时`numBlocks`固定为1，表示启动1个MIX组合；运行时参数依次传入Device侧A矩阵地址、B矩阵地址、Bias地址、C矩阵地址、workspace地址和Host侧生成的tiling数据。

- `__mix__(1, 2)`场景下SetDim和kernel启动核数配置说明：

  MIX分离模式下Matmul API是从AIV侧发起的，AIV调用`Iterate`时通知AIC做矩阵计算，AIC完成后再通知AIV。因此tiling侧按AIV视角切分任务。

  本样例中`SetDim(2)`表示tiling侧按照2个AIV逻辑核生成Matmul分核参数；`__mix__(1, 2)`表示每个AI Core组合包含1个AIC和2个AIV；
  
  本样例为Kernel直调样例，Host侧kernel launch的`numBlocks = 1`表示启动1个MIX组合，对应2个AIV侧逻辑任务，正好与`SetDim(2)`配置的2个AIV逻辑核对应。

- 接口参数说明：

  以下结构体以花括号`{}`方式传参，各字段含义如下：

  **`AscendC::DataCopyParams`** — `DataCopy`接口使用，描述Local Memory到Global Memory的数据搬运参数：
  ```cpp
  struct DataCopyParams {
      uint16_t blockCount;  // 待搬运的连续传输数据块个数，[1, 4095]
      uint16_t blockLen;    // 每个连续传输数据块长度，单位为DataBlock（32字节），[1, 65535]
      uint16_t srcGap;      // 源操作数相邻连续数据块的间隔，单位为DataBlock（32字节）
      uint16_t dstGap;      // 目的操作数相邻连续数据块的间隔，单位为DataBlock（32字节）
  };
  ```
  本样例搬出C矩阵分块时使用`{tiling.baseM, tiling.baseN * sizeof(float) / AscendC::DEFAULT_C0_SIZE, 0, (tiling.N - tiling.baseN) * sizeof(float) / AscendC::DEFAULT_C0_SIZE}`。其中，`blockCount`表示按`baseM`行搬运，`blockLen`表示每行搬运`baseN`个C矩阵元素对应的数据量；本样例`N = baseN`，因此目的端相邻行之间无额外间隔。

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

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

| 选项 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT，Atlas A3 训练系列产品/Atlas A3 推理系列产品，Atlas A2 训练系列产品/Atlas A2 推理系列产品 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
