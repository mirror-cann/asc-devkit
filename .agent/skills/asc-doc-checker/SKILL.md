---
name: doc-checker
description: |
  文档写作规范检查工具。当用户需要检查 Markdown文档是否符合文档写作规范时使用此技能。
  适用场景包括：检查文档格式、标点符号、表格语法、图片路径、代码块规范、标题层级等是否符合标准。
  也适用于用户提到"检查文档"、"规范检查"、"写作规范"等关键词时触发。
---

# 文档写作规范检查技能

## 技能用途

根据项目文档写作规范，检查用户提供的 Markdown 文档是否符合标准格式。

## 检查项目清单（一定要逐项检查，不能跳过或者忽略）
<!-- 多余空格和英文标点误用的问题在扫描后也可能遗漏，人工自检时可以采用以下正则表达式搜索高亮：[\u4e00-\u9fff] [A-Za-z]|[A-Za-z] [\u4e00-\u9fff]|[\u4e00-\u9fff] [0-9]|[0-9] [\u4e00-\u9fff]|[\u4e00-\u9fffA-Za-z0-9][,.:;!?()"]|[\u4e00-\u9fffA-Za-z] [/]|\)|\(| /|/ |[\u4e00-\u9fffA-Za-z0-9] [+-=/!] -->

### 1. 标点符号与空格使用规范
- [ ] 中文描述中使用中文标点符号（，。、；：？！""''（）【】），不使用英文标点
- [ ] 标点符号完整、准确，括号成对闭合，避免缺失或多余，尤其是句子结尾需要有句号（包括代码注释）
- [ ] 中文与英文单词之间、中文与数字之间不能有空格

检测正则表达式（用于匹配空格和英文标点问题）：
```
[\u4e00-\u9fff] [A-Za-z]|[A-Za-z] [\u4e00-\u9fff]|[\u4e00-\u9fff] [0-9]|[0-9] [\u4e00-\u9fff]|[\u4e00-\u9fffA-Za-z0-9][,.:;!?()"]|[\u4e00-\u9fffA-Za-z] [/]|\)|\(| /|/
```

正则匹配说明：
- `[\u4e00-\u9fff] [A-Za-z]`：中文字符后有空格再接英文字母
- `[A-Za-z] [\u4e00-\u9fff]`：英文字母后有空格再接中文字符
- `[\u4e00-\u9fff] [0-9]`：中文字符后有空格再接数字
- `[0-9] [\u4e00-\u9fff]`：数字后有空格再接中文字符
- `[\u4e00-\u9fffA-Za-z0-9][,.:;!?()"]`：字符直接接英文标点（需确认是否应为中文标点）
- `[\u4e00-\u9fffA-Za-z] [/]`：字符后有空格再接斜杠

**错误示例：**
这是一个测试:包含了英文冒号。

**正确示例：**
这是一个测试：包含了中文冒号。

**错误示例：**
使用 API 进行调用

**正确示例：**
使用API进行调用

**错误示例：**
版本 1.0 发布了

**正确示例：**
版本1.0发布了

### 2. Markdown 格式规范
- [ ] 标题使用 `# [空格][标题名]` 格式
- [ ] 标题级别逐级递增，最多四级
- [ ] 标题末尾不加标点
- [ ] 标题中不能手工添加序号（如 "## 1. 安装" 应改为 "## 安装"）

### 3. 代码块规范
- [ ] 行内代码使用 1 对反引号：`` `code` ``
- [ ] **全文所有**代码块均使用 3 个反引号，并声明语言类型（如 `cpp`、`text` 等），不要遗漏函数原型等非调用示例的代码块
- [ ] 代码块上下空一行
- [ ] 代码块内缩进 2/4 个空格

### 4. 图片规范
- [ ] 使用相对路径引用图片
- [ ] 路径使用正斜杠 `/`（不是反斜杠 `\`）
- [ ] 使用 `![](图片路径)` 格式
- [ ] 所有图片统一存放在 `docs/api/figures/` 目录下，引用时根据不同文档位置计算相对路径
- [ ] 所有图片的文件名必须是英文小写加下划线（如：atomic_operation_pipelined_data_movement_diagram.png、atomic_operation_3.png）
- [ ] 如果有图注（没有图注的话就忽略），图注（如"图 1"、"图 2"）必须写在图片上方，格式为 `**图 n**  图片标题`（注意 `**图 n**` 和标题之间是两个空格）
  **正确示例（在 `docs/api/SIMD-API/基础API/原子操作/原子操作概述.md` 中引用）：**
  ```
  **图 1**  数据搬运随路原子累加效果示意图
  ![](../../../figures/atomic_operation_pipelined_data_movement_diagram.png)
  ```
  **错误示例：**
  ```
  ❌ 路径使用了反斜杠
  ![](..\..\..\figures\atomic_operation_pipelined_data_movement_diagram.png)
  ```
### 5. 图和表编号规范
- [ ] 文中的图和表从1开始递增编号（如"图 1"、"图 2"；"表 1"、"表 2"），不允许中断或跳号
- [ ] 图注和表格标题严格区分，图上方为"**图 n**  标题"，表上方为"**表 n**  标题"
- [ ] 同一个文档中不同章节共享同一个图/表编号序列，不按章节重新编号

  **正确示例（依次出现时编号连续）：**
  ```
  **图 1**  xxx
  ![](../figures/xxx.png)

  **图 2**  xxx
  ![](../figures/xxx.png)

  **表 1**  xxx
  | a | b |
  ```

  **错误示例：**
  ```
  **图 1**  xxx        ← 正确
  **图 3**  xxx        ← ❌ 跳过了图2
  ```

### 6. 引用规范
- [ ] /docs/api目录中各个文件之间相互引用使用相对路径，格式为`[xxx](../xxx.md)`
- [ ] /docs/api目录中API文件和/docs/guide目录中的文件相互引用时，必须用https链接，格式为`[xxx](https://gitcode.com/cann/asc-devkit/blob/master/docs/xxxxx)`
- [ ] 调用示例部分引用样例必须用https链接，格式为`[xxx](https://gitcode.com/cann/asc-devkit/tree/master/examples/xxx)`

### 7. 无序列表格式
- [ ] 只有一个条目时，不需要使用无序列表符号（- 或 *）

**错误示例：**
- 这是一个单独的条目

**正确示例：**
这是一个单独的条目

### 8. 芯片过滤标签（多芯片差异内容）
- [ ] “Atlas A2/A3”是错误的描述，一旦出现务必告警！，正确的做法是将描述拆分为两个版本分别描述，即使这段描述对于两个版本都相同。
- [ ] 支持的芯片类型：950、910b、A3、910、310p、310b、x90、9030
- [ ] 标签内需显式说明芯片型号
- [ ] `npu-type` 的值与标签内描述的芯片产品名称必须一致，参照以下映射关系（来源：`references/芯片与标签映射关系.md`）：

  | `npu-type` | 对应的芯片产品名称 |
  |------------|-------------------|
  | 950 | `Ascend 950PR/Ascend 950DT` |
  | A3 | `Atlas A3 训练系列产品/Atlas A3 推理系列产品` |
  | 910b | `Atlas A2 训练系列产品/Atlas A2 推理系列产品` |
  | 310b | `Atlas 200I/500 A2 推理产品` |
  | 310p | `Atlas 推理系列产品 AI Core` |
  | 310p | `Atlas 推理系列产品 Vector Core` |
  | 910 | `Atlas 训练系列产品` |
  | x90 | `Kirin X90` |
  | 9030 | `Kirin 9030` |

  **错误示例**（标签值与描述不匹配）：
  ```
  <cann-filter npu-type="310b">
  Atlas 推理系列产品    ← ❌ 310b 对应的应是 "Atlas 200I/500 A2 推理产品"
  </cann-filter>
  ```

- [ ] 产品支持情况表格中，芯片的排列顺序应按照 950 → A3 → 910b → 310b → 310p → 910 的顺序排列（即支持的产品优先排列，不支持的产品排列在最后，同类产品中位宽越大越靠前）。当存在多个 `<cann-filter>` 块罗列时，块之间的顺序也应遵循此规则。

  **正确示例（芯片顺序）：**
  ```
  | 产品 | 是否支持 |
  |------|----------|
  | <cann-filter npu-type="950">... | √</cann-filter> |
  | <cann-filter npu-type="A3">... | √</cann-filter> |
  | <cann-filter npu-type="910b">... | √</cann-filter> |
  | <cann-filter npu-type="310b">... | x</cann-filter> |
  | <cann-filter npu-type="310p">... | x</cann-filter> |
  | <cann-filter npu-type="910">... | x</cann-filter> |
  ```

  **错误示例（芯片顺序混乱）：**
  ```
  | 产品 | 是否支持 |
  |------|----------|
  | <cann-filter npu-type="910b">... | √</cann-filter> |   ← ❌ 应为950在前
  | <cann-filter npu-type="A3">... | √</cann-filter> |
  | <cann-filter npu-type="950">... | √</cann-filter> |   ← ❌ 950应排在910b之前
  | <cann-filter npu-type="910">... | x</cann-filter> |
  | <cann-filter npu-type="310b">... | x</cann-filter> |
  | <cann-filter npu-type="310p">... | x</cann-filter> |
  ```

- [ ] 如果文本中出现了以上这些芯片产品名称，但是这部分文本并没有被相应的 `<cann-filter>` 标签包裹，需要添加该标签。
  **正确示例：**参数config只有Ascend 950PR/Ascend 950DT支持，需要加上标签
| <cann-filter npu-type="950"> config | 该参数仅支持如下型号:Ascend 950PR/Ascend 950DT。<br>控制SyncAll函数的行为，在多个AI Core之间进行流水线同步时，指定哪些管道（pipe）用于触发和等待。<br>&bull; **triggerPipe**：指定哪个管道用于"发送触发信号"。<br>&bull; **waitPipe**：指定哪个管道用于"接收等待信号"。<br>默认为SyncAllConfig DEFAULT_SYNC_ALL_CONFIG= {PIPE_ALL, PIPE_ALL}，使用全部管道来进行触发和等待行为。</cann-filter> |

  **错误示例：**只有文本中出现了Ascend 950PR/Ascend 950DT产品，但是没有被相应的 `<cann-filter>` 标签包裹，需要添加该标签
| config | 该参数仅支持如下型号:Ascend 950PR/Ascend 950DT。<br>控制SyncAll函数的行为，在多个AI Core之间进行流水线同步时，指定哪些管道（pipe）用于触发和等待。<br>&bull; **triggerPipe**：指定哪个管道用于"发送触发信号"。<br>&bull; **waitPipe**：指定哪个管道用于"接收等待信号"。<br>默认为SyncAllConfig DEFAULT_SYNC_ALL_CONFIG= {PIPE_ALL, PIPE_ALL}，使用全部管道来进行触发和等待行为。|

### 9. 数据类型规范
- [ ] 数据类型的写法与代码中保持一致，必须使用标准类型名（如 `int8_t`），不能简写或变体（如 ❌ `Int8` / `int8` / `S8`）
- [ ] 数据类型的写作顺序保持一致（原则：位宽从小到大、有符号数→无符号数→浮点数、浮点数按指数位大小排序）：bool、int4b_t、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64
- [ ] 接口支持的数据类型只和位宽有关时，可通过位宽表达：b8、b16、b32、b64，注意表示位宽时，字母b不能大写。
- [ ] 多个数据类型用中文顿号分隔

### 10. API 文档章节结构（API 文件必检）
检查 API 文档是否包含以下标准章节，且严格按此顺序排列（一个不能缺，顺序不能乱）：

**标准章节顺序：**
- [ ] 产品支持情况
- [ ] 功能说明
- [ ] 函数原型
- [ ] 参数说明
- [ ] 数据类型（可选章节，不涉及可以没有）
- [ ] 返回值说明
- [ ] 约束说明
- [ ] 调用示例

注：概述类索引文件除外，单个 API 文档应包含上述章节。

### 11. 术语规范
检查文档中是否使用了非标准术语，遇到可疑短语（与术语类似的缩写/简写）应提出警示。

**缓冲区术语**（必须带"Buffer"后缀）：
- ✅ L0A Buffer / ❌ L0A / ❌ L0A
- ✅ L0B Buffer / ❌ L0B / ❌ LOB
- ✅ L0C Buffer / ❌ L0C / ❌ LOC
- ✅ L1 Buffer / ❌ L1（当指代物理缓冲区时）
- ✅ Unified Buffer / ✅ UB（两者均可）
- ✅ BiasTable Buffer / ❌ BiasTable
- ✅ Fixpipe Buffer / ❌ Fixpipe

**常见缩写规范写法**：
- ✅ Global Memory / ✅ GM（两者均可）
- ✅ Unified Buffer / ✅ UB（两者均可）
- ✅ AI Core / ❌ AICore
- ✅ Cube Core / ✅ Vector Core / ✅ Scalar
- ✅ Cube Unit / ✅ Vector Unit / ✅ Scalar Unit

**芯片产品名称规范写法**：
- ✅ Atlas 训练系列产品 / ❌ Atlas训练系列产品
- ✅ Atlas A2 训练系列产品 / ❌ Atlas A2训练系列产品
- ✅ Atlas 推理系列产品 / ❌ Atlas推理系列产品
- ✅ Ascend 950PR / ❌ ascend 950PR
- ✅ Ascend 950DT / ❌ ascend 950DT

**数据通路写法**：
- ✅ VECOUT->GM / ✅ UB->GM
- ❌ VECOUT → GM（使用 `->` 而非 `→`）

**检查原则**：遇到可疑的短语（如单独出现的 L0C、L1、L0A 等不带 Buffer 后缀的缓冲区名称），应提出警示并建议修正为标准术语。代码块内的注释也需要检查术语。

术语标准参考来源：`references/terminology.md`

### 12. 错别字与语病检查
检查文档中是否存在错别字、语病或者逻辑不通顺之处，注意调用示例部分的注释，也需要检查。

### 13. 头文件路径规范
- [ ] 参考示范：头文件路径为：`"basic_api/kernel_operator_cache_intf.h"`。
- [ ] 文字描述一定是“头文件路径为：”
- [ ] 单个反引号包裹路径

### 14. 已变更接口检查
- [ ] 先读取 `docs/api/附录/接口变更说明.md`，从其中的接口变更表格获取最新的旧→新接口名映射关系。
- [ ] 依据读取到的最新映射关系，检查文档中是否出现已废弃的旧接口名（代码块和调用示例中的也需要检查）。
- [ ] 如果发现旧接口名，应提示用户替换为新接口名，报告中明确指出具体位置、旧名称和推荐的新名称。

## 使用流程

1. **读取文件**：使用 `read_file` 工具读取用户指定的 Markdown 文件
2. **逐项检查**：按照上述检查清单逐项核对文档内容
3. **输出报告**：列出所有发现的问题，格式如下：
   ```
   ## 文档检查报告

   文件：xxx.md

   ### 发现的问题

   | # | 检查项 | 位置 | 问题描述 | 修改建议 |
   |---|--------|------|----------|----------|
   | 1 | **[标点符号]** | [第 XX 行](file:///绝对路径:XX) | 中文内容使用了英文逗号 | 将 `,` 改为 `，` |
   | 2 | **[表格格式]** | [第 XX 行](file:///绝对路径:XX) | 使用了 HTML 表格 | 改为 Markdown 表格 |
   ...

   ### 合格项

   - [x] 标题格式正确
   - [x] 代码块语法正确
   ...
   ```
4. **跳转功能**：报告中每个问题的"位置"列提供可点击链接，格式为 `[第 XX 行](file:///绝对路径:XX)`，用户点击后可直接跳转到文档对应行。绝对路径从读取文件时获取。
5. **修改建议与确认**：报告输出后，使用 `ask_followup_question` 工具将所有问题逐项（或分批）列出，每项包含：
   - 问题描述
   - 原文内容
   - 建议修改为的内容
   用户确认后，使用 `replace_in_file` 工具逐项执行修改。

## 参考文档

详细的写作规范请参考 `references/writing_specs.md`。
详细的术语标准请参考 `references/terminology.md`。
