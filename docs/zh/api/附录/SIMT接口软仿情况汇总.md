# SIMT接口软仿情况汇总

本节按照SIMT API文档所在目录分级汇总各接口的软仿情况。

非软仿接口是指使用单一硬件指令完成操作的接口；软仿接口是指由编译器实现，或由上层接口通过拼接多条硬件指令实现功能的接口。

## Warp函数

### LaneID类函数

| API名 | API是否软仿 |
| --- | --- |
| [laneid](../SIMT-API/Warp函数/Lane-ID类函数/laneid.md) | 否 |
| [lanemask_eq](../SIMT-API/Warp函数/Lane-ID类函数/lanemask_eq.md) | 否 |
| [lanemask_ge](../SIMT-API/Warp函数/Lane-ID类函数/lanemask_ge.md) | 否 |
| [lanemask_gt](../SIMT-API/Warp函数/Lane-ID类函数/lanemask_gt.md) | 否 |
| [lanemask_le](../SIMT-API/Warp函数/Lane-ID类函数/lanemask_le.md) | 否 |
| [lanemask_lt](../SIMT-API/Warp函数/Lane-ID类函数/lanemask_lt.md) | 否 |

### Warp Reduce类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_reduce_add](../SIMT-API/Warp函数/Warp-Reduce类函数/asc_reduce_add.md) | 否 |
| [asc_reduce_max](../SIMT-API/Warp函数/Warp-Reduce类函数/asc_reduce_max.md) | 否 |
| [asc_reduce_min](../SIMT-API/Warp函数/Warp-Reduce类函数/asc_reduce_min.md) | 否 |

### Warp Shfl类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_shfl](../SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl.md) | 否 |
| [asc_shfl_down](../SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_down.md) | 否 |
| [asc_shfl_up](../SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_up.md) | 否 |
| [asc_shfl_xor](../SIMT-API/Warp函数/Warp-Shfl类函数/asc_shfl_xor.md) | 否 |

### Warp Vote类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_activemask](../SIMT-API/Warp函数/Warp-Vote类函数/asc_activemask.md) | 否 |
| [asc_all](../SIMT-API/Warp函数/Warp-Vote类函数/asc_all.md) | 否 |
| [asc_any](../SIMT-API/Warp函数/Warp-Vote类函数/asc_any.md) | 否 |
| [asc_ballot](../SIMT-API/Warp函数/Warp-Vote类函数/asc_ballot.md) | 否 |

## 协作组

| API名 | API是否软仿 |
| --- | --- |
| [binary_partition](../SIMT-API/协作组/binary_partition.md) | 是 |
| [tiled_partition](../SIMT-API/协作组/tiled_partition.md) | 是 |

### coalesced_group

| API名 | API是否软仿 |
| --- | --- |
| [all](../SIMT-API/协作组/coalesced_group/all.md) | 是 |
| [any](../SIMT-API/协作组/coalesced_group/any.md) | 是 |
| [ballot](../SIMT-API/协作组/coalesced_group/ballot.md) | 是 |
| [coalesced_group构造函数](../SIMT-API/协作组/coalesced_group/coalesced_group构造函数.md) | 是 |
| [meta_group_rank](../SIMT-API/协作组/coalesced_group/meta_group_rank.md) | 是 |
| [meta_group_size](../SIMT-API/协作组/coalesced_group/meta_group_size.md) | 是 |
| [num_threads](../SIMT-API/协作组/coalesced_group/num_threads.md) | 是 |
| [shfl](../SIMT-API/协作组/coalesced_group/shfl.md) | 是 |
| [shfl_down](../SIMT-API/协作组/coalesced_group/shfl_down.md) | 是 |
| [shfl_up](../SIMT-API/协作组/coalesced_group/shfl_up.md) | 是 |
| [size](../SIMT-API/协作组/coalesced_group/size.md) | 是 |
| [sync](../SIMT-API/协作组/coalesced_group/sync.md) | 是 |
| [thread_rank](../SIMT-API/协作组/coalesced_group/thread_rank.md) | 是 |

### thread_block

| API名 | API是否软仿 |
| --- | --- |
| [dim_threads](../SIMT-API/协作组/thread_block/dim_threads.md) | 是 |
| [group_dim](../SIMT-API/协作组/thread_block/group_dim.md) | 是 |
| [group_index](../SIMT-API/协作组/thread_block/group_index.md) | 是 |
| [num_threads](../SIMT-API/协作组/thread_block/num_threads.md) | 是 |
| [size](../SIMT-API/协作组/thread_block/size.md) | 是 |
| [sync](../SIMT-API/协作组/thread_block/sync.md) | 是 |
| [thread_block构造函数](../SIMT-API/协作组/thread_block/thread_block构造函数.md) | 是 |
| [thread_index](../SIMT-API/协作组/thread_block/thread_index.md) | 是 |
| [thread_rank](../SIMT-API/协作组/thread_block/thread_rank.md) | 是 |

### thread_block_tile

| API名 | API是否软仿 |
| --- | --- |
| [all](../SIMT-API/协作组/thread_block_tile/all.md) | 是 |
| [any](../SIMT-API/协作组/thread_block_tile/any.md) | 是 |
| [ballot](../SIMT-API/协作组/thread_block_tile/ballot.md) | 是 |
| [meta_group_rank](../SIMT-API/协作组/thread_block_tile/meta_group_rank.md) | 是 |
| [meta_group_size](../SIMT-API/协作组/thread_block_tile/meta_group_size.md) | 是 |
| [num_threads](../SIMT-API/协作组/thread_block_tile/num_threads.md) | 是 |
| [shfl](../SIMT-API/协作组/thread_block_tile/shfl.md) | 是 |
| [shfl_down](../SIMT-API/协作组/thread_block_tile/shfl_down.md) | 是 |
| [shfl_up](../SIMT-API/协作组/thread_block_tile/shfl_up.md) | 是 |
| [shfl_xor](../SIMT-API/协作组/thread_block_tile/shfl_xor.md) | 是 |
| [size](../SIMT-API/协作组/thread_block_tile/size.md) | 是 |
| [sync](../SIMT-API/协作组/thread_block_tile/sync.md) | 是 |
| [thread_block_tile构造函数](../SIMT-API/协作组/thread_block_tile/thread_block_tile构造函数.md) | 是 |
| [thread_rank](../SIMT-API/协作组/thread_block_tile/thread_rank.md) | 是 |

## 原子操作

下表汇总原子操作接口在SIMD与SIMT编程场景下的软仿情况。SIMT编程场景中，接口形参不携带UB/GM地址空间标识，编译器需先完成入参指针到UB/GM地址空间指针的转换，再完成对应的原子操作实现。

| API名 | 非软仿数据类型 | 软仿数据类型 |
| --- | --- | --- |
| [asc_atomic_add](../SIMT-API/原子操作/asc_atomic_add.md) | int32_t、uint32_t、int64_t、uint64_t、half、bfloat16_t | float、half2、bfloat16x2_t |
| [asc_atomic_and](../SIMT-API/原子操作/asc_atomic_and.md) | - | 所有数据类型 |
| [asc_atomic_cas](../SIMT-API/原子操作/asc_atomic_cas.md) | uint32_t、uint64_t | int32_t、int64_t、float、half、bfloat16_t、half2、bfloat16x2_t |
| [asc_atomic_exch](../SIMT-API/原子操作/asc_atomic_exch.md) | uint32_t、uint64_t | int32_t、int64_t、float、half、bfloat16_t、half2、bfloat16x2_t |
| [asc_atomic_dec](../SIMT-API/原子操作/asc_atomic_dec.md) | - | 所有数据类型 |
| [asc_atomic_inc](../SIMT-API/原子操作/asc_atomic_inc.md) | - | 所有数据类型 |
| [asc_atomic_max](../SIMT-API/原子操作/asc_atomic_max.md) | uint32_t、int32_t、int64_t、uint64_t、half、bfloat16_t | float、half2、bfloat16x2_t |
| [asc_atomic_min](../SIMT-API/原子操作/asc_atomic_min.md) | uint32_t、int32_t、int64_t、uint64_t、half、bfloat16_t | float、half2、bfloat16x2_t |
| [asc_atomic_or](../SIMT-API/原子操作/asc_atomic_or.md) | - | 所有数据类型 |
| [asc_atomic_sub](../SIMT-API/原子操作/asc_atomic_sub.md) | - | 所有数据类型 |
| [asc_atomic_xor](../SIMT-API/原子操作/asc_atomic_xor.md) | - | 所有数据类型 |

## 同步与内存栅栏

### 内存栅栏接口

| API名 | API是否软仿 |
| --- | --- |
| [asc_threadfence](../SIMT-API/同步与内存栅栏/内存栅栏接口/asc_threadfence.md) | 否 |
| [asc_threadfence_block](../SIMT-API/同步与内存栅栏/内存栅栏接口/asc_threadfence_block.md) | 否 |

### 同步接口

| API名 | API是否软仿 |
| --- | --- |
| [asc_syncthreads](../SIMT-API/同步与内存栅栏/同步接口/asc_syncthreads.md) | 否 |

## 地址空间谓词函数

| API名 | API是否软仿 |
| --- | --- |
| [__isGlobal](../SIMT-API/地址空间谓词函数/__isGlobal.md) | 否 |
| [__isLocal](../SIMT-API/地址空间谓词函数/__isLocal.md) | 否 |
| [__isUbuf](../SIMT-API/地址空间谓词函数/__isUbuf.md) | 否 |

## 地址空间转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__cvta_generic_to_global](../SIMT-API/地址空间转换函数/__cvta_generic_to_global.md) | 否 |
| [__cvta_generic_to_local](../SIMT-API/地址空间转换函数/__cvta_generic_to_local.md) | 否 |
| [__cvta_generic_to_ubuf](../SIMT-API/地址空间转换函数/__cvta_generic_to_ubuf.md) | 否 |
| [__cvta_global_to_generic](../SIMT-API/地址空间转换函数/__cvta_global_to_generic.md) | 否 |
| [__cvta_local_to_generic](../SIMT-API/地址空间转换函数/__cvta_local_to_generic.md) | 否 |
| [__cvta_ubuf_to_generic](../SIMT-API/地址空间转换函数/__cvta_ubuf_to_generic.md) | 否 |

## 数学函数

### bfloat16类型

#### bfloat16x2类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [h2ceil](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2ceil-256.md) | 否 |
| [h2cos](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2cos-249.md) | 是 |
| [h2exp](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp-243.md) | 是 |
| [h2exp10](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp10-245.md) | 是 |
| [h2exp2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp2-244.md) | 是 |
| [h2floor](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2floor-255.md) | 否 |
| [h2log](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log-246.md) | 是 |
| [h2log10](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log10-248.md) | 是 |
| [h2log2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log2-247.md) | 是 |
| [h2rcp](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rcp-253.md) | 是 |
| [h2rint](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rint-254.md) | 否 |
| [h2rsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rsqrt-252.md) | 是 |
| [h2sin](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sin-250.md) | 是 |
| [h2sqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sqrt-251.md) | 是 |
| [h2tanh](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2tanh-242.md) | 是 |
| [h2trunc](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2trunc-257.md) | 是 |

#### bfloat16x2类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__hbequx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbequx2-207.md) | 是 |
| [__hbeqx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbeqx2-201.md) | 是 |
| [__hbgeux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgeux2-210.md) | 是 |
| [__hbgex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgex2-204.md) | 是 |
| [__hbgtux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtux2-212.md) | 是 |
| [__hbgtx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtx2-206.md) | 是 |
| [__hbleux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbleux2-209.md) | 是 |
| [__hblex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hblex2-203.md) | 是 |
| [__hbltux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltux2-211.md) | 是 |
| [__hbltx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltx2-205.md) | 是 |
| [__hbneux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbneux2-208.md) | 是 |
| [__hbnex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbnex2-202.md) | 是 |
| [__hequx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2-219.md) | 是 |
| [__hequx2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2_mask-231.md) | 是 |
| [__heqx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2-213.md) | 是 |
| [__heqx2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2_mask-225.md) | 是 |
| [__hgeux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2-222.md) | 是 |
| [__hgeux2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2_mask-234.md) | 是 |
| [__hgex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2-216.md) | 是 |
| [__hgex2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2_mask-228.md) | 是 |
| [__hgtux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2-224.md) | 是 |
| [__hgtux2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2_mask-236.md) | 是 |
| [__hgtx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2-218.md) | 是 |
| [__hgtx2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2_mask-230.md) | 是 |
| [__hleux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2-221.md) | 是 |
| [__hleux2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2_mask-233.md) | 是 |
| [__hlex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2-215.md) | 是 |
| [__hlex2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2_mask-227.md) | 是 |
| [__hltux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2-223.md) | 是 |
| [__hltux2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2_mask-235.md) | 是 |
| [__hltx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2-217.md) | 是 |
| [__hltx2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2_mask-229.md) | 是 |
| [__hmaxx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2-238.md) | 否 |
| [__hmaxx2_nan](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2_nan-239.md) | 是 |
| [__hminx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2-240.md) | 否 |
| [__hminx2_nan](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2_nan-241.md) | 是 |
| [__hneux2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2-220.md) | 是 |
| [__hneux2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2_mask-232.md) | 是 |
| [__hnex2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2-214.md) | 是 |
| [__hnex2_mask](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2_mask-226.md) | 是 |
| [__isnanx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型比较函数/__isnanx2-237.md) | 是 |

#### bfloat16x2类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habsx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__habsx2-196.md) | 否 |
| [__haddx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__haddx2-192.md) | 否 |
| [__hcmadd](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hcmadd-200.md) | 是 |
| [__hdivx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hdivx2-195.md) | 否 |
| [__hfmax2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2-197.md) | 否 |
| [__hfmax2_relu](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2_relu-199.md) | 是 |
| [__hmulx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hmulx2-194.md) | 否 |
| [__hnegx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hnegx2-198.md) | 否 |
| [__hsubx2](../SIMT-API/数学函数/bfloat16类型/bfloat16x2类型算术函数/__hsubx2-193.md) | 否 |

#### bfloat16类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [hceil](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hceil-190.md) | 否 |
| [hcos](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hcos-183.md) | 是 |
| [hexp](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp-177.md) | 是 |
| [hexp10](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp10-179.md) | 是 |
| [hexp2](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hexp2-178.md) | 是 |
| [hfloor](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hfloor-189.md) | 否 |
| [hlog](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog-180.md) | 是 |
| [hlog10](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog10-182.md) | 是 |
| [hlog2](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hlog2-181.md) | 是 |
| [hrcp](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrcp-187.md) | 是 |
| [hrint](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrint-188.md) | 否 |
| [hrsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hrsqrt-186.md) | 是 |
| [hsin](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hsin-184.md) | 是 |
| [hsqrt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/hsqrt-185.md) | 是 |
| [htanh](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/htanh-176.md) | 是 |
| [htrunc](../SIMT-API/数学函数/bfloat16类型/bfloat16类型数学库函数/htrunc-191.md) | 是 |

#### bfloat16类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__heq](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__heq-162.md) | 否 |
| [__hequ](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hequ-168.md) | 否 |
| [__hge](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hge-165.md) | 否 |
| [__hgeu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgeu-171.md) | 否 |
| [__hgt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgt-167.md) | 否 |
| [__hgtu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hgtu-173.md) | 否 |
| [__hisinf](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hisinf-161.md) | 否 |
| [__hisnan](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hisnan-160.md) | 否 |
| [__hle](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hle-164.md) | 否 |
| [__hleu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hleu-170.md) | 否 |
| [__hlt](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hlt-166.md) | 否 |
| [__hltu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hltu-172.md) | 否 |
| [__hmax](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmax-158.md) | 是 |
| [__hmax_nan](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmax_nan-174.md) | 是 |
| [__hmin](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmin-159.md) | 是 |
| [__hmin_nan](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hmin_nan-175.md) | 是 |
| [__hne](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hne-163.md) | 否 |
| [__hneu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型比较函数/__hneu-169.md) | 否 |

#### bfloat16类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habs](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__habs-150.md) | 是 |
| [__hadd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hadd-152.md) | 否 |
| [__hdiv](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hdiv-155.md) | 否 |
| [__hfma](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hfma-151.md) | 否 |
| [__hfma_relu](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hfma_relu-157.md) | 是 |
| [__hmul](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hmul-154.md) | 否 |
| [__hneg](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hneg-156.md) | 否 |
| [__hsub](../SIMT-API/数学函数/bfloat16类型/bfloat16类型算术函数/__hsub-153.md) | 否 |

#### bfloat16类型精度转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__bfloat1622float2](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat1622float2.md) | 是 |
| [__bfloat162bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat162.md) | 是 |
| [__bfloat162bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rd.md) | 否 |
| [__bfloat162bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rn.md) | 否 |
| [__bfloat162bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rna.md) | 否 |
| [__bfloat162bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_ru.md) | 否 |
| [__bfloat162bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rz.md) | 否 |
| [__bfloat162float](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162float.md) | 是 |
| [__bfloat162int_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rd.md) | 否 |
| [__bfloat162int_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rn.md) | 否 |
| [__bfloat162int_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rna.md) | 否 |
| [__bfloat162int_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_ru.md) | 否 |
| [__bfloat162int_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rz.md) | 否 |
| [__bfloat162ll_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rd.md) | 是 |
| [__bfloat162ll_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rn.md) | 是 |
| [__bfloat162ll_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rna.md) | 是 |
| [__bfloat162ll_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_ru.md) | 是 |
| [__bfloat162ll_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rz.md) | 是 |
| [__bfloat162uint_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rd.md) | 否 |
| [__bfloat162uint_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rn.md) | 否 |
| [__bfloat162uint_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rna.md) | 否 |
| [__bfloat162uint_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_ru.md) | 否 |
| [__bfloat162uint_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rz.md) | 否 |
| [__bfloat162ull_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rd.md) | 是 |
| [__bfloat162ull_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rn.md) | 是 |
| [__bfloat162ull_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rna.md) | 是 |
| [__bfloat162ull_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_ru.md) | 是 |
| [__bfloat162ull_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rz.md) | 是 |
| [__float22bfloat162_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd.md) | 否 |
| [__float22bfloat162_rd_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd_sat.md) | 否 |
| [__float22bfloat162_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn.md) | 否 |
| [__float22bfloat162_rn_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn_sat.md) | 否 |
| [__float22bfloat162_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna.md) | 否 |
| [__float22bfloat162_rna_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna_sat.md) | 否 |
| [__float22bfloat162_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru.md) | 否 |
| [__float22bfloat162_ru_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru_sat.md) | 否 |
| [__float22bfloat162_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz.md) | 否 |
| [__float22bfloat162_rz_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz_sat.md) | 否 |
| [__float2bfloat16](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16.md) | 否 |
| [__float2bfloat162_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat162_rn.md) | 是 |
| [__float2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd.md) | 否 |
| [__float2bfloat16_rd_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd_sat.md) | 否 |
| [__float2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn.md) | 否 |
| [__float2bfloat16_rn_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn_sat.md) | 否 |
| [__float2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna.md) | 否 |
| [__float2bfloat16_rna_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna_sat.md) | 否 |
| [__float2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru.md) | 否 |
| [__float2bfloat16_ru_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru_sat.md) | 否 |
| [__float2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz.md) | 否 |
| [__float2bfloat16_rz_sat](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz_sat.md) | 否 |
| [__floats2bfloat162_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__floats2bfloat162_rn.md) | 是 |
| [__half2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rd.md) | 否 |
| [__half2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rn.md) | 否 |
| [__half2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rna.md) | 否 |
| [__half2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_ru.md) | 否 |
| [__half2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rz.md) | 否 |
| [__halves2bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__halves2bfloat162.md) | 是 |
| [__high2bfloat16](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat16.md) | 否 |
| [__high2bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat162.md) | 是 |
| [__high2float](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2float.md) | 否 |
| [__highs2bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__highs2bfloat162.md) | 是 |
| [__int2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rd.md) | 否 |
| [__int2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rn.md) | 否 |
| [__int2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rna.md) | 否 |
| [__int2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_ru.md) | 否 |
| [__int2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rz.md) | 否 |
| [__ll2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rd.md) | 是 |
| [__ll2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rn.md) | 是 |
| [__ll2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rna.md) | 是 |
| [__ll2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_ru.md) | 是 |
| [__ll2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rz.md) | 是 |
| [__low2bfloat16](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat16.md) | 否 |
| [__low2bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat162.md) | 是 |
| [__low2float](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2float.md) | 否 |
| [__lowhigh2highlow](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__lowhigh2highlow.md) | 是 |
| [__lows2bfloat162](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__lows2bfloat162.md) | 是 |
| [__uint2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rd.md) | 否 |
| [__uint2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rn.md) | 否 |
| [__uint2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rna.md) | 否 |
| [__uint2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_ru.md) | 否 |
| [__uint2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rz.md) | 否 |
| [__ull2bfloat16_rd](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rd.md) | 是 |
| [__ull2bfloat16_rn](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rn.md) | 是 |
| [__ull2bfloat16_rna](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rna.md) | 是 |
| [__ull2bfloat16_ru](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_ru.md) | 是 |
| [__ull2bfloat16_rz](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rz.md) | 是 |
| [__ushort_as_bfloat16](../SIMT-API/数学函数/bfloat16类型/bfloat16类型精度转换函数/__ushort_as_bfloat16.md) | 是 |

### float类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [__fdividef](../SIMT-API/数学函数/float类型数学库函数/__fdividef.md) | 否 |
| [__saturatef](../SIMT-API/数学函数/float类型数学库函数/__saturatef.md) | 是 |
| [acosf](../SIMT-API/数学函数/float类型数学库函数/acosf.md) | 是 |
| [acoshf](../SIMT-API/数学函数/float类型数学库函数/acoshf.md) | 是 |
| [asinf](../SIMT-API/数学函数/float类型数学库函数/asinf.md) | 是 |
| [asinhf](../SIMT-API/数学函数/float类型数学库函数/asinhf.md) | 是 |
| [atan2f](../SIMT-API/数学函数/float类型数学库函数/atan2f.md) | 是 |
| [atanf](../SIMT-API/数学函数/float类型数学库函数/atanf.md) | 是 |
| [atanhf](../SIMT-API/数学函数/float类型数学库函数/atanhf.md) | 是 |
| [cbrtf](../SIMT-API/数学函数/float类型数学库函数/cbrtf.md) | 是 |
| [ceilf](../SIMT-API/数学函数/float类型数学库函数/ceilf.md) | 否 |
| [copysignf](../SIMT-API/数学函数/float类型数学库函数/copysignf.md) | 是 |
| [cosf](../SIMT-API/数学函数/float类型数学库函数/cosf.md) | 是 |
| [coshf](../SIMT-API/数学函数/float类型数学库函数/coshf.md) | 是 |
| [cospif](../SIMT-API/数学函数/float类型数学库函数/cospif.md) | 是 |
| [cyl_bessel_i0f](../SIMT-API/数学函数/float类型数学库函数/cyl_bessel_i0f.md) | 是 |
| [cyl_bessel_i1f](../SIMT-API/数学函数/float类型数学库函数/cyl_bessel_i1f.md) | 是 |
| [erfcf](../SIMT-API/数学函数/float类型数学库函数/erfcf.md) | 是 |
| [erfcinvf](../SIMT-API/数学函数/float类型数学库函数/erfcinvf.md) | 是 |
| [erfcxf](../SIMT-API/数学函数/float类型数学库函数/erfcxf.md) | 是 |
| [erff](../SIMT-API/数学函数/float类型数学库函数/erff.md) | 是 |
| [erfinvf](../SIMT-API/数学函数/float类型数学库函数/erfinvf.md) | 是 |
| [exp10f](../SIMT-API/数学函数/float类型数学库函数/exp10f.md) | 否 |
| [exp2f](../SIMT-API/数学函数/float类型数学库函数/exp2f.md) | 否 |
| [expf](../SIMT-API/数学函数/float类型数学库函数/expf.md) | 否 |
| [expm1f](../SIMT-API/数学函数/float类型数学库函数/expm1f.md) | 是 |
| [fabsf](../SIMT-API/数学函数/float类型数学库函数/fabsf.md) | 否 |
| [fdimf](../SIMT-API/数学函数/float类型数学库函数/fdimf.md) | 是 |
| [fdividef](../SIMT-API/数学函数/float类型数学库函数/fdividef.md) | 是 |
| [floorf](../SIMT-API/数学函数/float类型数学库函数/floorf.md) | 否 |
| [fmaf](../SIMT-API/数学函数/float类型数学库函数/fmaf.md) | 否 |
| [fmaxf](../SIMT-API/数学函数/float类型数学库函数/fmaxf.md) | 是 |
| [fminf](../SIMT-API/数学函数/float类型数学库函数/fminf.md) | 是 |
| [fmodf](../SIMT-API/数学函数/float类型数学库函数/fmodf.md) | 是 |
| [frexpf](../SIMT-API/数学函数/float类型数学库函数/frexpf.md) | 是 |
| [hypotf](../SIMT-API/数学函数/float类型数学库函数/hypotf.md) | 是 |
| [ilogbf](../SIMT-API/数学函数/float类型数学库函数/ilogbf.md) | 是 |
| [isfinite](../SIMT-API/数学函数/float类型数学库函数/isfinite1.md) | 否 |
| [isinf](../SIMT-API/数学函数/float类型数学库函数/isinf1.md) | 否 |
| [isnan](../SIMT-API/数学函数/float类型数学库函数/isnan1.md) | 否 |
| [j0f](../SIMT-API/数学函数/float类型数学库函数/j0f.md) | 是 |
| [j1f](../SIMT-API/数学函数/float类型数学库函数/j1f.md) | 是 |
| [jnf](../SIMT-API/数学函数/float类型数学库函数/jnf.md) | 是 |
| [ldexpf](../SIMT-API/数学函数/float类型数学库函数/ldexpf.md) | 是 |
| [lgammaf](../SIMT-API/数学函数/float类型数学库函数/lgammaf.md) | 是 |
| [llrintf](../SIMT-API/数学函数/float类型数学库函数/llrintf.md) | 是 |
| [llroundf](../SIMT-API/数学函数/float类型数学库函数/llroundf.md) | 是 |
| [log10f](../SIMT-API/数学函数/float类型数学库函数/log10f.md) | 是 |
| [log1pf](../SIMT-API/数学函数/float类型数学库函数/log1pf.md) | 是 |
| [log2f](../SIMT-API/数学函数/float类型数学库函数/log2f.md) | 是 |
| [logbf](../SIMT-API/数学函数/float类型数学库函数/logbf.md) | 是 |
| [logf](../SIMT-API/数学函数/float类型数学库函数/logf.md) | 是 |
| [lrintf](../SIMT-API/数学函数/float类型数学库函数/lrintf.md) | 是 |
| [lroundf](../SIMT-API/数学函数/float类型数学库函数/lroundf.md) | 是 |
| [modff](../SIMT-API/数学函数/float类型数学库函数/modff.md) | 是 |
| [nearbyintf](../SIMT-API/数学函数/float类型数学库函数/nearbyintf.md) | 是 |
| [nextafterf](../SIMT-API/数学函数/float类型数学库函数/nextafterf.md) | 是 |
| [norm3df](../SIMT-API/数学函数/float类型数学库函数/norm3df.md) | 是 |
| [norm4df](../SIMT-API/数学函数/float类型数学库函数/norm4df.md) | 是 |
| [normcdff](../SIMT-API/数学函数/float类型数学库函数/normcdff.md) | 是 |
| [normcdfinvf](../SIMT-API/数学函数/float类型数学库函数/normcdfinvf.md) | 是 |
| [normf](../SIMT-API/数学函数/float类型数学库函数/normf.md) | 是 |
| [powf](../SIMT-API/数学函数/float类型数学库函数/powf.md) | 否 |
| [rcbrtf](../SIMT-API/数学函数/float类型数学库函数/rcbrtf.md) | 是 |
| [remainderf](../SIMT-API/数学函数/float类型数学库函数/remainderf.md) | 是 |
| [remquof](../SIMT-API/数学函数/float类型数学库函数/remquof.md) | 是 |
| [rhypotf](../SIMT-API/数学函数/float类型数学库函数/rhypotf.md) | 是 |
| [rintf](../SIMT-API/数学函数/float类型数学库函数/rintf.md) | 是 |
| [rnorm3df](../SIMT-API/数学函数/float类型数学库函数/rnorm3df.md) | 是 |
| [rnorm4df](../SIMT-API/数学函数/float类型数学库函数/rnorm4df.md) | 是 |
| [rnormf](../SIMT-API/数学函数/float类型数学库函数/rnormf.md) | 是 |
| [roundf](../SIMT-API/数学函数/float类型数学库函数/roundf.md) | 否 |
| [rsqrtf](../SIMT-API/数学函数/float类型数学库函数/rsqrtf.md) | 是 |
| [scalblnf](../SIMT-API/数学函数/float类型数学库函数/scalblnf.md) | 是 |
| [scalbnf](../SIMT-API/数学函数/float类型数学库函数/scalbnf.md) | 是 |
| [signbit](../SIMT-API/数学函数/float类型数学库函数/signbit.md) | 是 |
| [sincosf](../SIMT-API/数学函数/float类型数学库函数/sincosf.md) | 是 |
| [sincospif](../SIMT-API/数学函数/float类型数学库函数/sincospif.md) | 是 |
| [sinf](../SIMT-API/数学函数/float类型数学库函数/sinf.md) | 是 |
| [sinhf](../SIMT-API/数学函数/float类型数学库函数/sinhf.md) | 是 |
| [sinpif](../SIMT-API/数学函数/float类型数学库函数/sinpif.md) | 是 |
| [sqrtf](../SIMT-API/数学函数/float类型数学库函数/sqrtf.md) | 否 |
| [tanf](../SIMT-API/数学函数/float类型数学库函数/tanf.md) | 是 |
| [tanhf](../SIMT-API/数学函数/float类型数学库函数/tanhf.md) | 是 |
| [tanpif](../SIMT-API/数学函数/float类型数学库函数/tanpif.md) | 是 |
| [tgammaf](../SIMT-API/数学函数/float类型数学库函数/tgammaf.md) | 是 |
| [truncf](../SIMT-API/数学函数/float类型数学库函数/truncf.md) | 否 |
| [y0f](../SIMT-API/数学函数/float类型数学库函数/y0f.md) | 是 |
| [y1f](../SIMT-API/数学函数/float类型数学库函数/y1f.md) | 是 |
| [ynf](../SIMT-API/数学函数/float类型数学库函数/ynf.md) | 是 |

### fp8类型

#### fp8数据转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__asc_cvt_float2_to_fp8x2](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__asc_cvt_float2_to_fp8x2.md) | 是 |
| [__e4m3x22float2](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__e4m3x22float2.md) | 否 |
| [__e5m2x22float2](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__e5m2x22float2.md) | 否 |
| [__float22hif82_rh](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rh.md) | 否 |
| [__float22hif82_rh_sat](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rh_sat.md) | 否 |
| [__float22hif82_rna](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rna.md) | 否 |
| [__float22hif82_rna_sat](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__float22hif82_rna_sat.md) | 否 |
| [__half22hif82_rh](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rh.md) | 否 |
| [__half22hif82_rh_sat](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rh_sat.md) | 否 |
| [__half22hif82_rna](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rna.md) | 否 |
| [__half22hif82_rna_sat](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__half22hif82_rna_sat.md) | 否 |
| [__hif822float2](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__hif822float2.md) | 否 |
| [__hif822half2](../SIMT-API/数学函数/fp8类型/fp8数据转换函数/__hif822half2.md) | 否 |

### half类型

#### half2类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [h2ceil](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2ceil.md) | 是 |
| [h2cos](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2cos.md) | 是 |
| [h2exp](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp.md) | 是 |
| [h2exp10](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp10.md) | 是 |
| [h2exp2](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2exp2.md) | 是 |
| [h2floor](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2floor.md) | 是 |
| [h2log](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log.md) | 是 |
| [h2log10](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log10.md) | 是 |
| [h2log2](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2log2.md) | 是 |
| [h2rcp](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2rcp.md) | 是 |
| [h2rint](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2rint.md) | 是 |
| [h2rsqrt](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2rsqrt.md) | 是 |
| [h2sin](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2sin.md) | 是 |
| [h2sqrt](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2sqrt.md) | 否 |
| [h2tanh](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2tanh.md) | 是 |
| [h2trunc](../SIMT-API/数学函数/half类型/half2类型数学库函数/h2trunc.md) | 是 |

#### half2类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__hbequx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbequx2.md) | 否 |
| [__hbeqx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbeqx2.md) | 否 |
| [__hbgeux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbgeux2.md) | 否 |
| [__hbgex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbgex2.md) | 否 |
| [__hbgtux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbgtux2.md) | 否 |
| [__hbgtx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbgtx2.md) | 否 |
| [__hbleux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbleux2.md) | 否 |
| [__hblex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hblex2.md) | 否 |
| [__hbltux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbltux2.md) | 否 |
| [__hbltx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbltx2.md) | 否 |
| [__hbneux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbneux2.md) | 否 |
| [__hbnex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hbnex2.md) | 否 |
| [__hequx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hequx2.md) | 否 |
| [__hequx2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hequx2_mask.md) | 否 |
| [__heqx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__heqx2.md) | 否 |
| [__heqx2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__heqx2_mask.md) | 否 |
| [__hgeux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgeux2.md) | 否 |
| [__hgeux2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgeux2_mask.md) | 否 |
| [__hgex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgex2.md) | 否 |
| [__hgex2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgex2_mask.md) | 否 |
| [__hgtux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgtux2.md) | 否 |
| [__hgtux2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgtux2_mask.md) | 否 |
| [__hgtx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgtx2.md) | 否 |
| [__hgtx2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hgtx2_mask.md) | 否 |
| [__hleux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hleux2.md) | 否 |
| [__hleux2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hleux2_mask.md) | 否 |
| [__hlex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hlex2.md) | 否 |
| [__hlex2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hlex2_mask.md) | 否 |
| [__hltux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hltux2.md) | 否 |
| [__hltux2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hltux2_mask.md) | 否 |
| [__hltx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hltx2.md) | 否 |
| [__hltx2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hltx2_mask.md) | 否 |
| [__hmaxx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hmaxx2.md) | 否 |
| [__hmaxx2_nan](../SIMT-API/数学函数/half类型/half2类型比较函数/__hmaxx2_nan.md) | 是 |
| [__hminx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hminx2.md) | 否 |
| [__hminx2_nan](../SIMT-API/数学函数/half类型/half2类型比较函数/__hminx2_nan.md) | 是 |
| [__hneux2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hneux2.md) | 否 |
| [__hneux2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hneux2_mask.md) | 否 |
| [__hnex2](../SIMT-API/数学函数/half类型/half2类型比较函数/__hnex2.md) | 否 |
| [__hnex2_mask](../SIMT-API/数学函数/half类型/half2类型比较函数/__hnex2_mask.md) | 否 |
| [__isnanx2](../SIMT-API/数学函数/half类型/half2类型比较函数/__isnanx2.md) | 否 |

#### half2类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habsx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__habsx2.md) | 否 |
| [__haddx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__haddx2.md) | 否 |
| [__hcmadd](../SIMT-API/数学函数/half类型/half2类型算术函数/__hcmadd.md) | 是 |
| [__hdivx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hdivx2.md) | 否 |
| [__hfmax2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hfmax2.md) | 否 |
| [__hfmax2_relu](../SIMT-API/数学函数/half类型/half2类型算术函数/__hfmax2_relu.md) | 是 |
| [__hmulx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hmulx2.md) | 否 |
| [__hnegx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hnegx2.md) | 否 |
| [__hsubx2](../SIMT-API/数学函数/half类型/half2类型算术函数/__hsubx2.md) | 否 |

#### half类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [hceil](../SIMT-API/数学函数/half类型/half类型数学库函数/hceil.md) | 是 |
| [hcos](../SIMT-API/数学函数/half类型/half类型数学库函数/hcos.md) | 是 |
| [hexp](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp.md) | 否 |
| [hexp10](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp10.md) | 是 |
| [hexp2](../SIMT-API/数学函数/half类型/half类型数学库函数/hexp2.md) | 是 |
| [hfloor](../SIMT-API/数学函数/half类型/half类型数学库函数/hfloor.md) | 是 |
| [hlog](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog.md) | 否 |
| [hlog10](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog10.md) | 是 |
| [hlog2](../SIMT-API/数学函数/half类型/half类型数学库函数/hlog2.md) | 是 |
| [hrcp](../SIMT-API/数学函数/half类型/half类型数学库函数/hrcp.md) | 是 |
| [hrint](../SIMT-API/数学函数/half类型/half类型数学库函数/hrint.md) | 否 |
| [hrsqrt](../SIMT-API/数学函数/half类型/half类型数学库函数/hrsqrt.md) | 是 |
| [hsin](../SIMT-API/数学函数/half类型/half类型数学库函数/hsin.md) | 是 |
| [hsqrt](../SIMT-API/数学函数/half类型/half类型数学库函数/hsqrt.md) | 否 |
| [htanh](../SIMT-API/数学函数/half类型/half类型数学库函数/htanh.md) | 是 |
| [htrunc](../SIMT-API/数学函数/half类型/half类型数学库函数/htrunc.md) | 是 |

#### half类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__heq](../SIMT-API/数学函数/half类型/half类型比较函数/__heq.md) | 否 |
| [__hequ](../SIMT-API/数学函数/half类型/half类型比较函数/__hequ.md) | 否 |
| [__hge](../SIMT-API/数学函数/half类型/half类型比较函数/__hge.md) | 否 |
| [__hgeu](../SIMT-API/数学函数/half类型/half类型比较函数/__hgeu.md) | 否 |
| [__hgt](../SIMT-API/数学函数/half类型/half类型比较函数/__hgt.md) | 否 |
| [__hgtu](../SIMT-API/数学函数/half类型/half类型比较函数/__hgtu.md) | 否 |
| [__hisinf](../SIMT-API/数学函数/half类型/half类型比较函数/__hisinf.md) | 否 |
| [__hisnan](../SIMT-API/数学函数/half类型/half类型比较函数/__hisnan.md) | 否 |
| [__hle](../SIMT-API/数学函数/half类型/half类型比较函数/__hle.md) | 否 |
| [__hleu](../SIMT-API/数学函数/half类型/half类型比较函数/__hleu.md) | 否 |
| [__hlt](../SIMT-API/数学函数/half类型/half类型比较函数/__hlt.md) | 否 |
| [__hltu](../SIMT-API/数学函数/half类型/half类型比较函数/__hltu.md) | 否 |
| [__hmax](../SIMT-API/数学函数/half类型/half类型比较函数/__hmax.md) | 是 |
| [__hmax_nan](../SIMT-API/数学函数/half类型/half类型比较函数/__hmax_nan.md) | 是 |
| [__hmin](../SIMT-API/数学函数/half类型/half类型比较函数/__hmin.md) | 是 |
| [__hmin_nan](../SIMT-API/数学函数/half类型/half类型比较函数/__hmin_nan.md) | 是 |
| [__hne](../SIMT-API/数学函数/half类型/half类型比较函数/__hne.md) | 否 |
| [__hneu](../SIMT-API/数学函数/half类型/half类型比较函数/__hneu.md) | 否 |

#### half类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habs](../SIMT-API/数学函数/half类型/half类型算术函数/__habs.md) | 是 |
| [__hadd](../SIMT-API/数学函数/half类型/half类型算术函数/__hadd.md) | 否 |
| [__hdiv](../SIMT-API/数学函数/half类型/half类型算术函数/__hdiv.md) | 否 |
| [__hfma](../SIMT-API/数学函数/half类型/half类型算术函数/__hfma.md) | 否 |
| [__hfma_relu](../SIMT-API/数学函数/half类型/half类型算术函数/__hfma_relu.md) | 是 |
| [__hmul](../SIMT-API/数学函数/half类型/half类型算术函数/__hmul.md) | 否 |
| [__hneg](../SIMT-API/数学函数/half类型/half类型算术函数/__hneg.md) | 否 |
| [__hsub](../SIMT-API/数学函数/half类型/half类型算术函数/__hsub.md) | 否 |

#### half类型精度转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__bfloat162half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rd.md) | 否 |
| [__bfloat162half_rd_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rd_sat.md) | 否 |
| [__bfloat162half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rn.md) | 否 |
| [__bfloat162half_rn_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rn_sat.md) | 否 |
| [__bfloat162half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rna.md) | 否 |
| [__bfloat162half_rna_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rna_sat.md) | 否 |
| [__bfloat162half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_ru.md) | 否 |
| [__bfloat162half_ru_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_ru_sat.md) | 否 |
| [__bfloat162half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rz.md) | 否 |
| [__bfloat162half_rz_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__bfloat162half_rz_sat.md) | 否 |
| [__float22half2_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rd.md) | 否 |
| [__float22half2_rd_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rd_sat.md) | 否 |
| [__float22half2_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rn.md) | 否 |
| [__float22half2_rn_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rn_sat.md) | 否 |
| [__float22half2_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rna.md) | 否 |
| [__float22half2_rna_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rna_sat.md) | 否 |
| [__float22half2_ro](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ro.md) | 否 |
| [__float22half2_ro_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ro_sat.md) | 否 |
| [__float22half2_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ru.md) | 否 |
| [__float22half2_ru_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_ru_sat.md) | 否 |
| [__float22half2_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rz.md) | 否 |
| [__float22half2_rz_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float22half2_rz_sat.md) | 否 |
| [__float2half](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half.md) | 否 |
| [__float2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rd.md) | 否 |
| [__float2half_rd_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rd_sat.md) | 否 |
| [__float2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rn.md) | 否 |
| [__float2half_rn_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rn_sat.md) | 否 |
| [__float2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rna.md) | 否 |
| [__float2half_rna_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rna_sat.md) | 否 |
| [__float2half_ro](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ro.md) | 否 |
| [__float2half_ro_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ro_sat.md) | 否 |
| [__float2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ru.md) | 是 |
| [__float2half_ru_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_ru_sat.md) | 否 |
| [__float2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rz.md) | 否 |
| [__float2half_rz_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__float2half_rz_sat.md) | 否 |
| [__floats2half2_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__floats2half2_rn.md) | 是 |
| [__half22float2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half22float2.md) | 否 |
| [__half2float](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2float.md) | 是 |
| [__half2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rd.md) | 否 |
| [__half2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rn.md) | 否 |
| [__half2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rna.md) | 否 |
| [__half2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_ru.md) | 否 |
| [__half2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2half_rz.md) | 否 |
| [__half2int_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rd.md) | 否 |
| [__half2int_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rn.md) | 否 |
| [__half2int_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rna.md) | 否 |
| [__half2int_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_ru.md) | 否 |
| [__half2int_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2int_rz.md) | 否 |
| [__half2ll_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rd.md) | 是 |
| [__half2ll_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rn.md) | 是 |
| [__half2ll_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rna.md) | 是 |
| [__half2ll_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_ru.md) | 是 |
| [__half2ll_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ll_rz.md) | 是 |
| [__half2uint_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rd.md) | 否 |
| [__half2uint_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rn.md) | 否 |
| [__half2uint_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rna.md) | 否 |
| [__half2uint_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_ru.md) | 否 |
| [__half2uint_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2uint_rz.md) | 否 |
| [__half2ull_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rd.md) | 是 |
| [__half2ull_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rn.md) | 是 |
| [__half2ull_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rna.md) | 是 |
| [__half2ull_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_ru.md) | 是 |
| [__half2ull_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__half2ull_rz.md) | 是 |
| [__halves2half2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__halves2half2.md) | 是 |
| [__high2float](../SIMT-API/数学函数/half类型/half类型精度转换函数/__high2float.md) | 否 |
| [__high2half](../SIMT-API/数学函数/half类型/half类型精度转换函数/__high2half.md) | 否 |
| [__high2half2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__high2half2.md) | 是 |
| [__highs2half2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__highs2half2.md) | 是 |
| [__int2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rd.md) | 否 |
| [__int2half_rd_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rd_sat.md) | 否 |
| [__int2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rn.md) | 否 |
| [__int2half_rn_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rn_sat.md) | 否 |
| [__int2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rna.md) | 否 |
| [__int2half_rna_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rna_sat.md) | 否 |
| [__int2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_ru.md) | 否 |
| [__int2half_ru_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_ru_sat.md) | 否 |
| [__int2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rz.md) | 否 |
| [__int2half_rz_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__int2half_rz_sat.md) | 否 |
| [__ll2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rd.md) | 是 |
| [__ll2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rn.md) | 是 |
| [__ll2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rna.md) | 是 |
| [__ll2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_ru.md) | 是 |
| [__ll2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ll2half_rz.md) | 是 |
| [__low2float](../SIMT-API/数学函数/half类型/half类型精度转换函数/__low2float.md) | 否 |
| [__low2half](../SIMT-API/数学函数/half类型/half类型精度转换函数/__low2half.md) | 是 |
| [__low2half2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__low2half2.md) | 是 |
| [__lowhigh2highlow](../SIMT-API/数学函数/half类型/half类型精度转换函数/__lowhigh2highlow.md) | 是 |
| [__lows2half2](../SIMT-API/数学函数/half类型/half类型精度转换函数/__lows2half2.md) | 是 |
| [__uint2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rd.md) | 否 |
| [__uint2half_rd_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rd_sat.md) | 否 |
| [__uint2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rn.md) | 否 |
| [__uint2half_rn_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rn_sat.md) | 否 |
| [__uint2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rna.md) | 否 |
| [__uint2half_rna_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rna_sat.md) | 否 |
| [__uint2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_ru.md) | 否 |
| [__uint2half_ru_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_ru_sat.md) | 否 |
| [__uint2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rz.md) | 否 |
| [__uint2half_rz_sat](../SIMT-API/数学函数/half类型/half类型精度转换函数/__uint2half_rz_sat.md) | 否 |
| [__ull2half_rd](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rd.md) | 是 |
| [__ull2half_rn](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rn.md) | 是 |
| [__ull2half_rna](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rna.md) | 是 |
| [__ull2half_ru](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_ru.md) | 是 |
| [__ull2half_rz](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ull2half_rz.md) | 是 |
| [__ushort_as_half](../SIMT-API/数学函数/half类型/half类型精度转换函数/__ushort_as_half.md) | 是 |

### 数据类型转换

#### 类型转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__float2float_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rd.md) | 否 |
| [__float2float_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rn.md) | 否 |
| [__float2float_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rna.md) | 否 |
| [__float2float_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_ru.md) | 否 |
| [__float2float_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2float_rz.md) | 否 |
| [__float2int_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rd.md) | 否 |
| [__float2int_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rn.md) | 否 |
| [__float2int_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rna.md) | 否 |
| [__float2int_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_ru.md) | 否 |
| [__float2int_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2int_rz.md) | 否 |
| [__float2ll_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rd.md) | 否 |
| [__float2ll_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rn.md) | 否 |
| [__float2ll_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rna.md) | 否 |
| [__float2ll_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_ru.md) | 否 |
| [__float2ll_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ll_rz.md) | 否 |
| [__float2uint_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rd.md) | 否 |
| [__float2uint_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rn.md) | 否 |
| [__float2uint_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rna.md) | 否 |
| [__float2uint_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_ru.md) | 否 |
| [__float2uint_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2uint_rz.md) | 否 |
| [__float2ull_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rd.md) | 否 |
| [__float2ull_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rn.md) | 否 |
| [__float2ull_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rna.md) | 否 |
| [__float2ull_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_ru.md) | 否 |
| [__float2ull_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float2ull_rz.md) | 否 |
| [__float_as_int](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float_as_int.md) | 是 |
| [__float_as_uint](../SIMT-API/数学函数/数据类型转换/类型转换函数/__float_as_uint.md) | 是 |
| [__int2float_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rd.md) | 否 |
| [__int2float_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rn.md) | 否 |
| [__int2float_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rna.md) | 否 |
| [__int2float_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_ru.md) | 否 |
| [__int2float_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int2float_rz.md) | 否 |
| [__int_as_float](../SIMT-API/数学函数/数据类型转换/类型转换函数/__int_as_float.md) | 是 |
| [__ll2float_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rd.md) | 否 |
| [__ll2float_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rn.md) | 否 |
| [__ll2float_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rna.md) | 否 |
| [__ll2float_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_ru.md) | 否 |
| [__ll2float_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ll2float_rz.md) | 否 |
| [__uint2float_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rd.md) | 否 |
| [__uint2float_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rn.md) | 否 |
| [__uint2float_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rna.md) | 否 |
| [__uint2float_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_ru.md) | 否 |
| [__uint2float_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint2float_rz.md) | 否 |
| [__uint_as_float](../SIMT-API/数学函数/数据类型转换/类型转换函数/__uint_as_float.md) | 是 |
| [__ull2float_rd](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rd.md) | 否 |
| [__ull2float_rn](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rn.md) | 否 |
| [__ull2float_rna](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rna.md) | 否 |
| [__ull2float_ru](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_ru.md) | 否 |
| [__ull2float_rz](../SIMT-API/数学函数/数据类型转换/类型转换函数/__ull2float_rz.md) | 否 |

### 整型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [__brev](../SIMT-API/数学函数/整型数学库函数/__brev.md) | 否 |
| [__byte_perm](../SIMT-API/数学函数/整型数学库函数/__byte_perm.md) | 否 |
| [__clz](../SIMT-API/数学函数/整型数学库函数/__clz.md) | 否 |
| [__ffs](../SIMT-API/数学函数/整型数学库函数/__ffs.md) | 否 |
| [__hadd](../SIMT-API/数学函数/整型数学库函数/__hadd-259.md) | 否 |
| [__mul24](../SIMT-API/数学函数/整型数学库函数/__mul24.md) | 否 |
| [__mul64hi](../SIMT-API/数学函数/整型数学库函数/__mul64hi.md) | 否 |
| [__mul_i32toi64](../SIMT-API/数学函数/整型数学库函数/__mul_i32toi64.md) | 否 |
| [__mulhi](../SIMT-API/数学函数/整型数学库函数/__mulhi.md) | 否 |
| [__popc](../SIMT-API/数学函数/整型数学库函数/__popc.md) | 否 |
| [__rhadd](../SIMT-API/数学函数/整型数学库函数/__rhadd.md) | 否 |
| [__sad](../SIMT-API/数学函数/整型数学库函数/__sad.md) | 否 |
| [__uhadd](../SIMT-API/数学函数/整型数学库函数/__uhadd.md) | 否 |
| [__umul24](../SIMT-API/数学函数/整型数学库函数/__umul24.md) | 否 |
| [__umul64hi](../SIMT-API/数学函数/整型数学库函数/__umul64hi.md) | 否 |
| [__umulhi](../SIMT-API/数学函数/整型数学库函数/__umulhi.md) | 否 |
| [__urhadd](../SIMT-API/数学函数/整型数学库函数/__urhadd.md) | 否 |
| [__usad](../SIMT-API/数学函数/整型数学库函数/__usad.md) | 否 |
| [labs](../SIMT-API/数学函数/整型数学库函数/labs.md) | 是 |
| [llabs](../SIMT-API/数学函数/整型数学库函数/llabs.md) | 是 |
| [llmax](../SIMT-API/数学函数/整型数学库函数/llmax.md) | 是 |
| [llmin](../SIMT-API/数学函数/整型数学库函数/llmin.md) | 是 |
| [max](../SIMT-API/数学函数/整型数学库函数/max1.md) | 否 |
| [min](../SIMT-API/数学函数/整型数学库函数/min1.md) | 否 |
| [ullmax](../SIMT-API/数学函数/整型数学库函数/ullmax.md) | 是 |
| [ullmin](../SIMT-API/数学函数/整型数学库函数/ullmin.md) | 是 |
| [umax](../SIMT-API/数学函数/整型数学库函数/umax.md) | 否 |
| [umin](../SIMT-API/数学函数/整型数学库函数/umin.md) | 否 |

## 访存函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_dcci_entire](../SIMT-API/访存函数/asc_dcci_entire.md) | 否 |
| [asc_dcci_single](../SIMT-API/访存函数/asc_dcci_single.md) | 否 |
| [asc_ldca](../SIMT-API/访存函数/asc_ldca.md) | 否 |
| [asc_ldcg](../SIMT-API/访存函数/asc_ldcg.md) | 否 |
| [asc_stcg](../SIMT-API/访存函数/asc_stcg.md) | 否 |
| [asc_stwt](../SIMT-API/访存函数/asc_stwt.md) | 否 |
