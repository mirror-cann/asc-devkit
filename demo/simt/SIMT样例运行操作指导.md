# SIMT样例运行操作指导

本文档记录从 GitCode 下载 `cann/asc-devkit` 仓库 `exp/kadc` 分支，并编译运行 `demo/simt` 下两个样例的完整步骤。

## 1. 环境说明

- 仓库地址：`https://gitcode.com/cann/asc-devkit.git`
- 分支：`exp/kadc`
- 仓代码根路径：`<asc-devkit代码仓根路径>`
- CANN 环境脚本：`/usr/local/Ascend/cann/set_env.sh`

`demo/simt` 下包含两个样例：

| 样例目录 | 说明 |
|---|---|
| `demo/simt/basic_scatter` | SIMT Scatter 样例 |
| `demo/simt/matrix_transpose` | SIMT MatrixTranspose 访存优化样例 |

## 2. 查看代码

查看 `demo/simt` 下的样例：

```text
demo/simt/basic_scatter
demo/simt/matrix_transpose
```

## 3. 配置 CANN 环境

每次打开新终端后，先设置配置CANN环境：

```bash
source /usr/local/Ascend/cann/set_env.sh
```


## 4. 运行 basic_scatter 样例

进入样例目录：

```bash
cd ${ASC_DEVKIT_ROOT}/demo/simt/basic_scatter
```

创建并进入构建目录：

```bash
mkdir -p build
cd build
```

配置工程：

```bash
cmake ..
```

编译工程：

```bash
make -j
```

运行默认样例：

```bash
./scatter
```

本次验证关键输出：

```text
[INFO] input_length=10, output_length=50
[INFO] Block number is 1.
[INFO] Thread number in a block is 10.
[Success] Case accuracy is verification passed.
```

运行指定 shape 样例：

```bash
./scatter 8 32
```

本次验证关键输出：

```text
[INFO] input_length=8, output_length=32
[INFO] Block number is 1.
[INFO] Thread number in a block is 8.
[Success] Case accuracy is verification passed.
```

## 5. 运行 matrix_transpose 样例

进入样例目录：

```bash
cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose
```

该样例通过编译参数 `SCENARIO_NUM` 选择不同场景：

| SCENARIO_NUM | 场景 |
|---|---|
| `0` | copy |
| `1` | transpose_naive |
| `2` | transpose_coalesced |
| `3` | transpose_avoid_bank_conflicts |

逐个编译并运行 4 个场景：

```bash
for s in 0 1 2 3; do
  echo "=== matrix_transpose SCENARIO_NUM=${s} ==="
  mkdir -p build_case_${s}
  cd build_case_${s}
  cmake -DSCENARIO_NUM=${s} ..
  make -j
  ./matrix_transpose
  cd ..
done
```

本次验证关键输出：

```text
=== matrix_transpose SCENARIO_NUM=0 ===
Running case 0: copy...
[Success] Case accuracy is verification passed.

=== matrix_transpose SCENARIO_NUM=1 ===
Running case 1: transpose_naive...
[Success] Case accuracy is verification passed.

=== matrix_transpose SCENARIO_NUM=2 ===
Running case 2: transpose_coalesced...
[Success] Case accuracy is verification passed.

=== matrix_transpose SCENARIO_NUM=3 ===
Running case 3: transpose_avoid_bank_conflicts...
[Success] Case accuracy is verification passed.
```

## 6. matrix_transpose 性能测试任务

完成 4 个场景的正确性验证后，使用 `msprof` 分别采集 case0 到 case3 的性能数据，并对比不同实现方式的性能表现。

### 6.1 测试目标

对以下 4 个场景分别采集性能数据：

| Case | SCENARIO_NUM | 场景 | 性能关注点 |
|---|---:|---|---|
| case0 | `0` | copy | 作为连续读写拷贝基线 |
| case1 | `1` | transpose_naive | 观察朴素转置的不连续访存开销 |
| case2 | `2` | transpose_coalesced | 观察合并访存优化后的性能变化 |
| case3 | `3` | transpose_avoid_bank_conflicts | 观察规避Bank冲突后的性能变化 |

对比时重点记录：

- 程序是否仍然输出 `[Success] Case accuracy is verification passed.`。
- Kernel/Task耗时。
- Host侧端到端耗时。
- case1、case2、case3相对case0和前一优化版本的性能变化。

### 6.2 采集前准备

进入 `matrix_transpose` 样例目录，并确保 4 个场景都已编译完成：

```bash
source /usr/local/Ascend/cann/set_env.sh
cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose

for s in 0 1 2 3; do
  echo "=== build matrix_transpose SCENARIO_NUM=${s} ==="
  mkdir -p build_case_${s}
  cd build_case_${s}
  cmake -DSCENARIO_NUM=${s} ..
  make -j
  ./matrix_transpose
  cd ..
done
```

### 6.3 使用 msprof 采集 case0 到 case3

建议每个case单独输出到独立目录，避免性能数据互相覆盖：

```bash
cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose

for s in 0 1 2 3; do
  echo "=== profile matrix_transpose case ${s} ==="
  rm -rf prof_case_${s}
  msprof --output=./prof_case_${s} \
      --runtime-api=on \
      --task-time=on \
      --ai-core=on \
      ./build_case_${s}/matrix_transpose
done
```

如果采集后没有自动生成可读summary，可执行解析和导出：

```bash
cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose

for s in 0 1 2 3; do
  echo "=== export matrix_transpose case ${s} ==="
  msprof --parse=on --output=./prof_case_${s}
  msprof --export=on --output=./prof_case_${s} --summary-format=csv
done
```

查看导出的CSV文件：

```bash
find ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose/prof_case_* -name "*.csv" | sort
```

### 6.4 端到端耗时采集

`msprof`用于查看Runtime API、Task和AI Core侧性能数据；端到端耗时可以用 `/usr/bin/time` 补充记录：

```bash
cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose

for s in 0 1 2 3; do
  echo "=== e2e matrix_transpose case ${s} ==="
  /usr/bin/time -f "case ${s} elapsed=%e sec" ./build_case_${s}/matrix_transpose
done
```

端到端耗时包含Host侧初始化、内存申请、数据拷贝、Kernel执行、同步、结果拷回和校验等开销；Kernel/Task耗时更适合观察Device侧算子实现差异。

### 6.5 性能对比记录表

建议将采集结果整理为如下表格：

| Case | 场景 | 正确性 | 端到端耗时 | Kernel/Task耗时 | 相对case1提升 | 现象说明 |
|---|---|---|---:|---:|---:|---|
| case0 | copy | pass/fail | 待填写 | 待填写 | - | 连续读写基线 |
| case1 | transpose_naive | pass/fail | 待填写 | 待填写 | - | 朴素转置，可能存在不连续访存 |
| case2 | transpose_coalesced | pass/fail | 待填写 | 待填写 | 待填写 | 通过访存合并优化全局内存访问 |
| case3 | transpose_avoid_bank_conflicts | pass/fail | 待填写 | 待填写 | 待填写 | 进一步规避共享存储Bank冲突 |

分析结论建议包含：

- case1相对case0的差距，说明转置操作引入的不连续访存开销。
- case2相对case1的变化，说明合并访存是否带来收益。
- case3相对case2的变化，说明规避Bank冲突是否进一步改善性能。
- 如果某个case性能没有提升，需要结合矩阵规模、硬件、编译选项和msprof数据说明可能原因。

## 7. 一键复现实验命令

如果已经完成代码下载，可直接执行下面命令重新编译并运行两个样例：

```bash
source /usr/local/Ascend/cann/set_env.sh
cd ${ASC_DEVKIT_ROOT}

cd ${ASC_DEVKIT_ROOT}/demo/simt/basic_scatter
mkdir -p build
cd build
cmake ..
make -j
./scatter
./scatter 8 32

cd ${ASC_DEVKIT_ROOT}/demo/simt/matrix_transpose
for s in 0 1 2 3; do
  echo "=== matrix_transpose SCENARIO_NUM=${s} ==="
  mkdir -p build_case_${s}
  cd build_case_${s}
  cmake -DSCENARIO_NUM=${s} ..
  make -j
  ./matrix_transpose
  cd ..
done
```

## 8. 注意事项

1. `SCENARIO_NUM` 必须显式设置为 `0`、`1`、`2` 或 `3`。不要执行 `cmake -DSCENARIO_NUM=$SCENARIO_NUM ..` 时让 shell 变量为空，否则会生成空宏并导致编译失败。
2. 如果运行时报 `context is a null pointer`，优先检查 CANN 环境是否已 source、设备是否可见、当前用户是否有设备访问权限。
3. 若复用旧构建目录遇到异常，可删除对应 `build` 或 `build_case_*` 目录后重新配置。
