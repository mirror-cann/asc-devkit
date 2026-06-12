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

CASE_REL=04_aicpu/02_features/00_framwork/00_pytorch/tiling_sink_programming
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../../../../_case_entry.sh"
presmoke_case_init "$CASE_REL"

case_build() {
    mkdir -p "$BUILD_DIR"
    presmoke_ensure_custom_op_package
}

case_run() {
    mkdir -p "$BUILD_DIR"
    presmoke_clear_plog
    (cd "$CASE_DIR" && ASCEND_GLOBAL_LOG_LEVEL=1 soc_version=$SOC_VERSION bash -lc 'python3 test_add_custom_tiling_sink.py')
    presmoke_verify_tiling_sink_task_log 'GenerateTaskForSinkOp:Node [AddCustomTilingSink, AddCustomTilingSink] starts to generate tasks for the tiling sink, sk_flag [0].'
}

case_verify() {
    mkdir -p "$BUILD_DIR"
    :
}

case_clean() {
    presmoke_default_clean
}

presmoke_case_main "$@"
