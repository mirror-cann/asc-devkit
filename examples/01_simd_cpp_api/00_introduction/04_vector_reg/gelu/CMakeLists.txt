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

set(CMAKE_ASC_RUN_MODE "npu" CACHE STRING "Run mode: npu, sim")
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture: dav-3510")
set(CMAKE_VF_MODE "true" CACHE STRING "VF fusion mode: true, false")

find_package(ASC REQUIRED)

project(kernel_samples LANGUAGES ASC CXX)

add_executable(demo
    gelu.asc
)

if(CMAKE_ASC_RUN_MODE STREQUAL "sim" AND CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510")
    # TEMP: 新版 CANN 包的 dav-3510 仿真仍需要手动补充 camodel 路径。
    # 后续包侧恢复默认搜索路径后，这段兼容逻辑可以删除。
    set(ASC_SIM_CAMODEL_PATH "$ENV{ASCEND_HOME_PATH}/tools/simulator/dav_3510/camodel")
    if(EXISTS "${ASC_SIM_CAMODEL_PATH}")
        string(PREPEND CMAKE_ASC_LINK_FLAGS
            " -Wl,-rpath,${ASC_SIM_CAMODEL_PATH} -Wl,-L${ASC_SIM_CAMODEL_PATH}")
    endif()
endif()

target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>
    $<$<COMPILE_LANGUAGE:ASC>:--cce-simd-vf-fusion=${CMAKE_VF_MODE}>
)
