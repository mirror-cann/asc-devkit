# GetWindowsInAddr

## 产品支持情况

- Ascend 950PR/Ascend 950DT：不支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

获取卡间通信数据WindowsIn起始地址，可用来直接作为计算的输入输出地址，减少拷贝。该接口默认在所有核上工作，用户也可以在调用前通过[GetBlockIdx](../../../基础API/工具接口/系统资源与变量/GetBlockIdx.md)指定其在某一个核上运行。

## 函数原型

```
__aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId)
```

## 参数说明

**表 1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| rankId | 输入 | 待查询的卡的Id。 |

## 返回值说明

返回对应卡的卡间通信数据WindowsIn起始地址。当rankId非法时，返回nullptr。

## 约束说明

无

## 调用示例

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData为对应算子头文件定义的结构体
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // AscendC自定义算子kernel中，通过此方式获取HCCL context
hccl.InitV2(contextGM, &tilingData);

auto winInAddr = hccl.GetWindowsInAddr(0);
auto winOutAddr = hccl.GetWindowsOutAddr(0);
auto rankId = hccl.GetRankId();
auto rankDim = hccl.GetRankDim();  // 4张卡
```

