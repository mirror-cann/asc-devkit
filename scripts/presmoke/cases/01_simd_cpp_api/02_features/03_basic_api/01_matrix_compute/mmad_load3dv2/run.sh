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

set -euo pipefail

CASE_REL=01_simd_cpp_api/02_features/03_basic_api/01_matrix_compute/mmad_load3dv2
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../../../../_case_entry.sh"
presmoke_case_init "$CASE_REL"

case_build() {
    mkdir -p "$BUILD_DIR"
    (cd "$BUILD_DIR" && K=40 M=30 N=70 SCENARIO=4 soc_version=$SOC_VERSION bash -lc 'cmake .. -DSCENARIO_NUM=$SCENARIO -DM_SIZE=$M -DK_SIZE=$K -DN_SIZE=$N -DCMAKE_ASC_ARCHITECTURES="$ARCH" $RUN_MODE_ARG')
    (cd "$BUILD_DIR" && K=40 M=30 N=70 SCENARIO=4 soc_version=$SOC_VERSION bash -lc 'make -j')
    (cd "$BUILD_DIR" && K=40 M=30 N=70 SCENARIO=4 soc_version=$SOC_VERSION bash -lc 'python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO -m=$M -k=$K -n=$N')
}

case_run() {
    mkdir -p "$BUILD_DIR"
    (cd "$BUILD_DIR" && K=40 M=30 N=70 SCENARIO=4 soc_version=$SOC_VERSION bash -lc ./demo)
}

case_verify() {
    mkdir -p "$BUILD_DIR"
    (cd "$BUILD_DIR" && K=40 M=30 N=70 SCENARIO=4 soc_version=$SOC_VERSION bash -lc 'python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO output/output.bin output/golden.bin')
}

case_clean() {
    presmoke_default_clean
}

presmoke_case_main "$@"
