# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture: dav-2201, dav-3510")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

# 场景参数（通过命令行传入）
# SCENARIO_NUM: 场景编号
#   1 - WholeReduceMax：每个repeat内求最大值，输入[1, 1024]，输出[1, 8]
#   2 - WholeReduceMin：每个repeat内求最小值及索引，输入[1, 1024]，输出[1, 16]
#   3 - WholeReduceSum：每个repeat内求和，输入float类型[1, 2048]，输出[1, 32]
#   4 - RepeatReduceSum：累加模式求和，输入float类型[1, 2048]，输出[1, 32]
#   5 - WholeReduceMin + GetReduceRepeatMaxMinSpr：求全局最小值及索引，输入[1, 1024]，输出[1, 16]
#   6 - WholeReduceSum非对齐场景：对每行非对齐数据求和，输入float类型[13, 57]，输出[1, 13]
set(SCENARIO_NUM "1" CACHE STRING "Scene number (1-6)")

add_definitions(-DSCENARIO_NUM=${SCENARIO_NUM})

add_executable(demo
    whole_reduce_min_max_sum.asc
)

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
)
