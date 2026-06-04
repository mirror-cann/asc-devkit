#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

init_case() {
    rm -rf *.bc
    rm -rf a.out
    rm -rf input
    rm -rf output
}


run_case() {
    init_case
    case_name="$1"
    echo "[ RUN      ] ${case_name}"
    set -e
    rm -rf build
    mkdir build
    pushd build
    cmake ..
    make -j4
    start=$(date +%s.%N)
    eval "$2"
    end=$(date +%s.%N)
    compile_time=$(awk -v start="$start" -v end="$end" 'BEGIN {print end - start}')

    set +e
    start=$(date +%s.%N)
    # output=$(./a.out 2>&1)
    res=$?
    end=$(date +%s.%N)
    run_time=$(awk -v start="$start" -v end="$end" 'BEGIN {print end - start}')
    res=$((res + $?))
    init_case
    set -e
    return $res
}

set -e
run_case demo "./demo"