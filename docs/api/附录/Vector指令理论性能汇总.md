# Vector指令理论性能汇总<a name="ZH-CN_TOPIC_0000002557396789"></a>

本节汇总介绍了一些主要的Vector指令的理论性能，以下内容针对如下型号生效：
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->

注：
- 缓存控制、同步控制和原子操作类指令，不涉及理论性能。
- 对于矢量计算API，若接口调用的传入参数`count`或`repeatTime`取值为0，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作），但相较于不调用会有额外性能耗时。

**表1**  基础算术类指令理论性能汇总

<a name="table291476204714"></a>
| 接口 | 输入/输出数据类型 | 硬件并行度（单位：element/cycle） |
| --- | --- | --- |
| Add | half | 128 |
| Add | int16_t | 128 |
| Add | int32_t | 64 |
| Add | float | 64 |
| Exp | half | 32 |
| Exp | float | 32 |
| Ln | half | 32 |
| Ln | float | 32 |
| Abs | half | 128 |
| Abs | float | 64 |
| Reciprocal | half | 128 |
| Reciprocal | float | 64 |
| Sqrt | half | 32 |
| Sqrt | float | 32 |
| Rsqrt | half | 128 |
| Rsqrt | float | 64 |
| Relu | half | 128 |
| Relu | float | 64 |
| Relu | int32_t | 64 |
| Sub | half | 128 |
| Sub | int16_t | 128 |
| Sub | int32_t | 64 |
| Sub | float | 64 |
| Mul | half | 128 |
| Mul | int16_t | 128 |
| Mul | int32_t | 64 |
| Mul | float | 64 |
| Div | half | 32 |
| Div | float | 32 |
| Max | half | 128 |
| Max | int16_t | 128 |
| Max | int32_t | 64 |
| Max | float | 64 |
| Min | half | 128 |
| Min | int16_t | 128 |
| Min | int32_t | 64 |
| Min | float | 64 |
| BilinearInterpolation(ISASI) | half | 软仿指令，不涉及理论并行度 |
| Adds | half | 128 |
| Adds | int16_t | 128 |
| Adds | int32_t | 64 |
| Adds | float | 64 |
| Muls | half | 128 |
| Muls | int16_t | 128 |
| Muls | int32_t | 64 |
| Muls | float | 64 |
| Maxs | half | 128 |
| Maxs | int16_t | 128 |
| Maxs | int32_t | 64 |
| Maxs | float | 64 |
| Mins | half | 128 |
| Mins | int16_t | 128 |
| Mins | int32_t | 64 |
| Mins | float | 64 |
| LeakyRelu | half | 128 |
| LeakyRelu | int16_t | 128 |
| LeakyRelu | int32_t | 64 |
| LeakyRelu | float | 64 |

**表2**  逻辑计算类指令理论性能汇总

<a name="table116881509216"></a>
| 接口 | src数据类型 | 硬件并行度（单位：element/cycle） |
| --- | --- | --- |
| Not | int16_t/uint16_t | 128 |
| And | int16_t/uint16_t | 128 |
| Or | int16_t/uint16_t | 128 |
| ShiftLeft | int16_t/uint16_t | 128 |
| ShiftLeft | int32_t/uint32_t | 64 |
| ShiftRight | int16_t/uint16_t | 128 |
| ShiftRight | int32_t/uint32_t | 64 |

**表3**  复合计算类指令理论性能汇总

<a name="table1089823515222"></a>
| 接口 | src0数据类型 | scalar/src1数据类型 | dst数据类型 | 硬件并行度（单位：element/cycle） |
| --- | --- | --- | --- | --- |
| Axpy | half | half | half | 128 |
| Axpy | float | float | float | 64 |
| Axpy | half | half | float | 64 |
| MulAddDst | half | half | half | 64 |
| MulAddDst | float | float | float | 32 |
| MulAddDst | half | half | float | 32 |
| MulCast | half | half | uint8_t | 128 |
| MulCast | half | half | int8_t | 128 |
| AddRelu | int16_t | int16_t | int16_t | 128 |
| AddRelu | half | half | half | 128 |
| AddRelu | float | float | float | 64 |
| AddReluCast | float | float | half | 64 |
| AddReluCast | half | half | int8_t | 128 |
| AddReluCast | int16_t | int16_t | int8_t | 128 |
| SubRelu | int16_t | int16_t | int16_t | 128 |
| SubRelu | half | half | half | 128 |
| SubRelu | float | float | float | 64 |
| SubReluCast | float | float | half | 64 |
| SubReluCast | half | half | int8_t | 128 |
| SubReluCast | int16_t | int16_t | int8_t | 128 |
| FusedMulAdd | half | half | half | 128 |
| FusedMulAdd | float | float | float | 64 |
| MulAddRelu | half | half | half | 128 |
| MulAddRelu | float | float | float | 64 |
| CastDequant | int16_t | NA | int8_t/uint8_t | 128 |
| CastDequant | int32_t | NA | half | 64 |
| AddDeqRelu | - | - | - | 软仿指令，不涉及理论并行度 |

**表4**  比较与选择类指令理论性能汇总

<a name="table205131952256"></a>
| 接口 | src0数据类型 | scalar/src1数据类型 | dst数据类型 | 硬件并行度（单位：element/cycle） |
| --- | --- | --- | --- | --- |
| Compare | half | half | uint8_t、int8_t | 128 |
| Compare | float | float | uint8_t、int8_t | 64 |
| Compare | int32_t | int32_t | uint8_t、int8_t | 64 |
| Compare（结果存入寄存器） | half | half | uint8_t、int8_t | 128 |
| Compare（结果存入寄存器） | float | float | uint8_t、int8_t | 64 |
| Compares | half | half | uint8_t | 128 |
| Compares | float | float | uint8_t | 64 |
| Compares | int32_t | int32_t | uint8_t | 64 |
| Select | half | half | uint8_t | 128 |
| Select | float | float | uint8_t | 64 |
| GetCmpMask(ISASI) | - | - | - | 1 |
| SetCmpMask(ISASI) | - | - | - | 1 |
| GatherMask | half | uint16_t | half | 64 |
| GatherMask | bfloat16_t | uint16_t | bfloat16_t | 64 |
| GatherMask | uint16_t | uint16_t | uint16_t | 64 |
| GatherMask | int16_t | uint16_t | int16_t | 64 |
| GatherMask | float | uint32_t | float | 32 |
| GatherMask | uint32_t | uint32_t | uint32_t | 32 |
| GatherMask | int32_t | uint32_t | int32_t | 32 |

**表5**  类型转换类指令理论性能汇总

<a name="table1712055602918"></a>
| 接口 | src数据类型 | dst数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- | --- |
| Cast | int4b_t | half | 128 |
| Cast | uint8_t | half | 128 |
| Cast | int8_t | half | 128 |
| Cast | int16_t | half | 128 |
| Cast | int16_t | float | 64 |
| Cast | half | int4b_t/int8_t/uint8_t/int16_t | 128 |
| Cast | half | int32_t/float | 64 |
| Cast | bfloat16_t | int32_t/float | 64 |
| Cast | int32_t | int16_t/half | 64 |
| Cast | int32_t | float | 64 |
| Cast | int32_t | int64_t | 32 |
| Cast | float | half | 64 |
| Cast | float | int16_t/bfloat16_t/int32_t/float | 64 |
| Cast | float | int64_t | 32 |

**表6**  归约计算类指令理论性能汇总

<a name="table76761043412"></a>
| 接口 | src数据类型 | dst数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- | --- |
| ReduceDataBlock | half | half | 128/7 |
| ReduceDataBlock | float | float | 64 |
| ReduceRepeat | half | half | 128/7 |
| ReduceRepeat | float | float | 64/7 |
| ReducePairElem | half | half | 128 |
| ReducePairElem | float | float | 64 |
| ReduceMax/ReduceMin/ReduceSum | - | - | 软仿指令，不涉及理论并行度 |

**表7**  数据排布转换类指令理论性能汇总

<a name="table2514311153610"></a>
| 接口 | src数据类型 | dst数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- | --- |
| Transpose | int8_t/uint8_t | int8_t/uint8_t | 256 |
| Transpose | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| Transpose | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |
| TransDataTo5HD | int8_t/uint8_t | int8_t/uint8_t | 256 |
| TransDataTo5HD | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| TransDataTo5HD | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |

**表8**  数据填充类指令理论性能汇总

<a name="table18675133833617"></a>
| 接口 | src数据类型 | dst数据类型 | 理论输入并行度（element/cycle） | 理论输出并行度（element/cycle） |
| --- | --- | --- | --- | --- |
| Brcb | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | 8 | 128 |
| Brcb | int32_t/uint32_t/float | int32_t/uint32_t/float | 8 | 64 |
| Duplicate | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | - | 128 |
| Duplicate | int32_t/uint32_t/float | int32_t/uint32_t/float | - | 64 |
| CreateVecIndex | int16_t/half | int16_t/half | - | 128 |
| CreateVecIndex | int32_t/float | int32_t/float | - | 64 |

**表9**  排序组合类指令理论性能汇总

<a name="table1361419514367"></a>
| 接口 | dst数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- |
| Sort32 | half | 2 |
| Sort32 | float | 2 |
| MrgSort | half | 2 |
| MrgSort | float | 2 |

**表10**  离散与聚合类指令理论性能汇总

<a name="table111588914379"></a>
| 接口 | src数据类型 | dst数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- | --- |
| Gather | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | 5 |
| Gatherb | uint16_t | uint16_t | 42 |
| Gatherb | uint32_t | uint32_t | 21 |

**表11**  掩码操作类指令理论性能汇总

<a name="table12905102516375"></a>
| 接口 | 输入数据类型 | 理论并行度（element/cycle） |
| --- | --- | --- |
| SetMaskCount | - | 1 |
| SetMaskNorm | - | 1 |
| SetVectorMask | uint64_t | 1 |
| SetVectorMask | int32_t | 1 |
| ResetMask | - | 1 |

**表12**  标量计算类指令理论性能汇总

<a name="table12906133883716"></a>
| 接口 | 输入数据类型 | 输出数据类型 | 理论并行度（cycle/instruction） |
| --- | --- | --- | --- |
| GetBitCount | uint64_t | int64_t | 1 |
| CountLeadingZero | uint64_t | int64_t | 1 |
| GetSFFValue | uint64_t | int64_t | 1 |
| CountBitsCntSameAsSignBit | int64_t | int64_t | 1 |
| Cast（float转half、int32_t） | float | half/int32_t | 1 |
| Cast（float转bfloat16_t） | - | - | 软仿指令，不涉及理论并行度 |
| Cast（bfloat16_t转float） | - | - | 软仿指令，不涉及理论并行度 |

**表13**  工具接口类指令理论性能汇总

<a name="table1843362415383"></a>
| 接口 | 理论并行度（cycle/instruction） |
| --- | --- |
| InitSocState | 软仿指令，不涉及理论并行度 |
| Async | 静态编译，无消耗 |
| GetBlockNum | 1 |
| GetBlockIdx（AIC） | 1 |
| GetBlockIdx（AIV） | 软仿指令，不涉及理论并行度 |
| GetTaskRatio | 1 |
| GetSubBlockNum | 1 |
| GetSubBlockIdx | 1 |
| GetDataBlockSizeInBytes | 静态编译，无消耗，与直接使用立即数行为一致。 |
| GetSystemCycle | 1 |
| GetProgramCounter | 软仿指令，不涉及理论并行度 |
| GetArchVersion | 软仿指令，不涉及理论并行度 |

