# API流水类型汇总<a name="ZH-CN_TOPIC_0000002533354434"></a>

<cann-filter npu-type="A3,910b">

本节汇总介绍了一些主要的API的流水类型，以下内容针对如下型号生效：

<cann-filter npu-type="A3">

- Atlas A3 训练系列产品/Atlas A3 推理系列产品

</cann-filter>
<cann-filter npu-type="910b">

- Atlas A2 训练系列产品/Atlas A2 推理系列产品

</cann-filter>

**表 1**  Memory矢量计算API流水类型汇总

<a name="table132070121817"></a>
| API类型 | API接口名称 | 执行核（AIC/AIV） | 流水类型 |
| --- | --- | --- | --- |
| 基础算术 | Exp | AIV | PIPE_V |
| 基础算术 | Ln | AIV | PIPE_V |
| 基础算术 | Abs | AIV | PIPE_V |
| 基础算术 | Reciprocal | AIV | PIPE_V |
| 基础算术 | Sqrt | AIV | PIPE_V |
| 基础算术 | Rsqrt | AIV | PIPE_V |
| 基础算术 | Relu | AIV | PIPE_V |
| 基础算术 | Sub | AIV | PIPE_V |
| 基础算术 | Mul | AIV | PIPE_V |
| 基础算术 | Div | AIV | PIPE_V |
| 基础算术 | Max | AIV | PIPE_V |
| 基础算术 | Min | AIV | PIPE_V |
| 基础算术 | Adds | AIV | PIPE_V |
| 基础算术 | Muls | AIV | PIPE_V |
| 基础算术 | Maxs | AIV | PIPE_V |
| 基础算术 | Mins | AIV | PIPE_V |
| 基础算术 | LeakyRelu | AIV | PIPE_V |
| 逻辑计算 | Not | AIV | PIPE_V |
| 逻辑计算 | And | AIV | PIPE_V |
| 逻辑计算 | Or | AIV | PIPE_V |
| 逻辑计算 | ShiftLeft | AIV | PIPE_V |
| 逻辑计算 | ShiftRight | AIV | PIPE_V |
| 复合计算 | Axpy | AIV | PIPE_V |
| 复合计算 | MulAddDst | AIV | PIPE_V |
| 复合计算 | MulCast | AIV | PIPE_V |
| 复合计算 | AddRelu | AIV | PIPE_V |
| 复合计算 | AddReluCast | AIV | PIPE_V |
| 复合计算 | SubRelu | AIV | PIPE_V |
| 复合计算 | SubReluCast | AIV | PIPE_V |
| 复合计算 | FusedMulAdd | AIV | PIPE_V |
| 复合计算 | MulAddRelu | AIV | PIPE_V |
| 复合计算 | CastDequant | AIV | PIPE_V |
| 复合计算 | AddDeqRelu | AIV | PIPE_V |
| 比较与选择 | Compare | AIV | PIPE_V |
| 比较与选择 | Compare（结果存入寄存器） | AIV | PIPE_V |
| 比较与选择 | Compares | AIV | PIPE_V |
| 比较与选择 | GetCmpMask(ISASI) | AIV | PIPE_V |
| 比较与选择 | SetCmpMask(ISASI) | AIV | PIPE_V |
| 比较与选择 | Select | AIV | PIPE_V |
| 比较与选择 | GatherMask | AIV | PIPE_V |
| 类型转换 | Cast | AIV | PIPE_V |
| 归约计算 | ReduceDataBlock | AIV | PIPE_V |
| 归约计算 | ReduceRepeat | AIV | PIPE_V |
| 归约计算 | ReducePairElem | AIV | PIPE_V |
| 归约计算 | ReduceMax | AIV | PIPE_V |
| 归约计算 | ReduceMin | AIV | PIPE_V |
| 归约计算 | ReduceSum | AIV | PIPE_V |
| 归约计算 | GetReduceRepeatSumSpr | AIV | PIPE_S |
| 归约计算 | GetReduceRepeatMaxMinSpr | AIV | PIPE_S |
| 数据排布格式转换 | Transpose | AIV | PIPE_V |
| 数据排布格式转换 | TransDataTo5HD | AIV | PIPE_V |
| 数据填充 | Brcb | AIV | PIPE_V |
| 数据填充 | Duplicate | AIV | PIPE_V |
| 数据填充 | CreateVecIndex | AIV | PIPE_V |
| 排序组合 | Sort32 | AIV | PIPE_V |
| 排序组合 | MrgSort | AIV | PIPE_V |
| 排序组合 | GetMrgSortResult | AIV | PIPE_S |
| 离散与聚合 | Gather | AIV | PIPE_V |
| 离散与聚合 | Gatherb | AIV | PIPE_V |
| 掩码操作 | SetMaskCount | AIV | PIPE_S |
| 掩码操作 | SetMaskNorm | AIV | PIPE_S |
| 掩码操作 | SetVectorMask | AIV | PIPE_S |
| 掩码操作 | ResetMask | AIV | PIPE_S |
| 数据搬运 | DataCopy(GM->UB) | AIV | PIPE_MTE2 |
| 数据搬运 | DataCopy(UB->GM) | AIV | PIPE_MTE3 |
| 数据搬运 | DataCopyPad(GM->UB) | AIV | PIPE_MTE2 |
| 数据搬运 | DataCopyPad(UB->GM) | AIV | PIPE_MTE3 |
| 数据搬运 | DataCopy(UB->UB) | AIV | PIPE_V |
| 数据搬运 | Copy | AIV | PIPE_V |

**表 2**  Cube API流水类型汇总

<a name="table12311219174812"></a>
| API接口名称 | 执行核（AIC/AIV） | 流水类型 |
| --- | --- | --- |
| DataCopy(GM->L1) | AIC | PIPE_MTE2 |
| Load2D(GM->L1) | AIC | PIPE_MTE2 |
| Load2DV2(GM->L1) | AIC | PIPE_MTE2 |
| Load2D(GM->L0) | AIC | PIPE_MTE2 |
| Load2D(L1->L0) | AIC | PIPE_MTE1 |
| Load3D(L1->L0) | AIC | PIPE_MTE1 |
| LoadDataWithTranspose(L1->L0) | AIC | PIPE_MTE1 |
| LoadDataWithSparse(L1->L0) | AIC | PIPE_MTE1 |
| Bias Table Buffer(L1->BT) | AIC | PIPE_MTE1 |
| Fixpipe Buffer(L1->FB) | AIC | PIPE_MTE1 |
| Fill(L1) | AIC | PIPE_MTE2 |
| Fill(L0) | AIC | PIPE_MTE1 |
| SetFmatrix | AIC | PIPE_S |
| SetLoadDataBoundary | AIC | PIPE_S |
| SetLoadDataRepeat | AIC | PIPE_S |
| SetLoadDataPaddingValue | AIC | PIPE_S |
| Mmad | AIC | PIPE_M |
| MmadWithSparse | AIC | PIPE_M |
| SetMMColumnMajor/SetMMRowMajor | AIC | PIPE_S |
| SetHF32Mode | AIC | PIPE_S |
| SetHF32TransMode | AIC | PIPE_S |
| DataCopy(L0C->GM) | AIC | PIPE_FIX |
| DataCopy(L0C->L1) | AIC | PIPE_FIX |
| DataCopy(L1->GM) | AIC | PIPE_MTE3 |
| Fixpipe(L0C->GM) | AIC | PIPE_FIX |
| Fixpipe(L0C->L1) | AIC | PIPE_FIX |
| SetFixPipeConfig | AIC | PIPE_S |
| SetFixpipePreQuantFlag | AIC | PIPE_S |
| SetFixpipeNz2ndFlag | AIC | PIPE_S |
| SetLreluALPHA | AIC | PIPE_S |

**表 3**  标量计算指令流水类型汇总

| API接口名称 | 流水类型 |
| --- | --- |
| GetBitCount | PIPE_S |
| CountLeadingZero | PIPE_S |
| GetSFFValue | PIPE_S |
| CountBitsCntSameAsSignBit | PIPE_S |
| Cast（float转half、int32_t） | PIPE_S |
| Cast（float转bfloat16_t） | PIPE_S |
| Cast（bfloat16_t转float） | PIPE_S |
| ReadGmByPassDCache(ISASI) | PIPE_S |
| WriteGmByPassDCache(ISASI) | PIPE_S |

**表 4**  同步指令流水类型汇总

<a name="table1054353913238"></a>
| API接口名称 | 流水类型 |
| --- | --- |
| SetFlag/WaitFlag | PIPE_S |
| PipeBarrier | PIPE_S |
| DataSyncBarrier | PIPE_S |
| CrossCoreSetFlag | PIPE_S |
| CrossCoreWaitFlag | PIPE_S |
| SyncAll（硬件同步接口） | PIPE_S |
| SetNextTaskStart | PIPE_S |
| WaitPreTaskEnd | PIPE_S |

**表 5**  原子操作流水类型汇总

<a name="table25871912193814"></a>
| API接口名称 | 流水类型 |
| --- | --- |
| SetAtomicAdd | PIPE_S |
| SetAtomicMax | PIPE_S |
| SetAtomicMin | PIPE_S |
| SetAtomicType | PIPE_S |
| DisableDmaAtomic | PIPE_S |
| GetStoreAtomicConfig(ISASI) | PIPE_S |
| SetStoreAtomicConfig(ISASI) | PIPE_S |

**表 6**  工具接口流水类型汇总

<a name="table1147331317457"></a>
| API接口名称 | 流水类型 |
| --- | --- |
| InitSocState | PIPE_S |
| GetBlockNum | PIPE_S |
| GetBlockIdx | PIPE_S |
| GetTaskRatio | PIPE_S |
| GetSubBlockNum | PIPE_S |
| GetSubBlockIdx | PIPE_S |
| GetDataBlockSizeInBytes | PIPE_S |
| GetSystemCycle | PIPE_S |
| GetProgramCounter | PIPE_S |
| GetArchVersion | PIPE_S |

**表 7**  缓存控制接口流水类型汇总

| API接口名称 | 流水类型 |
| --- | --- |
| DataCachePreload | PIPE_S |
| DataCacheCleanAndInvalid | PIPE_S |
| ICachePreLoad(ISASI) | PIPE_S |
| GetICachePreloadStatus(ISASI) | PIPE_S |

</cann-filter>
