# ----------------------------------------------------------------------------------------------------------
# This program is free software, you can redistribute it and/or modify it.
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This file is a part of the CANN Open Software.
# Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(kernel_samples)

find_package(ASC REQUIRED)
enable_language(ASC CXX)

add_executable(scatter scatter.asc)

target_compile_options(scatter PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-3510 --enable-simt>
)

add_compile_options(scatter OPTIONS -g)

target_link_libraries(scatter PRIVATE
    m
    platform
    tiling_api
    dl
)
