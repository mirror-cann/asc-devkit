# SIMT样例开发任务指导

本文档面向参考`demo/simt/basic_scatter`进行SIMT样例开发与性能调优的任务。开始前请先确认当前环境已经安装CANN开发套件，并能正常编译、运行`basic_scatter`样例。

开始本文档中的开发任务前，需要先按照`demo/simt/SIMT样例运行操作指导.md`完成已有样例的基本验证，确保初始代码、CANN环境、编译工具链和设备访问均正常。至少需要完成：

- `demo/simt/basic_scatter`样例编译运行通过。
- `demo/simt/matrix_transpose`的`SCENARIO_NUM=0`到`3`四个case均编译运行通过。
- 运行输出中包含`[Success] Case accuracy is verification passed.`。

## 任务概览

本次包含三个开发任务：

- 任务一：参考`basic_scatter`实现一维Gather算子，理解“按索引离散读取、连续写输出”的SIMT实现方式。
- 任务二：参考`basic_scatter`将SIMT编程样例改造成SIMD&SIMT混合编程样例，理解SIMT编程和SIMD&SIMT混合编程的差异。
- 任务三：改写Scatter算子为固定shape版本，只支持`input_length = 2048`、`output_length = 4096`，并通过调整`blockDim`、`gridDim`观察性能变化。

每个用户都需要在`/home/wly/asc-devkit-exp-kadc-run/experiment/simt_experiment/`下新建自己的样例路径，不要直接修改公共的`demo/simt/basic_scatter`目录，也不要复用其他用户已经创建的样例路径。建议使用“样例名_姓名拼音”的方式命名各自任务代码路径，例如：

```text
experiment/simt_experiment/basic_gather_zhangsan
experiment/simt_experiment/mixed_scatter_zhangsan
experiment/simt_experiment/fixed_shape_scatter_zhangsan
```

后续命令中的`<姓名拼音>`请替换为自己的姓名拼音或团队约定的唯一标识。


如果CANN安装在非默认路径，请将`source /usr/local/Ascend/cann/set_env.sh`替换为实际安装路径下的`set_env.sh`。

## 任务一：参考basic_scatter实现gather算子

### 任务目标

参考`basic_scatter`样例，新建一个以`basic_gather_<姓名拼音>`命名的Gather样例，实现一维Gather算子，并完成编译、运行和结果校验。

Gather算子的核心含义是“按索引离散读取”。给定输入数组`input`和索引数组`index`，每个线程根据`index[i]`从`input`中读取一个元素，并写入连续的输出位置：

```cpp
output[i] = input[index[i]]
```

它和Scatter的访存方向相反：

- Scatter：连续读取`input[i]`，离散写入`output[index[i]]`。
- Gather：离散读取`input[index[i]]`，连续写入`output[i]`。

Gather适合用于查表、稀疏数据读取、索引选择、Embedding类访问等不规则读场景。SIMT线程可以让每个线程独立计算自己的读取地址，因此能自然表达这类离散访存逻辑。

### 建议目录

```text
experiment/simt_experiment/basic_gather_<姓名拼音>
|-- CMakeLists.txt
|-- gather.asc
`-- README.md
```

可以先复制`experiment/simt_experiment/experiment_gather`作为起点，再按Gather语义改写。


### 核函数改写要点


实现Gather核函数逻辑：

```cpp
if (gather_index < input_length) {
    // TODO 
}
```
提示： output[idx] = input[gather_index];

### 验收标准

编译运行命令：

```bash
cd experiment/simt_experiment/basic_gather_<姓名拼音>
source /usr/local/Ascend/cann/set_env.sh
mkdir -p build
cd build
cmake ..
make -j
./gather
```

验收要求：

- 程序能够编译通过。
- 程序运行不报ACL、Kernel launch或内存访问错误。
- 输出包含`[Success] Case accuracy is verification passed.`。
- `README.md`说明Gather功能、输入输出规格、编译运行方式和预期输出。

## 任务二：将basic_scatter demo改造成SIMD&SIMT混合编程

### 任务目标

参考`basic_scatter`样例，将样例改造成SIMD&SIMT混合编程。该任务的重点不是改变Scatter算法，而是理解SIMT编程和SIMD&SIMT混合编程的差异。

`experiment/simt_experiment/scatter_2_fusion_experiment`已经修改了大部分内容，可复制这个样例模板，再在新目录中进行SIMD&SIMT混合编程改造，避免直接修改原始样例。

```bash
cp -r experiment/simt_experiment/scatter_2_fusion_experiment experiment/simt_experiment/fusion_scatter_<姓名拼音>
cd experiment/simt_experiment/fusion_scatter_<姓名拼音>
```

### 任务修改点

- 修改`asc_vf_call`调用SIMT_VF函数的第一个参数(配置启动先线程数)：
```
{
    asc_vf_call<scatter_simt_vf>(//TODO, output, input, index, input_length, output_length);
}
```
提示： 这里的入参配置为dim3(MAX_THREAD_COUNT)

- 修改`<<<>>>`调用入参，配置启动的核数
```
scatter_custom<<< // TODO >>>(output_device, input_device, index_device, input_length, output_length);
```
提示： 混合编程下，<<<>>>与SIMD编程一致， 入参分别为 numBlocks, dyn_ubuf_size, stream

### 更多信息

当前`basic_scatter`使用的是SIMT编程：

```cpp
__global__ __launch_bounds__(MAX_THREAD_COUNT) void scatter_custom(...)

scatter_custom<<<numBlocks, thread_num_per_block, dyn_ubuf_size, stream>>>(...);
```

SIMD&SIMT混合编程中，SIMT逻辑通常拆为两层：

- `__simt_vf__`函数：承载SIMT线程级计算逻辑。
- `__global__ __vector__` SIMD&SIMT混合编程入口函数：作为Device侧入口，通过`asc_vf_call`调用SIMT VF。

### 整体改造思路

1. 保留Host侧ACL流程、输入输出数据构造和结果校验。
2. 将原`scatter_custom`中的线程级计算逻辑移动到SIMT VF函数中。
3. 为Global Memory指针参数增加`__gm__`修饰。
4. 新增SIMD&SIMT混合编程入口核函数，在入口核函数中通过`asc_vf_call`调用SIMT VF。
5. 调整Host侧Kernel launch参数。SIMT编程使用4段启动参数；SIMD&SIMT混合编程入口使用3段启动参数：

```cpp
// SIMT编程入口
scatter_custom<<<numBlocks, thread_num_per_block, dyn_ubuf_size, stream>>>(...);

// SIMD&SIMT混合编程入口
scatter_custom<<<numBlocks, dyn_ubuf_size, stream>>>(...);
```

6. 调整CMake编译选项。SIMT编程样例中使用`--enable-simt`；改为SIMD&SIMT混合编程后，通常需要从ASC编译选项中移除`--enable-simt`。

### 代码骨架参考

以下骨架用于说明改造方向，实际参数列表以样例代码为准。

```cpp

template <typename type_data, typename type_idx>
__simt_vf__ __launch_bounds__(MAX_THREAD_COUNT) inline void simt_scatter_custom_kernel(
    __gm__ type_data* output,
    const __gm__ type_data* input,
    const __gm__ type_idx* index,
    uint32_t input_length,
    uint32_t output_length)
{
    uint32_t begin = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;

    for (uint32_t i = begin; i < input_length; i += stride) {
        //TODO
    }
}

template <typename type_data, typename type_idx>
__global__ __vector__ void scatter_custom(
    __gm__ type_data* output,
    const __gm__ type_data* input,
    const __gm__ type_idx* index,
    uint32_t input_length,
    uint32_t output_length)
{
    asc_vf_call<simt_scatter_custom_kernel<type_data, type_idx>>(dim3(MAX_THREAD_COUNT),
        output, input, index, input_length, output_length);
}
```

### 调试检查点

建议每完成一个小步骤就编译一次：

1. 只增加头文件和空的SIMD&SIMT混合编程入口，确认编译器能识别相关关键字。
2. 移动原SIMT计算逻辑到`__simt_vf__`函数，确认编译通过。
3. 改Host侧Kernel launch参数，确认程序能够启动。
4. 运行默认用例，确认结果校验通过。
5. 如果出现编译错误，优先检查`__gm__`修饰符、`asc_vf_call`模板参数、启动参数个数和CMake编译选项。

### 验收标准

- 编译通过。
- 运行结果和原`basic_scatter`一致。
- 输出包含`[Success] Case accuracy is verification passed.`。
- 文档说明SIMT编程和SIMD&SIMT混合编程入口的差异。

## 任务三：固定shape scatter性能调优

### 任务目标

改写当前Scatter算子，只支持固定shape，并围绕`blockDim`、`gridDim`两个参数进行性能调试。固定shape定义如下：

```text
input_length = 2048
output_length = 4096
```

也就是输入元素数为2048，输出元素数为4096。样例不再需要支持命令行传入任意shape。

### 功能改写要求

建议先复制`basic_scatter`作为起点，再在新目录中进行固定shape和性能调优改写，避免直接修改原始样例。

```bash
mkdir -p experiment/simt_experiment
cp -r demo/simt/basic_scatter experiment/simt_experiment/fixed_shape_scatter_<姓名拼音>
cd experiment/simt_experiment/fixed_shape_scatter_<姓名拼音>
```

改写要求：

- 将`DEFAULT_INPUT_LENGTH`固定为`2048`。
- 将`DEFAULT_OUTPUT_LENGTH`固定为`4096`。
- 删除或忽略`main`函数中的命令行shape解析逻辑。
- Host侧始终构造`input_length = 2048`、`output_length = 4096`的测试数据。
- 保留`index`无重复生成逻辑，避免多个线程写同一输出位置导致校验不稳定。
- 保留CPU Golden校验。
- 输出日志打印固定shape、`gridDim`和`blockDim`配置。

### 性能调试参数

建议将待调参数集中为常量或CMake宏，便于多次编译对比：

```cpp
constexpr uint32_t THREAD_NUM_PER_BLOCK = 256;
constexpr uint32_t GRID_DIM = 8;
```

Kernel启动：

```cpp
scatter_custom<<<GRID_DIM, THREAD_NUM_PER_BLOCK, dyn_ubuf_size, stream>>>(...);
```

建议先测试以下组合：

| 轮次 | `blockDim` | `gridDim` | 说明 |
|---|---:|---:|---|
| 1 | 128 | 16 | 较小线程块，观察基础并行性能 |
| 2 | 256 | 8 | 常用起点 |
| 3 | 512 | 4 | 更大线程块，检查吞吐变化 |
| 4 | 1024 | 2 | 线程块更大，观察是否收益下降 |
| 5 | 2048 | 1 | 单Block覆盖全部输入元素 |

注意：SIMT线程总数为`gridDim * blockDim`。对于固定输入长度2048，线程总数等于或略大于2048即可覆盖所有元素；如果使用grid-stride循环，线程总数小于2048也能覆盖，但单线程会处理多个元素。

### 性能记录表

每次调整参数后，记录编译、运行和msprof结果。性能查看主要通过msprof导出的`task_duration`数据进行对比，建议使用如下表格：

| 轮次 | `blockDim` | `gridDim` | `task_duration` | 校验结果 | 备注 |
|---|---:|---:|---:|---|---|
| 1 | 128 | 16 | 待填写 | pass/fail |  |
| 2 | 256 | 8 | 待填写 | pass/fail |  |
| 3 | 512 | 4 | 待填写 | pass/fail |  |
| 4 | 1024 | 2 | 待填写 | pass/fail |  |
| 5 | 2048 | 1 | 待填写 | pass/fail |  |

性能比较时先保证所有轮次校验通过，再比较`task_duration`。若校验失败，该轮性能数据不作为有效结论。

### msprof基本使用方法

执行Profiling前先确认普通运行可以通过：

```bash
cd experiment/simt_experiment/fixed_shape_scatter_<姓名拼音>/build
source /usr/local/Ascend/cann/set_env.sh
./scatter
```

使用msprof采集Profiling数据：

```bash
msprof --output=./prof_scatter ./scatter
```

如果需要显式打开任务耗时、Runtime API和AI Core相关数据，可使用：

```bash
msprof --output=./prof_scatter \
    --runtime-api=on \
    --task-time=on \
    --ai-core=on \
    ./scatter
```

导出summary数据为CSV：

```bash
msprof --export=on --output=./prof_scatter --summary-format=csv
```

如果采集后没有自动解析，可先解析再导出：

```bash
msprof --parse=on --output=./prof_scatter
msprof --export=on --output=./prof_scatter --summary-format=csv
```

查看结果时重点关注：

- `summary`目录下的CSV文件。
- Task相关条目的`task_duration`。
- 不同`blockDim`、`gridDim`组合下`task_duration`的变化。

通常可以按如下方式查找导出的CSV：

```bash
find ./prof_scatter -name "*.csv"
```

### msprof op基本使用方法

`msprof op`用于单算子或算子开发场景的性能分析。该能力依赖CANN安装包中的`msopprof`组件，不同CANN版本和安装形态可能存在差异。使用前先执行：

```bash
msprof op --help
```

如果命令能正常显示帮助信息，再按帮助中的参数格式采集当前样例。如果提示`msopprof does not exist or permission denied`，说明当前环境缺少`msopprof`组件或权限不足，需要先补齐CANN算子开发工具组件或修复权限。

在当前样例调试中，如果`msprof op`不可用，可以先使用普通`msprof --output=... ./scatter`方式采集Task性能数据，并重点对比`task_duration`。

### 调优建议

- 先固定shape和数据生成逻辑，不要同时调整功能和性能参数。
- 每次只调整一个变量，例如只改`blockDim`并保持`gridDim`不变，或只改`gridDim`并保持`blockDim`不变。
- 线程数过少会导致单线程处理多个元素，可能降低并行度。
- 线程数过多可能增加调度开销，且线程块越大，每个线程可用寄存器可能越少。
- Scatter是离散写场景，性能可能主要受Global Memory写入模式、Data Cache命中和写冲突影响；本任务通过无重复index避免写冲突干扰。
- 结论必须基于多轮重复运行，建议每组参数至少运行3次，记录平均值。

## 对外资料链接

- Ascend C SIMD&SIMT混合编程：<https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10052.html>
- Ascend C SIMT线程架构：<https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/Ascendcopdevg/atlas_ascendc_10_10065.html>
- Ascend C SIMT内存层级和`launch_bounds`相关说明：<https://www.hiascend.com/document/detail/zh/canncommercial/900/programug/Ascendcopdevg/atlas_ascendc_10_10066.html>
- SIMT语言扩展层C API：<https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10056.html>
- msprof采集通用命令：<https://www.hiascend.com/document/detail/zh/mindstudio/70RC3/T%26ITools/Profiling/atlasprofiling_16_0009.html>
- msprof解析并导出性能数据：<https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/devaids/Profiling/atlasprofiling_16_0021.html>
