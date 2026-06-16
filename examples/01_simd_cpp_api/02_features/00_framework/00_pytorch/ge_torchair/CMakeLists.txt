# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
project(ge_torchair LANGUAGES ASC)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env TORCH_DEVICE_BACKEND_AUTOLOAD=0
            ${Python3_EXECUTABLE} -c "import torch; print(torch.utils.cmake_prefix_path)"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE TORCH_CMAKE_PREFIX_PATH
)
find_package(Torch REQUIRED HINTS ${TORCH_CMAKE_PREFIX_PATH})

add_library(ascendc_ops SHARED
    add_custom_kernel.asc
    add_custom_pytorch.asc
    add_custom_ge.asc
)

target_include_directories(ascendc_ops PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${TORCH_INCLUDE_DIRS}
    ${ASCEND_HOME_PATH}/include
    ${ASCEND_HOME_PATH}/include/graph
    ${ASCEND_HOME_PATH}/include/register
    ${ASCEND_HOME_PATH}/include/external
)

target_link_directories(ascendc_ops PRIVATE
    ${ASCEND_HOME_PATH}/lib64
)

target_link_libraries(ascendc_ops PRIVATE
    -Wl,--no-as-needed
    Python3::Python
    ascendc_runtime
    acl_rt
    graph
    graph_base
    register
    gert
    ${TORCH_LIBRARIES}
    -Wl,--as-needed
)
