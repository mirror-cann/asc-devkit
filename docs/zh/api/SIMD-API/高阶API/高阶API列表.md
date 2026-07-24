# 高阶API列表

## 数据结构

| 类型 | 功能描述 |
| --- | --- |
| [TensorShape](数据结构/TensorShape.md) | 描述高阶API的Host侧Tiling接口使用的Tensor形状。 |
| [TensorDataType](数据结构/TensorDataType.md) | 描述高阶API的Host侧Tiling接口使用的Tensor数据类型。 |

## 数学计算

| 接口名 | 功能描述 |
| --- | --- |
| [Acos](数学计算/Acos接口/Acos.md) | 按元素做反余弦函数计算。 |
| [Acosh](数学计算/Acosh接口/Acosh.md) | 按元素做双曲反余弦函数计算。 |
| [Asin](数学计算/Asin接口/Asin.md) | 按元素做反正弦函数计算。 |
| [Asinh](数学计算/Asinh接口/Asinh.md) | 按元素做反双曲正弦函数计算。 |
| [Atan](数学计算/Atan接口/Atan.md) | 按元素做三角函数反正切运算。 |
| [Atanh](数学计算/Atanh接口/Atanh.md) | 按元素做反双曲正切余弦函数计算。 |
| [Axpy](数学计算/Axpy接口/Axpy-80.md) | 源操作数中每个元素与标量求积后和目的操作数中的对应元素相加。 |
| [Ceil](数学计算/Ceil接口/Ceil.md) | 获取大于或等于x的最小的整数值，即向正无穷取整操作。 |
| [ClampMax](数学计算/Clamp接口/ClampMax.md) | 将srcTensor中大于scalar的数替换为scalar，小于等于scalar的数保持不变，作为dstTensor输出。 |
| [ClampMin](数学计算/Clamp接口/ClampMin.md) | 将srcTensor中小于scalar的数替换为scalar，大于等于scalar的数保持不变，作为dstTensor输出。 |
| [Cos](数学计算/Cos接口/Cos.md) | 按元素做三角函数余弦运算。 |
| [Cosh](数学计算/Cosh接口/Cosh.md) | 按元素做双曲余弦函数计算。 |
| [CumSum](数学计算/CumSum接口/CumSum.md) | 对数据按行依次累加或按列依次累加。 |
| [Digamma](数学计算/Digamma接口/Digamma.md) | 按元素计算x的gamma函数的对数导数。 |
| [Erf](数学计算/Erf接口/Erf.md) | 按元素做误差函数计算，也称为高斯误差函数。 |
| [Erfc](数学计算/Erfc接口/Erfc.md) | 返回输入x的互补误差函数结果，积分区间为x到无穷大。 |
| [Exp](数学计算/Exp接口/Exp-81.md) | 按元素取自然指数。 |
| [Floor](数学计算/Floor接口/Floor.md) | 获取小于或等于x的最小的整数值，即向负无穷取整操作。 |
| [Fmod](数学计算/Fmod接口/Fmod.md) | 按元素计算两个浮点数相除后的余数。 |
| [Frac](数学计算/Frac接口/Frac.md) | 按元素做取小数计算。 |
| [Hypot](数学计算/Hypot接口/Hypot.md) | 按元素计算两个浮点数平方和的平方根。 |
| [IsFinite](数学计算/IsFinite接口/IsFinite.md) | 按元素判断输入的浮点数是否非NAN、非±INF。 |
| [Lgamma](数学计算/Lgamma接口/Lgamma.md) | 按元素计算x的gamma函数的绝对值并求自然对数。 |
| [Log](数学计算/Log接口/Log-79.md) | 按元素以e、2、10为底做对数运算。 |
| [Power](数学计算/Power接口/Power.md) | 实现按元素做幂运算功能。 |
| [Round](数学计算/Round接口/Round.md) | 将输入的元素四舍五入到最接近的整数。 |
| [Sign](数学计算/Sign接口/Sign.md) | 按元素执行Sign操作，Sign是指返回输入数据的符号。 |
| [Sin](数学计算/Sin接口/Sin.md) | 按元素做正弦函数计算。 |
| [Sinh](数学计算/Sinh接口/Sinh.md) | 按元素做双曲正弦函数计算。 |
| [Tan](数学计算/Tan接口/Tan.md) | 按元素做正切函数计算。 |
| [Tanh](数学计算/Tanh接口/Tanh.md) | 按元素做逻辑回归Tanh。 |
| [Trunc](数学计算/Trunc接口/Trunc.md) | 按元素做浮点数截断操作，即向零取整操作。 |
| [Xor](数学计算/Xor接口/Xor-82.md) | 按元素执行Xor（异或）运算。 |
| [Fma](数学计算/Fma接口/Fma接口.md) | 按元素计算两个输入相乘后与第三个输入相加的结果。 |
| [IsNan](数学计算/IsNan接口/IsNan接口.md) | 按元素判断输入的浮点数是否为nan。 |
| [IsInf](数学计算/IsInf接口/IsInf接口.md) | 按元素判断输入的浮点数是否为±INF。 |
| [Rint](数学计算/Rint接口/Rint接口.md) | 获取与输入数据最接近的整数。 |
| [SinCos](数学计算/SinCos接口/SinCos接口.md) | 按元素进行正弦计算和余弦计算，分别获得正弦和余弦的结果。 |
| [LogicalNot](数学计算/LogicalNot接口/LogicalNot.md) | 按元素进行取反操作。 |
| [LogicalAnd](数学计算/LogicalAnd接口/LogicalAnd.md) | 按元素进行与操作。 |
| [LogicalAnds](数学计算/LogicalAnds接口/LogicalAnds.md) | 输入矢量内的每个元素与标量进行与操作。 |
| [LogicalOr](数学计算/LogicalOr接口/LogicalOr.md) | 按元素进行或操作。 |
| [LogicalOrs](数学计算/LogicalOrs接口/LogicalOrs.md) | 输入矢量内的每个元素与标量进行或操作。 |
| [LogicalXor](数学计算/LogicalXor接口/LogicalXor.md) | 按元素进行逻辑异或操作。 |
| [BitwiseNot](数学计算/BitwiseNot接口/BitwiseNot.md) | 逐比特对输入进行取反。 |
| [BitwiseAnd](数学计算/BitwiseAnd接口/BitwiseAnd.md) | 逐比特对两个输入进行与操作。 |
| [BitwiseOr](数学计算/BitwiseOr接口/BitwiseOr.md) | 逐比特对两个输入进行或操作。 |
| [BitwiseXor](数学计算/BitwiseXor接口/BitwiseXor.md) | 逐比特对两个输入进行异或操作。 |
| [Where](数学计算/Where接口/Where.md) | 根据指定的条件，从两个源操作数中选择元素，生成目标操作数。 |

## 量化操作

| 接口名 | 功能描述 |
| --- | --- |
| [AntiQuantize](量化操作/AntiQuantize.md) | 按元素做伪量化计算，比如将int8_t数据类型伪量化为half数据类型。 |
| [AscendAntiQuant](量化操作/AscendAntiQuant.md) | 按元素做伪量化计算，比如将int8_t数据类型伪量化为half数据类型。 |
| [Dequantize](量化操作/Dequantize.md) | 按元素做反量化计算，比如将int32_t数据类型反量化为half/float等数据类型。 |
| [AscendDequant](量化操作/AscendDequant.md) | 按元素做反量化计算，比如将int32_t数据类型反量化为half/float等数据类型。 |
| [Quantize](量化操作/Quantize.md) | 按元素做量化计算，比如将half/float数据类型量化为int8_t数据类型。 |
| [AscendQuant](量化操作/AscendQuant.md) | 按元素做量化计算，比如将half/float数据类型量化为int8_t数据类型。 |

##  归一化操作

| 接口名 | 功能描述 |
| --- | --- |
| [BatchNorm](归一化操作/BatchNorm.md) | 对于每个batch中的样本，对其输入的每个特征在batch的维度上进行归一化。 |
| [DeepNorm](归一化操作/DeepNorm.md) | 在深层神经网络训练过程中，可以替代LayerNorm的一种归一化方法。 |
| [GroupNorm](归一化操作/GroupNorm.md) | 将输入的C维度分为groupNum组，对每一组数据进行标准化。 |
| [LayerNorm](归一化操作/LayerNorm.md) | 将输入数据收敛到[0, 1]之间，可以规范网络层输入输出数据分布的一种归一化方法。 |
| [LayerNorm-Tiling](归一化操作/LayerNorm-Tiling.md) | LayerNorm的Tiling参数配置接口，用于获取LayerNorm kernel计算时所需的Tiling参数。 |
| [LayerNormGrad](归一化操作/LayerNormGrad.md) | 用于计算LayerNorm的反向传播梯度。 |
| [LayerNormGrad-Tiling](归一化操作/LayerNormGrad-Tiling.md) | LayerNormGrad的Tiling参数配置接口，用于设置反向传播梯度计算所需的分片参数。 |
| [LayerNormGradBeta](归一化操作/LayerNormGradBeta.md) | 用于获取反向beta/gmma的数值，和LayerNormGrad共同输出pdx, gmma和beta。 |
| [LayerNormGradBeta-Tiling](归一化操作/LayerNormGradBeta-Tiling.md) | LayerNormGradBeta的Tiling参数配置接口。 |
| [Normalize](归一化操作/Normalize.md) | [LayerNorm](归一化操作/LayerNorm.md)中，已知均值和方差，计算shape为[A，R]的输入数据的标准差的倒数rstd和归一化输出y。 |
| [RmsNorm](归一化操作/RmsNorm.md) | 实现对shape大小为[B，S，H]的输入数据的RmsNorm归一化。 |
| [WelfordUpdate](归一化操作/WelfordUpdate.md) | 实现Welford算法的前处理。 |
| [WelfordFinalize](归一化操作/WelfordFinalize.md) | 实现Welford算法的后处理。 |

##  激活函数

| 接口名 | 功能描述 |
| --- | --- |
| [AdjustSoftMaxRes](激活函数/SoftMax接口/AdjustSoftMaxRes.md) | 用于对SoftMax相关计算结果做后处理，调整SoftMax的计算结果为指定的值。 |
| [FasterGelu](激活函数/Gelu接口/FasterGelu.md) | FastGelu化简版本的一种激活函数。 |
| [FasterGeluV2](激活函数/Gelu接口/FasterGeluV2.md) | 实现FastGeluV2版本的一种激活函数。 |
| [GeGLU](激活函数/GeGLU接口/GeGLU.md) | 采用GeLU作为激活函数的GLU变体。 |
| [Gelu](激活函数/Gelu接口/Gelu.md) | GELU是一个重要的激活函数，其灵感来源于relu和dropout，在激活中引入了随机正则的思想。 |
| [LogSoftMax](激活函数/LogSoftMax接口/LogSoftMax.md) | 对输入tensor做LogSoftmax计算。 |
| [ReGlu](激活函数/ReGlu接口/ReGlu.md) | 一种GLU变体，使用Relu作为激活函数。 |
| [Sigmoid](激活函数/Sigmoid接口/Sigmoid.md) | 按元素做逻辑回归Sigmoid。 |
| [Silu](激活函数/Silu接口/Silu.md) | 按元素做Silu运算。 |
| [SimpleSoftMax](激活函数/SoftMax接口/SimpleSoftMax.md) | 使用计算好的sum和max数据对输入tensor做softmax计算。 |
| [SoftMax](激活函数/SoftMax接口/SoftMax.md) | 对输入tensor按行做Softmax计算。 |
| [SoftmaxFlash](激活函数/SoftMax接口/SoftmaxFlash.md) | SoftMax增强版本，除了可以对输入tensor做softmaxflash计算，还可以根据上一次softmax计算的sum和max来更新本次的softmax计算结果。 |
| [SoftmaxFlashV2](激活函数/SoftMax接口/SoftmaxFlashV2.md) | SoftmaxFlash增强版本，对应FlashAttention-2算法。 |
| [SoftmaxFlashV3](激活函数/SoftMax接口/SoftmaxFlashV3.md) | SoftmaxFlash增强版本，对应Softmax PASA算法。 |
| [SoftmaxGrad](激活函数/SoftMax接口/SoftmaxGrad.md) | 对输入tensor做grad反向计算的一种方法。 |
| [SoftmaxGradFront](激活函数/SoftMax接口/SoftmaxGradFront.md) | 对输入tensor做grad反向计算的一种方法。 |
| [SwiGLU](激活函数/SwiGLU接口/SwiGLU.md) | 采用Swish作为激活函数的GLU变体。 |
| [Swish](激活函数/Swish接口/Swish.md) | 神经网络中的Swish激活函数。 |

##  归约操作

| 接口名 | 功能描述 |
| --- | --- |
| [Sum](归约操作/Sum接口/Sum.md) | 获取最后一个维度的元素总和。 |
| [Mean](归约操作/Mean接口/Mean.md) | 根据最后一轴的方向对各元素求平均值。 |
| [ReduceXorSum](归约操作/ReduceXorSum接口/ReduceXorSum.md) | 按照元素执行Xor（按位异或）运算，并将计算结果ReduceSum求和。 |
| [ReduceSum](归约操作/ReduceSum接口/ReduceSum-90.md) | 对一个多维向量按照指定的维度进行数据累加。 |
| [ReduceMean](归约操作/ReduceMean接口/ReduceMean.md) | 对一个多维向量按照指定的维度求平均值。 |
| [ReduceMax](归约操作/ReduceMax接口/ReduceMax-91.md) | 对一个多维向量在指定的维度求最大值。 |
| [ReduceMin](归约操作/ReduceMin接口/ReduceMin-92.md) | 对一个多维向量在指定的维度求最小值。 |
| [ReduceAny](归约操作/ReduceAny接口/ReduceAny.md) | 对一个多维向量在指定的维度求逻辑或。 |
| [ReduceAll](归约操作/ReduceAll接口/ReduceAll.md) | 对一个多维向量在指定的维度求逻辑与。 |
| [ReduceProd](归约操作/ReduceProd接口/ReduceProd.md) | 对一个多维向量在指定的维度求积。 |

##  排序操作

| 接口名 | 功能描述 |
| --- | --- |
| [TopK](排序操作/TopK.md) | 获取最后一个维度的前k个最大值或最小值及其对应的索引。 |
| [Concat](排序操作/Concat.md) | 对数据进行预处理，将要排序的源操作数srcLocal一一对应的合入目标数据concatLocal中，数据预处理完后，可以进行Sort。 |
| [Extract](排序操作/Extract.md) | 处理Sort的结果数据，输出排序后的value和index。 |
| [Sort](排序操作/Sort.md) | 排序函数，按照数值大小进行降序排序。 |
| [MrgSort](排序操作/MrgSort-93.md) | 将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。 |

##  数据过滤

| 接口名 | 功能描述 |
| --- | --- |
| [Select](数据过滤/Select-95.md) | 给定两个源操作数src0和src1，根据maskTensor相应位置的值（非bit位）选取元素，得到目的操作数dst。 |
| [DropOut](数据过滤/DropOut.md) | 提供根据MaskTensor对源操作数进行过滤的功能，得到目的操作数。 |

##  张量变换

| 接口名 | 功能描述 |
| --- | --- |
| [Transpose](张量变换/Transpose-96.md) | 对输入数据进行数据排布及Reshape操作。 |
| [TransData](张量变换/TransData.md) | 将输入数据的排布格式转换为目标排布格式。 |
| [Broadcast](张量变换/Broadcast.md) | 将输入按照输出shape进行广播。 |
| [Pad](张量变换/Pad.md) | 对height * width的二维Tensor在width方向上pad到32B对齐。 |
| [UnPad](张量变换/UnPad.md) | 对height * width的二维Tensor在width方向上进行unpad。 |
| [Fill](张量变换/Fill-97.md) | 将Global Memory上的数据初始化为指定值。 |

##  索引计算

| 接口名 | 功能描述 |
| --- | --- |
| [Arange](索引计算/Arange-94.md) | 给定起始值，等差值和长度，返回一个等差数列。 |

##  矩阵计算

| 接口名 | 功能描述 |
| --- | --- |
| [Matmul](矩阵计算/Matmul-Kernel侧接口/Matmul-Kernel侧接口.md) | Matmul矩阵乘法的运算。 |

##  HCCL通信类

| 接口名 | 功能描述 |
| --- | --- |
| [HCCL通信类](HCCL通信类/HCCL通信类.md) | 在AI Core侧编排集合通信任务。 |

##  卷积计算

| 接口名 | 功能描述 |
| --- | --- |
| [Conv3D](卷积计算/Conv3D-Kernel侧接口/Conv3D-Kernel侧接口.md) | 3维卷积正向矩阵运算。 |
| [Conv3DBackpropInput](卷积计算/Conv3DBackpropInput-Kernel侧接口/Conv3DBackpropInput-Kernel侧接口.md) | 卷积的反向运算，求解特征矩阵的反向传播误差。 |
| [Conv3DBackpropFilter](卷积计算/Conv3DBackpropFilter-Kernel侧接口/Conv3DBackpropFilter-Kernel侧接口.md) | 卷积的反向运算，求解权重的反向传播误差。 |

##  随机函数

| 接口名 | 功能描述 |
| --- | --- |
| [PhiloxRandom](随机函数/PhiloxRandom.md) | 基于Philox随机数生成算法，给定随机数种子，生成若干的随机数。 |
