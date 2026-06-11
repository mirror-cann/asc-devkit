#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

# 安全选项：遇到错误立即退出，使用未定义变量报错，管道失败时退出
set -euo pipefail

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# 环境配置
ASCEND_HOME_DIR="${ASCEND_HOME_DIR:-/usr/local/Ascend/ascend-toolkit/latest}"

# 加载 Ascend 环境
if [ ! -f "${ASCEND_HOME_DIR}/set_env.sh" ]; then
    echo "Error: Ascend environment file not found: ${ASCEND_HOME_DIR}/set_env.sh"
    exit 1
fi
source "${ASCEND_HOME_DIR}/set_env.sh"

# 配置路径
CODE_PATH="${PROJECT_ROOT}/examples"
LOG_PATH="${PROJECT_ROOT}/tmplog_devkit"

# 验证路径
if [ ! -d "${CODE_PATH}" ]; then
    echo "Error: Examples directory not found: ${CODE_PATH}"
    exit 1
fi

# 示例列表
EXAMPLE_LIST=(
    devkit_dir_check
    01_simd_cpp_api/00_introduction/01_vector/add
    01_simd_cpp_api/00_introduction/01_vector/add_tpipe_tque
    01_simd_cpp_api/00_introduction/02_matrix/matmul_advanced_api
    01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api
    01_simd_cpp_api/01_utilities/03_cpudebug
    01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/batch_matmul
    01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/load_data_l12l0
    01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/mmad_load3dv2
    01_simd_cpp_api/02_features/03_basic_api/02_memory_vector_compute/brcb
    01_simd_cpp_api/02_features/03_basic_api/02_memory_vector_compute/compare
    01_simd_cpp_api/02_features/03_basic_api/02_memory_vector_compute/gather
    01_simd_cpp_api/02_features/03_basic_api/02_memory_vector_compute/mrg_sort
    01_simd_cpp_api/02_features/03_basic_api/04_resource_management/get_tpipe_ptr
    01_simd_cpp_api/02_features/03_basic_api/06_sync_control/group_barrier
    01_simd_cpp_api/02_features/03_basic_api/08_atomic_operations/data_movement_with_atomic_operations
    01_simd_cpp_api/03_libraries/00_matrix/matmul_fused_manual
    01_simd_cpp_api/03_libraries/00_matrix/matmul_a2b2_share
    01_simd_cpp_api/03_libraries/00_matrix/matmul_tscm
    01_simd_cpp_api/03_libraries/01_activation/gelu
    01_simd_cpp_api/03_libraries/02_normalization/layernorm
    01_simd_cpp_api/03_libraries/03_quantization/dequant
    01_simd_cpp_api/03_libraries/04_reduce/reducemax
    01_simd_cpp_api/03_libraries/05_sort/sort
    01_simd_cpp_api/03_libraries/06_index/arange
    01_simd_cpp_api/03_libraries/07_filter/dropout
    01_simd_cpp_api/03_libraries/08_tensor_transformation/broadcast
    01_simd_cpp_api/03_libraries/12_math/acosh
    02_simd_c_api/00_introduction/01_add/c_api_async_add
    02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add
    02_simd_c_api/00_introduction/01_add/c_api_sync_add
)

# 时间格式化函数（秒转换为可读格式）
format_duration() {
    local seconds=$1
    if [ "${seconds}" -eq 0 ]; then
        echo "0s"
        return
    fi
    echo "${seconds}" | awk '{
        t=split("60 s 60 m 24 h 999 d", a)
        for(n=1; n<t; n+=2) {
            if($1==0) break
            s=$1%a[n] a[n+1] s
            $1=int($1/a[n])
        }
        print s
    }'
}

# devkit_dir_check执行函数
run_check() {
    local base_path=$1
    
    cd "${base_path}/" || return 1
    bash devkit_dir_check.sh 
}

# 标准执行函数（cmake + make + gen_data + demo + verify）
run_standard() {
    local base_path=$1
    local example_name=$2
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. || return 1
    make -j || return 1
    
    if [ -f "../scripts/gen_data.py" ]; then
        python3 ../scripts/gen_data.py || return 1
    fi
    
    ./demo || return 1
    
    if [ -f "../scripts/verify_result.py" ]; then
        python3 ../scripts/verify_result.py output/output.bin output/golden.bin || return 1
    fi
}

# 双输出验证函数
run_two_outputs() {
    local base_path=$1
    local example_name=$2
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. || return 1
    make -j || return 1
    
    if [ -f "../scripts/gen_data.py" ]; then
        python3 ../scripts/gen_data.py || return 1
    fi
    
    ./demo || return 1
    
    if [ -f "../scripts/verify_result.py" ]; then
        python3 ../scripts/verify_result.py output/output1.bin output/golden1.bin output/output2.bin output/golden2.bin || return 1
    fi
}

# CPU模式执行函数
run_cpu_mode() {
    local base_path=$1
    local example_name=$2
    local executable=${3:-add}
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    cmake -B build -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 || return 1
    cmake --build build || return 1
    ./build/"${executable}" || return 1
}

# 参数化执行函数
run_with_params() {
    local base_path=$1
    local example_name=$2
    local cmake_params=${3:-}
    local gen_data_params=${4:-}
    local executable=${5:-demo}
    local verify_params=${6:-output/output.bin output/golden.bin}
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. ${cmake_params} || return 1
    make -j || return 1
    
    if [ -f "../scripts/gen_data.py" ]; then
        python3 ../scripts/gen_data.py ${gen_data_params} || return 1
    fi
    
    ./"${executable}" || return 1
    
    if [ -f "../scripts/verify_result.py" ]; then
        python3 ../scripts/verify_result.py ${verify_params} || return 1
    fi
}

run_with_params_2() {
    local base_path=$1
    local example_name=$2
    local cmake_params=${3:-}
    local executable=${4:-demo}
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. ${cmake_params} || return 1
    make -j || return 1
    ./"${executable}" || return 1
}

# C API执行函数
run_c_api() {
    local base_path=$1
    local example_name=$2
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. || return 1
    make -j || return 1
    ./c_api_add_example || return 1
}

# msProf执行函数
run_msprof() {
    local base_path=$1
    local example_name=$2
    
    cd "${base_path}/${example_name}/" || return 1
    rm -rf build
    mkdir -p build && cd build || return 1
    cmake .. || return 1
    make -j || return 1
    
    if [ -f "../scripts/gen_data.py" ]; then
        python3 ../scripts/gen_data.py || return 1
    fi
    
    msprof op ./demo || return 1
    
    if [ -f "../scripts/verify_result.py" ]; then
        python3 ../scripts/verify_result.py output/output.bin output/golden.bin || return 1
    fi
}

# 执行测试用例
run_test_case() {
    local code_path=$1
    local example_name=$2
    local log_path=$3
    
    local case_name="${example_name##*/}"
    local start_time end_time duration elapsed
    
    start_time=$(date +%s)
    echo ">>>>>>>>>>>>>>>>>>>>> $(date '+%Y-%m-%d %H:%M:%S') run ${case_name} start! <<<<<<<<<<<<<<<<<<<<<"
    
    local test_result=0
    echo ${case_name} >> ${log_path}/../devkit_cases.txt
    # 根据用例名称选择执行函数
    case "${case_name}" in
        devkit_dir_check)
            run_check "${SCRIPT_DIR}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        matmul_a2b2_share)
            run_two_outputs "${code_path}" "${example_name}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        matmul_ibshareAB)
            run_with_params "${code_path}" "${example_name}" "" "" "demo" "output/output_ABshare.bin output/golden.bin" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        matmul_no_ibshareAB)
            run_with_params "${code_path}" "${example_name}" "" "" "demo" "output/output_noABshare.bin output/golden.bin" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        matmul_b8|matmul_ibshareB)
            run_with_params "${code_path}" "${example_name}" "-DDT_MODE=0" "0" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        hello_world_npu|list_tensor_desc_input|get_tpipe_ptr|group_barrier|acosh)
            run_with_params "${code_path}" "${example_name}" "" "" "demo" "" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        basic_api_tque_add)
            run_with_params "${code_path}" "${example_name}" "" "" "add" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        03_cpudebug)
            run_cpu_mode "${code_path}" "${example_name}" "cpu_debug" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        msProf)
            run_msprof "${code_path}" "${example_name}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        basic_api_memory_allocator_add)
            run_with_params "${code_path}" "${example_name}" "" "" "add" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        batch_matmul)
            run_with_params "${code_path}" "${example_name}" "-DB_SIZE=4 -DM_SIZE=30 -DK_SIZE=40 -DN_SIZE=70" "-b=4 -m=30 -k=40 -n=70" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        mmad_load3dv2)
            run_with_params "${code_path}" "${example_name}" "-DSCENARIO_NUM=4 -DM_SIZE=30 -DK_SIZE=40 -DN_SIZE=70" "-scenarioNum=4 -m=30 -k=40 -n=70" "demo" "-scenarioNum=4 output/output.bin output/golden.bin" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        load_data_l12l0)
            run_with_params "${code_path}" "${example_name}" "-DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=12" "-scenarioNum=12" "demo" "-scenarioNum=12 output/output.bin output/golden.bin" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        antiquant)
            run_with_params_2 "${code_path}" "${example_name}" "-DNPU_ARCH=dav-2201" "" "demo" "" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        gelu)
            run_with_params "${code_path}" "${example_name}" "-DNPU_ARCH=dav-2201" "" "demo" "" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        layernorm|reducemax|sort|arange|dropout|broadcast)
            run_standard "${code_path}" "${example_name}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        c_api_async_add|c_api_delicacy_async_add|c_api_sync_add)
            run_c_api "${code_path}" "${example_name}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
        *)
            run_standard "${code_path}" "${example_name}" 2>&1 | tee "${log_path}/${case_name}.log"
            ;;
    esac
    
    test_result=${PIPESTATUS[0]}
    end_time=$(date +%s)
    duration=$((end_time - start_time))
    elapsed=$(format_duration "${duration}")
    
    echo "test case ${case_name} duration: ${elapsed}"
    echo ">>>>>>>>>>>>>>>>>>>>> $(date '+%Y-%m-%d %H:%M:%S') run ${case_name} finished! <<<<<<<<<<<<<<<<<<<<<"
    
    return ${test_result}
}

# 主执行流程
main() {
    echo "Current directory: ${PWD}"
    echo "Code path: ${CODE_PATH}"
    echo "Log path: ${LOG_PATH}"
    
    local start_time end_time total_duration total_elapsed
    
    start_time=$(date +%s)
    echo "=== $(date '+%Y-%m-%d %H:%M:%S') ==="
    
    # 准备日志目录
    mkdir -p "${LOG_PATH}"
    rm -rf "${LOG_PATH:?}"/*
    rm -rf ../result_devkit.txt ../devkit_cases.txt
    
    # 执行所有测试用例
    for example_name in "${EXAMPLE_LIST[@]}"; do
        run_test_case "${CODE_PATH}" "${example_name}" "${LOG_PATH}"
    done
    
    # 分析测试结果
    cd "${LOG_PATH}"/..
    
    local total_failed=0
    local total_passed=0
    
    while read -r line; do
        if [ -z "${line}" ]; then
            continue
        fi
        
        local log_file="${LOG_PATH}/${line}.log"
        
        if [ ! -f "${log_file}" ]; then
            echo "Warning: Log file not found: ${log_file}" >&2
            echo "${line} fail (no log)" >> result_devkit.txt
            ((total_failed++)) || true
            continue
        fi
        
        if [ ! -s "${log_file}" ]; then
            echo "Warning: Log file is empty: ${log_file}" >&2
            echo "${line} fail (empty log)" >> result_devkit.txt
            ((total_failed++)) || true
            continue
        fi
        
        prf=$(grep -E "check pass|test pass|passed|\[Block \(5\/6\)\]: OUTPUT = 24" "${log_file}" || true)
        if [ -n "$prf" ]; then
            echo "${line} pass" >> result_devkit.txt
            ((total_passed++)) || true
        else
            if grep -qiE "error|fail|exception" "${log_file}"; then
                echo "${line} fail (error detected)" >> result_devkit.txt
            else
                echo "${line} fail (no success marker)" >> result_devkit.txt
            fi
            ((total_failed++)) || true
        fi
    done < devkit_cases.txt
    
    # 输出测试总结
    local total_count=$((total_passed + total_failed))
    echo "========================================"
    echo "Test Summary:"
    echo "  Total:  ${total_count}"
    echo "  Passed: ${total_passed}"
    echo "  Failed: ${total_failed}"
    echo "========================================"
    
    # 检查是否有失败的测试
    if [ ${total_failed} -gt 0 ]; then
        echo "execute samples failed" >&2
        end_time=$(date +%s)
        total_duration=$((end_time - start_time))
        total_elapsed=$(format_duration "${total_duration}")
        echo "=== $(date '+%Y-%m-%d %H:%M:%S') ==="
        echo "test cases all duration: ${total_elapsed}"
        exit 1
    else
        echo "execute samples success"
    fi
    
    # 输出总耗时
    end_time=$(date +%s)
    total_duration=$((end_time - start_time))
    total_elapsed=$(format_duration "${total_duration}")
    echo "=== $(date '+%Y-%m-%d %H:%M:%S') ==="
    echo "test cases all duration: ${total_elapsed}"
}

# 执行主函数
main
