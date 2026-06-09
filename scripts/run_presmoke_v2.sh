#!/usr/bin/env bash
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

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${PROJECT_ROOT:-$(cd "${SCRIPT_DIR}/.." && pwd)}"
ASCEND_HOME_DIR="${ASCEND_HOME_DIR:-/usr/local/Ascend/ascend-toolkit/latest}"
ARCH="${ARCH:-dav-2201}"
MODES="${MODES:-${MODE:-npu}}"
RUNNER_MODE="${RUNNER_MODE:-case-runner}"
SCHEDULE="${SCHEDULE:-fixed}"
SCHEDULE_FILE="${SCHEDULE_FILE:-}"
JOBS="${JOBS:-auto}"
NPU_SLOTS="${NPU_SLOTS:-1}"
CPU_RUN_SLOTS="${CPU_RUN_SLOTS:-auto}"
MAKE_JOBS="${MAKE_JOBS:-auto}"
TIMEOUT="${TIMEOUT:-120}"
CPU_RUN_TIMEOUT="${CPU_RUN_TIMEOUT:-300}"
PRIMARY_CARD="${PRIMARY_CARD:-0}"
RETRY_CARDS="${RETRY_CARDS:-}"
REPORT_FORMAT="${REPORT_FORMAT:-all}"
OUT_ROOT="${OUT_ROOT:-${PROJECT_ROOT}/presmoke_reports/presmoke_${ARCH}_$(date +%Y%m%d_%H%M%S)}"
LOCK_DIR="${LOCK_DIR:-${PROJECT_ROOT}/.presmoke_locks/run_presmoke.lock}"

if [[ "$MODES" != "npu" && "${SCHEDULE:-}" == "fixed" && -z "$SCHEDULE_FILE" ]]; then
    SCHEDULE="default"
fi

mkdir -p "$OUT_ROOT"
STATUS_FILE="$OUT_ROOT/status.txt"

date_iso() {
    date '+%Y-%m-%dT%H:%M:%S%z'
}

log() {
    printf '[%s] %s\n' "$(date_iso)" "$*" | tee -a "$STATUS_FILE"
}

cleanup_lock() {
    rm -rf "$LOCK_DIR"
}

acquire_lock() {
    mkdir -p "$(dirname "$LOCK_DIR")"
    if mkdir "$LOCK_DIR" 2>/dev/null; then
        echo "$$" > "$LOCK_DIR/pid"
        trap cleanup_lock EXIT
        return
    fi

    local pid
    pid="$(cat "$LOCK_DIR/pid" 2>/dev/null || true)"
    if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
        echo "Error: presmoke is already running, pid=$pid lock=$LOCK_DIR" >&2
        exit 2
    fi
    rm -rf "$LOCK_DIR"
    mkdir "$LOCK_DIR"
    echo "$$" > "$LOCK_DIR/pid"
    trap cleanup_lock EXIT
}

source_cann() {
    set +u
    if [[ -f "${ASCEND_HOME_DIR}/set_env.sh" ]]; then
        # shellcheck disable=SC1090
        source "${ASCEND_HOME_DIR}/set_env.sh" 2>/dev/null || true
    elif [[ -f "/usr/local/Ascend/cann/set_env.sh" ]]; then
        # shellcheck disable=SC1091
        source "/usr/local/Ascend/cann/set_env.sh" 2>/dev/null || true
    fi
    set -u
}

run_python_presmoke() {
    source_cann
    export PYTHONPATH="${SCRIPT_DIR}${PYTHONPATH:+:${PYTHONPATH}}"
    export PRESMOKE_PROJECT_ROOT="${PROJECT_ROOT}"
    exec python3 -m presmoke "$@"
}

run_python_module() {
    source_cann
    PYTHONPATH="${SCRIPT_DIR}${PYTHONPATH:+:${PYTHONPATH}}" \
    PRESMOKE_PROJECT_ROOT="${PROJECT_ROOT}" \
    python3 -m "$@"
}

npu_smi_info() {
    if ! command -v npu-smi >/dev/null 2>&1; then
        return 0
    fi
    if command -v timeout >/dev/null 2>&1; then
        timeout 10s npu-smi info 2>/dev/null || true
        return 0
    fi

    npu-smi info 2>/dev/null &
    local pid=$!
    local waited=0
    while kill -0 "$pid" 2>/dev/null; do
        if (( waited >= 10 )); then
            kill "$pid" 2>/dev/null || true
            return 0
        fi
        sleep 1
        waited=$((waited + 1))
    done
    wait "$pid" 2>/dev/null || true
}

presmoke_opp_path() {
    if [[ -n "${ASCEND_OPP_PATH:-}" ]]; then
        printf '%s\n' "$ASCEND_OPP_PATH"
        return
    fi
    if [[ -n "${ASCEND_HOME_PATH:-}" && -d "$ASCEND_HOME_PATH/opp" ]]; then
        printf '%s\n' "$ASCEND_HOME_PATH/opp"
        return
    fi
    printf '%s\n' "/usr/local/Ascend/cann-9.1.0/opp"
}

clean_cann_vendors() {
    source_cann
    local vendors_dir
    vendors_dir="$(presmoke_opp_path)/vendors"
    if [[ ! -d "$vendors_dir" ]]; then
        log "vendors_clean_skip missing_dir=$vendors_dir"
        return
    fi
    log "vendors_clean_begin dir=$vendors_dir"
    find "$vendors_dir" -mindepth 1 -maxdepth 1 -exec rm -rf {} +
    log "vendors_clean_done dir=$vendors_dir"
}

should_clean_cann_vendors() {
    local arg
    for arg in "$@"; do
        case "$arg" in
            --dry-run|--filter|--filter=*|--suggestions-only) return 1 ;;
        esac
    done
    return 0
}

run_presmoke() {
    local name="$1"
    local card="$2"
    shift 2
    local run_dir="$OUT_ROOT/$name"
    mkdir -p "$run_dir"
    log "run_start name=$name card=$card arch=$ARCH modes=$MODES jobs=$JOBS npu_slots=$NPU_SLOTS cpu_run_slots=$CPU_RUN_SLOTS make_jobs=$MAKE_JOBS timeout=$TIMEOUT cpu_run_timeout=$CPU_RUN_TIMEOUT schedule=$SCHEDULE args=$*"
    {
        echo "name=$name"
        echo "started_at=$(date_iso)"
        echo "project_root=$PROJECT_ROOT"
        echo "git=$(cd "$PROJECT_ROOT" && git rev-parse --short HEAD 2>/dev/null || true)"
        echo "arch=$ARCH"
        echo "modes=$MODES"
        echo "runner_mode=$RUNNER_MODE"
        echo "schedule=$SCHEDULE"
        echo "schedule_file=$SCHEDULE_FILE"
        echo "jobs=$JOBS"
        echo "npu_slots=$NPU_SLOTS"
        echo "cpu_run_slots=$CPU_RUN_SLOTS"
        echo "make_jobs=$MAKE_JOBS"
        echo "timeout=$TIMEOUT"
        echo "cpu_run_timeout=$CPU_RUN_TIMEOUT"
        echo "card=$card"
        echo "extra_args=$*"
        source_cann
        echo "npu_smi_before_begin"
        npu_smi_info
        echo "npu_smi_before_end"
    } > "$run_dir/meta.txt"

    set +e
    local start end rc
    start="$(date +%s)"
    (
        cd "$PROJECT_ROOT"
        export ASCEND_HOME_DIR
        export ASCEND_RT_VISIBLE_DEVICES="$card"
        export ASCEND_VISIBLE_DEVICES="$card"
        export ASCEND_DEVICE_ID=0
        export NPU_DEVICE_ID=0
        args=(
            --runner-mode "$RUNNER_MODE"
            --arch "$ARCH"
            --modes "$MODES"
            --jobs "$JOBS"
            --npu-slots "$NPU_SLOTS"
            --cpu-run-slots "$CPU_RUN_SLOTS"
            --make-jobs "$MAKE_JOBS"
            --timeout "$TIMEOUT"
            --cpu-run-timeout "$CPU_RUN_TIMEOUT"
            --schedule "$SCHEDULE"
            --report-format "$REPORT_FORMAT"
            --results "$run_dir/results"
        )
        if [[ -n "$SCHEDULE_FILE" ]]; then
            args+=(--schedule-file "$SCHEDULE_FILE")
        fi
        run_python_presmoke "${args[@]}" "$@"
    ) > "$run_dir/stdout.log" 2> "$run_dir/stderr.log"
    rc=$?
    end="$(date +%s)"
    set -e

    {
        echo "finished_at=$(date_iso)"
        echo "rc=$rc"
        echo "elapsed_sec=$((end - start))"
        source_cann
        echo "npu_smi_after_begin"
        npu_smi_info
        echo "npu_smi_after_end"
    } >> "$run_dir/meta.txt"
    log "run_done name=$name card=$card rc=$rc elapsed_sec=$((end - start))"
    return 0
}

timeout_examples() {
    local report="$1"
    [[ -f "$report" ]] || return
    run_python_module presmoke.orchestrate_report timeout-examples "$report"
}

retry_timeouts() {
    local full_report="$OUT_ROOT/full_card${PRIMARY_CARD}/results/report.json"
    [[ -f "$full_report" ]] || return
    local examples=()
    while IFS= read -r example; do
        examples+=("$example")
    done < <(timeout_examples "$full_report")
    if [[ "${#examples[@]}" -eq 0 ]]; then
        log "retry_skip no_timeouts"
        return
    fi
    if [[ -z "$RETRY_CARDS" ]]; then
        log "retry_skip retry_cards_empty count=${#examples[@]}"
        return
    fi

    log "retry_timeouts count=${#examples[@]}"
    local example card safe_name retry_report status
    for example in "${examples[@]}"; do
        [[ -n "$example" ]] || continue
        safe_name="${example//\//__}"
        for card in $RETRY_CARDS; do
            log "retry_start example=$example card=$card"
            run_presmoke "retry_${safe_name}_card${card}" "$card" --filter "$example"
            retry_report="$OUT_ROOT/retry_${safe_name}_card${card}/results/report.json"
            status="$(run_python_module presmoke.orchestrate_report retry-status "$retry_report")"
            log "retry_done example=$example card=$card status=$status"
            [[ "$status" == "PASS" ]] && break
        done
    done
}

write_final_report() {
    run_python_module presmoke.orchestrate_report write-final-report "$OUT_ROOT"
    log "final_report_written $OUT_ROOT/FINAL_REPORT.md"
}

main() {
    acquire_lock
    log "presmoke_start out_root=$OUT_ROOT"
    if should_clean_cann_vendors "$@"; then
        clean_cann_vendors
    else
        log "vendors_clean_skip reason=non_full_run args=$*"
    fi
    run_presmoke "full_card${PRIMARY_CARD}" "$PRIMARY_CARD" "$@"
    retry_timeouts
    write_final_report
    local effective_rc
    effective_rc="$(cat "$OUT_ROOT/effective_rc.txt" 2>/dev/null || echo 1)"
    log "presmoke_done out_root=$OUT_ROOT effective_rc=$effective_rc"
    run_python_module presmoke.orchestrate_report print-summary "$OUT_ROOT"
    if [[ "$effective_rc" -eq 0 ]]; then
        echo "execute samples success"
    else
        echo "execute samples failed" >&2
    fi
    exit "$effective_rc"
}

main "$@"
