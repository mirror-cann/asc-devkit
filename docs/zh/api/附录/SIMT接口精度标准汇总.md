# SIMT接口精度标准汇总

本节按照数据类型和接口类型对SIMT浮点计算接口进行精度标准的分类汇总。对于非软仿接口，给出对应硬件指令的精度标准；对于软仿接口，由于软仿程度较低且使用到的硬件指令较少，给出其所用硬件指令的综合性精度标准。

## bf16数据类型计算API精度标准

### 数学库函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [hexp](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp-177.md) | - | 1-ulp，Not Support denormalized numbers |
| [htanh](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/htanh-176.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp2](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp2-178.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp10](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp10-179.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog-180.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog2](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog2-181.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog10](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog10-182.md) | - | 1-ulp，Not Support denormalized numbers |
| [hsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hsqrt-185.md) | - | 1-ulp，Not Support denormalized numbers |
| [hrsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrsqrt-186.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2tanh](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2tanh-242.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp-243.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp2-244.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp10](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp10-245.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log-246.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log2-247.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log10](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log10-248.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2sqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sqrt-251.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2rsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rsqrt-252.md) | - | 1-ulp，Not Support denormalized numbers |

### 算术函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [\_\_habs](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__habs-150.md) | - | 0-ulp, Support denormalized numbers |
| [\_\_hadd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hadd-152.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmul](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hmul-154.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdiv](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hdiv-155.md) | 1-ulp，Not Support denormalized numbers | - |
| [\_\_haddx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__haddx2-192.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmulx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hmulx2-194.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdivx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hdivx2-195.md) | 1-ulp，Not Support denormalized numbers | - |

## fp16数据类型计算API精度标准

### 数学库函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [hexp](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp.md) | 1-ulp | - |
| [hexp2](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp10](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog.md) | 1-ulp，Not Support denormalized numbers | - |
| [hlog2](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog10](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hsqrt](../SIMT-API/数学函数/half类型/half类型数学库函数/hsqrt.md) | 1-ulp，Not Support denormalized numbers | - |
| [hrsqrt](../SIMT-API/数学函数/half类型/half类型数学库函数/hrsqrt.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2tanh](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2tanh.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp2](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp10](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log2](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log10](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2sqrt](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2sqrt.md) | 1-ulp，Not Support denormalized numbers | - |
| [h2rsqrt](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2rsqrt.md) | - | 1-ulp，Not Support denormalized numbers |

### 算术函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [\_\_hadd](../SIMT-API/数学函数/half类型/half类型算术函数/__hadd.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmul](../SIMT-API/数学函数/half类型/half类型算术函数/__hmul.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdiv](../SIMT-API/数学函数/half类型/half类型算术函数/__hdiv.md) | 1-ulp，Not Support denormalized numbers | - |
| [\_\_haddx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__haddx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmulx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hmulx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdivx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hdivx2.md) | 1-ulp，Not Support denormalized numbers | - |

## fp32数据类型计算API精度标准

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [expf](../SIMT-API/数学函数/float类型数学库函数/expf.md) | 1-ulp，Not Support denormalized numbers | - |
| [tanhf](../SIMT-API/数学函数/float类型数学库函数/tanhf.md) | - | 1-ulp，Not Support denormalized numbers |
| [tanpif](../SIMT-API/数学函数/float类型数学库函数/tanpif.md) | - | 1-ulp，Not Support denormalized numbers |
| [atanf](../SIMT-API/数学函数/float类型数学库函数/atanf.md) | - | 1-ulp，Not Support denormalized numbers |
| [atan2f](../SIMT-API/数学函数/float类型数学库函数/atan2f.md) | - | 1-ulp，Not Support denormalized numbers |
| [atanhf](../SIMT-API/数学函数/float类型数学库函数/atanhf.md) | - | 1-ulp，Not Support denormalized numbers |
| [expm1f](../SIMT-API/数学函数/float类型数学库函数/expm1f.md) | - | 1-ulp，Not Support denormalized numbers |
| [logf](../SIMT-API/数学函数/float类型数学库函数/logf.md) | - | 1-ulp，Not Support denormalized numbers |
| [log2f](../SIMT-API/数学函数/float类型数学库函数/log2f.md) | - | 1-ulp，Not Support denormalized numbers |
| [log10f](../SIMT-API/数学函数/float类型数学库函数/log10f.md) | - | 1-ulp，Not Support denormalized numbers |
| [log1pf](../SIMT-API/数学函数/float类型数学库函数/log1pf.md) | - | 1-ulp，Not Support denormalized numbers |
| [cosf](../SIMT-API/数学函数/float类型数学库函数/cosf.md) | - | 1-ulp，Not Support denormalized numbers |
| [acoshf](../SIMT-API/数学函数/float类型数学库函数/acoshf.md) | - | 1-ulp，Not Support denormalized numbers |
| [sqrtf](../SIMT-API/数学函数/float类型数学库函数/sqrtf.md) | 1-ulp，Not Support denormalized numbers | - |
| [rsqrtf](../SIMT-API/数学函数/float类型数学库函数/rsqrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [cbrtf](../SIMT-API/数学函数/float类型数学库函数/cbrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [rcbrtf](../SIMT-API/数学函数/float类型数学库函数/rcbrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [remquof](../SIMT-API/数学函数/float类型数学库函数/remquof.md) | - | 1-ulp，Not Support denormalized numbers |
| [fdividef](../SIMT-API/数学函数/float类型数学库函数/fdividef.md) | 1-ulp，Not Support denormalized numbers | - |
