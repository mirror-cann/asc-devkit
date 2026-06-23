#!/usr/bin/env bash
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

set -euo pipefail

LYCHEE_VERSION="${LYCHEE_VERSION:-0.23.0}"
MARKDOWN_LINK_CHECK_FULL="${MARKDOWN_LINK_CHECK_FULL:-0}"
MARKDOWN_LINK_CHECK_DRY_RUN="${MARKDOWN_LINK_CHECK_DRY_RUN:-0}"
MARKDOWN_LINK_CHECK_VERBOSE="${MARKDOWN_LINK_CHECK_VERBOSE:-0}"
MARKDOWN_LINK_CHECK_ONLINE="${MARKDOWN_LINK_CHECK_ONLINE:-0}"
MARKDOWN_LINK_CHECK_START=$SECONDS

REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null || pwd)
cd "$REPO_ROOT"

CONFIG_FILE="$REPO_ROOT/markdown-link-check.toml"
if [ ! -f "$CONFIG_FILE" ]; then
    echo "[Markdown check] Missing config: $CONFIG_FILE" >&2
    exit 1
fi

usage() {
    cat <<'EOF'
Usage: scripts/markdown_link_check.sh [--full|--all] [markdown-file...]

By default, checks changed Markdown files and Markdown files that link to them.
Set MARKDOWN_LINK_CHECK_FULL=1 or pass --full to scan tracked Markdown files in the repository.
Set MARKDOWN_LINK_CHECK_DRY_RUN=1 to print the computed scan set only.
Set MARKDOWN_LINK_CHECK_LYCHEE to use a preinstalled lychee binary.
Set MARKDOWN_LINK_CHECK_ONLINE=1 to check remote HTTP(S) links; remote links are skipped by default.
Set MARKDOWN_LINK_CHECK_LYCHEE_ARCHIVE_URL, MARKDOWN_LINK_CHECK_CARGO_BINSTALL_INSTALLER_URL, or
MARKDOWN_LINK_CHECK_LYCHEE_PKG_URL to use mirrors.
EOF
}

markdown_check_duration() {
    printf '%ss' "$((SECONDS - MARKDOWN_LINK_CHECK_START))"
}

download_lychee_archive() {
    local target archive_url tmp_dir version_placeholder target_placeholder

    case "$(uname -s):$(uname -m)" in
        Linux:x86_64|Linux:amd64)
            target="x86_64-unknown-linux-gnu"
            ;;
        Linux:aarch64|Linux:arm64)
            target="aarch64-unknown-linux-gnu"
            ;;
        *)
            return 1
            ;;
    esac

    if [ -n "${MARKDOWN_LINK_CHECK_LYCHEE_ARCHIVE_URL:-}" ]; then
        archive_url="$MARKDOWN_LINK_CHECK_LYCHEE_ARCHIVE_URL"
    else
        archive_url="https://gh.llkk.cc/https://github.com/lycheeverse/lychee/releases/download/lychee-v{version}/lychee-{target}.tar.gz"
    fi
    version_placeholder="{version}"
    target_placeholder="{target}"
    archive_url="${archive_url//$version_placeholder/$LYCHEE_VERSION}"
    archive_url="${archive_url//$target_placeholder/$target}"
    tmp_dir=$(mktemp -d)
    echo "[Markdown check] Downloading lychee@$LYCHEE_VERSION from $archive_url..." >&2
    if ! curl -L --proto '=https' --tlsv1.2 -sSf "$archive_url" -o "$tmp_dir/lychee.tar.gz"; then
        rm -rf "$tmp_dir"
        return 1
    fi
    if ! tar -xzf "$tmp_dir/lychee.tar.gz" -C "$tmp_dir" lychee; then
        rm -rf "$tmp_dir"
        return 1
    fi
    install -m 0755 "$tmp_dir/lychee" "$LYCHEE_BIN"
    rm -rf "$tmp_dir"
    return 0
}

FULL_SCAN="$MARKDOWN_LINK_CHECK_FULL"
INPUT_FILES=()
while [ "$#" -gt 0 ]; do
    case "$1" in
        --all|--full)
            FULL_SCAN=1
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            INPUT_FILES+=("$1")
            ;;
    esac
    shift
done

find_lychee() {
    if [ -n "${MARKDOWN_LINK_CHECK_LYCHEE:-}" ] && [ -x "$MARKDOWN_LINK_CHECK_LYCHEE" ]; then
        printf '%s\n' "$MARKDOWN_LINK_CHECK_LYCHEE"
        return 0
    fi

    if command -v lychee >/dev/null 2>&1; then
        command -v lychee
        return 0
    fi

    GIT_COMMON_DIR=$(git rev-parse --git-common-dir 2>/dev/null || printf '.git')
    case "$GIT_COMMON_DIR" in
        /*) CACHE_ROOT="$GIT_COMMON_DIR/markdown-link-check" ;;
        *) CACHE_ROOT="$REPO_ROOT/$GIT_COMMON_DIR/markdown-link-check" ;;
    esac

    CARGO_HOME_DIR="$CACHE_ROOT/cargo"
    CARGO_BIN="$CARGO_HOME_DIR/bin"
    LYCHEE_BIN="$CARGO_BIN/lychee-$LYCHEE_VERSION"

    if [ -x "$LYCHEE_BIN" ]; then
        printf '%s\n' "$LYCHEE_BIN"
        return 0
    fi

    mkdir -p "$CARGO_BIN"
    export CARGO_HOME="$CARGO_HOME_DIR"
    export PATH="$CARGO_BIN:$PATH"

    if download_lychee_archive; then
        printf '%s\n' "$LYCHEE_BIN"
        return 0
    fi

    if command -v cargo-binstall >/dev/null 2>&1; then
        BINSTALL=(cargo-binstall)
    elif command -v cargo >/dev/null 2>&1 && cargo binstall -V >/dev/null 2>&1; then
        BINSTALL=(cargo binstall)
    else
        echo "[Markdown check] Installing cargo-binstall..." >&2
        CARGO_BINSTALL_INSTALLER_URL="${MARKDOWN_LINK_CHECK_CARGO_BINSTALL_INSTALLER_URL:-https://raw.githubusercontent.com/cargo-bins/cargo-binstall/main/install-from-binstall-release.sh}"
        curl -L --proto '=https' --tlsv1.2 -sSf \
            "$CARGO_BINSTALL_INSTALLER_URL" \
            | CARGO_HOME="$CARGO_HOME_DIR" BASH_XTRACEFD=7 bash 7>/dev/null >&2
        BINSTALL=("$CARGO_BIN/cargo-binstall")
    fi

    echo "[Markdown check] Installing lychee@$LYCHEE_VERSION..." >&2
    BINSTALL_ARGS=(-y --install-path "$CARGO_BIN")
    if [ -n "${MARKDOWN_LINK_CHECK_LYCHEE_PKG_URL:-}" ]; then
        BINSTALL_ARGS+=(--pkg-url "$MARKDOWN_LINK_CHECK_LYCHEE_PKG_URL")
    fi
    BINSTALL_ARGS+=("lychee@$LYCHEE_VERSION")
    "${BINSTALL[@]}" "${BINSTALL_ARGS[@]}" >&2
    if [ ! -x "$CARGO_BIN/lychee" ]; then
        echo "[Markdown check] lychee binary was not installed." >&2
        exit 1
    fi
    mv -f "$CARGO_BIN/lychee" "$LYCHEE_BIN"
    printf '%s\n' "$LYCHEE_BIN"
}

SCAN_SET_FILE=$(mktemp)
LYCHEE_OUTPUT_FILE=$(mktemp)
FILTERED_OUTPUT_FILE=$(mktemp)
CROSS_LINK_OUTPUT_FILE=$(mktemp)
trap 'rm -f "$SCAN_SET_FILE" "$LYCHEE_OUTPUT_FILE" "$FILTERED_OUTPUT_FILE" "$CROSS_LINK_OUTPUT_FILE"' EXIT

PY_ARGS=("$REPO_ROOT" "$SCAN_SET_FILE")
if [ "${#INPUT_FILES[@]}" -gt 0 ]; then
    PY_ARGS+=("${INPUT_FILES[@]}")
fi

FULL_SCAN="$FULL_SCAN" python3 - "${PY_ARGS[@]}" <<'PY'
import os
import posixpath
import re
import subprocess
import sys
from pathlib import Path, PurePosixPath
from urllib.parse import unquote

repo_root = Path(sys.argv[1])
output_file = Path(sys.argv[2])
input_files = sys.argv[3:]

markdown_exts = (".md", ".markdown", ".mdx")
excluded_prefixes = ()
config_file = repo_root / "markdown-link-check.toml"
full_scan = os.environ.get("FULL_SCAN") == "1"
full_reason = ""


def ci_filelist_inputs():
    filelist = repo_root / "pr_filelist_precommit.txt"
    try:
        return [
            line.strip()
            for line in filelist.read_text(encoding="utf-8", errors="ignore").splitlines()
            if line.strip()
        ]
    except OSError:
        return []


input_files = list(dict.fromkeys(input_files + ci_filelist_inputs()))


def config_exclude_path_patterns():
    try:
        text = config_file.read_text(encoding="utf-8")
    except OSError:
        return []

    match = re.search(r"(?ms)^exclude_path\s*=\s*\[(.*?)\]", text)
    if not match:
        return []

    patterns = []
    for raw_match in re.finditer(r'"((?:\\.|[^"\\])*)"', match.group(1)):
        pattern = raw_match.group(1).replace(r"\\", "\\").replace(r"\"", '"')
        try:
            patterns.append(re.compile(pattern))
        except re.error:
            continue
    return patterns


exclude_path_patterns = config_exclude_path_patterns()


def to_rel(path):
    p = Path(path)
    if p.is_absolute():
        try:
            return p.resolve().relative_to(repo_root.resolve()).as_posix()
        except ValueError:
            return None
    return PurePosixPath(path).as_posix()


def is_markdown_path(path):
    if path is None:
        return False
    return (
        path.lower().endswith(markdown_exts)
        and not path.startswith(excluded_prefixes)
        and not any(pattern.search(path) for pattern in exclude_path_patterns)
    )


def existing_markdown_files():
    try:
        result = subprocess.run(
            ["git", "ls-files", "-z", "*.md", "*.markdown", "*.mdx"],
            cwd=repo_root,
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except OSError:
        result = None

    if result is not None and result.returncode == 0:
        return sorted(
            path.decode("utf-8", errors="ignore")
            for path in result.stdout.split(b"\0")
            if path and is_markdown_path(path.decode("utf-8", errors="ignore"))
        )

    files = []
    for path in repo_root.rglob("*"):
        if ".git" in path.parts:
            continue
        if not path.is_file() or not path.name.lower().endswith(markdown_exts):
            continue
        rel_path = path.relative_to(repo_root).as_posix()
        if is_markdown_path(rel_path):
            files.append(rel_path)
    return sorted(files)


def staged_markdown_changes():
    try:
        result = subprocess.run(
            ["git", "diff", "--cached", "--name-status", "--diff-filter=ADMR"],
            cwd=repo_root,
            check=False,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except OSError:
        return [], False

    changed = []
    needs_full = False
    for line in result.stdout.splitlines():
        parts = line.split("\t")
        if not parts:
            continue
        status = parts[0]
        paths = parts[1:]
        markdown_paths = [p for p in paths if is_markdown_path(p)]
        if not markdown_paths:
            continue
        if status.startswith(("D", "R")):
            needs_full = True
        for path in markdown_paths:
            if status.startswith("D"):
                continue
            changed.append(path)
    return changed, needs_full


def markdown_inline_link_targets(text):
    index = 0
    while index < len(text):
        marker = text.find("](", index)
        if marker == -1:
            break

        start = marker + 2
        cursor = start
        depth = 0
        escaped = False
        while cursor < len(text):
            char = text[cursor]
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == "(":
                depth += 1
            elif char == ")":
                if depth == 0:
                    raw_target = text[start:cursor].strip()
                    if raw_target.startswith("<") and ">" in raw_target:
                        yield raw_target[1:raw_target.find(">")]
                    elif raw_target:
                        yield raw_target.split()[0]
                    break
                depth -= 1
            cursor += 1
        index = cursor + 1 if cursor < len(text) else marker + 2


def extract_link_targets(text):
    # Inline Markdown links/images: [text](target) or ![alt](target)
    yield from markdown_inline_link_targets(text)

    # Reference definitions: [label]: target
    for match in re.finditer(r"(?m)^\s*\[[^\]]+\]:\s*(\S+)", text):
        yield match.group(1)

    # Raw HTML links used by some generated docs.
    for match in re.finditer(r"""(?i)\bhref\s*=\s*["']([^"']+)["']""", text):
        yield match.group(1)


def normalize_target(source_file, target):
    target = target.strip().strip("<>").strip("\"'")
    if not target or target.startswith("#"):
        return None
    if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", target):
        return None

    target = target.split("#", 1)[0].split("?", 1)[0]
    if not target:
        return None

    target = unquote(target)
    if target.startswith("/"):
        normalized = posixpath.normpath(target.lstrip("/"))
    else:
        normalized = posixpath.normpath(posixpath.join(posixpath.dirname(source_file), target))
    return normalized


if not full_scan:
    staged_changes, needs_full_for_staged = staged_markdown_changes()
    if needs_full_for_staged:
        full_scan = True
        full_reason = "staged Markdown rename/delete"

if full_scan:
    scan_set = existing_markdown_files()
else:
    changed = [to_rel(path) for path in input_files]
    changed = [path for path in changed if is_markdown_path(path)]
    if not changed:
        staged_changes, _ = staged_markdown_changes()
        changed = staged_changes

    changed_targets = {path for path in changed if path}
    changed_existing = {
        path for path in changed_targets
        if (repo_root / path).is_file()
    }

    all_markdown = existing_markdown_files()
    scan_set = set(changed_existing)
    for source in all_markdown:
        source_path = repo_root / source
        try:
            text = source_path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            text = source_path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue

        for target in extract_link_targets(text):
            normalized = normalize_target(source, target)
            if normalized in changed_targets:
                scan_set.add(source)
                break

    scan_set = sorted(scan_set)

with output_file.open("w", encoding="utf-8") as handle:
    if full_scan:
        full_reason = full_reason or "requested"
        handle.write(f"# full scan: {full_reason}\n")
    for item in scan_set:
        handle.write(item + "\n")
PY

SCAN_FILES=()
FULL_REASON=""
while IFS= read -r item; do
    case "$item" in
        "# full scan:"*)
            FULL_REASON="${item#"# full scan: "}"
            ;;
        "")
            ;;
        *)
            SCAN_FILES+=("$item")
            ;;
    esac
done < "$SCAN_SET_FILE"

if [ "${#SCAN_FILES[@]}" -eq 0 ]; then
    echo "[Markdown check] PASS: no Markdown files to check. duration=$(markdown_check_duration)"
    exit 0
fi

if [ "$MARKDOWN_LINK_CHECK_DRY_RUN" = "1" ]; then
    printf '%s\n' "${SCAN_FILES[@]}"
    echo "[Markdown check] DRY-RUN: ${#SCAN_FILES[@]} file(s) selected. duration=$(markdown_check_duration)" >&2
    exit 0
fi

set +e
python3 - "$REPO_ROOT" "$SCAN_SET_FILE" "$CROSS_LINK_OUTPUT_FILE" "$(markdown_check_duration)" <<'PY'
import posixpath
import re
import sys
from pathlib import Path
from urllib.parse import unquote

repo_root = Path(sys.argv[1])
scan_set_file = Path(sys.argv[2])
output_file = Path(sys.argv[3])
duration = sys.argv[4]
repo_url = "https://gitcode.com/cann/asc-devkit"

reference_pattern = re.compile(r"(?m)^(\s*\[[^\]]+\]:\s*)(\S+)")
html_href_pattern = re.compile(r"""(?i)(\bhref\s*=\s*["'])([^"']+)(["'])""")


def markdown_inline_link_targets(text):
    index = 0
    while index < len(text):
        marker = text.find("](", index)
        if marker == -1:
            break

        start = marker + 2
        cursor = start
        depth = 0
        escaped = False
        while cursor < len(text):
            char = text[cursor]
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == "(":
                depth += 1
            elif char == ")":
                if depth == 0:
                    raw_target = text[start:cursor].strip()
                    if raw_target.startswith("<") and ">" in raw_target:
                        yield raw_target[1:raw_target.find(">")]
                    elif raw_target:
                        yield raw_target.split()[0]
                    break
                depth -= 1
            cursor += 1
        index = cursor + 1 if cursor < len(text) else marker + 2


def source_area(path):
    if path.startswith("docs/api/"):
        return "api"
    if path.startswith("docs/guide/"):
        return "guide"
    if path.startswith("examples/"):
        return "examples"
    return None


def split_target(target):
    target = target.strip().strip("<>").strip("\"'")
    if not target or target.startswith("#"):
        return None
    if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", target):
        return None

    suffix_index = len(target)
    for sep in ("#", "?"):
        index = target.find(sep)
        if index != -1:
            suffix_index = min(suffix_index, index)
    path_part = target[:suffix_index]
    suffix = target[suffix_index:]
    if not path_part:
        return None
    return path_part, suffix


def normalize_target(source_file, target):
    parts = split_target(target)
    if parts is None:
        return None

    path_part, suffix = parts
    decoded = unquote(path_part)
    if decoded.startswith("/"):
        normalized = posixpath.normpath(decoded.lstrip("/"))
    else:
        normalized = posixpath.normpath(posixpath.join(posixpath.dirname(source_file), decoded))
    return normalized, suffix


def gitcode_url(target_path, suffix):
    view = "tree" if (repo_root / target_path).is_dir() else "blob"
    return f"{repo_url}/{view}/master/{target_path}{suffix}"


def file_url(target_path, suffix):
    readme_path = (repo_root / target_path / "README.md")
    if readme_path.is_file():
        return f"{target_path.rstrip('/')}/README.md{suffix}"
    return f"{target_path}{suffix}"


def iter_links(line):
    yield from markdown_inline_link_targets(line)
    for match in reference_pattern.finditer(line):
        yield match.group(2)
    for match in html_href_pattern.finditer(line):
        yield match.group(2)


scan_files = []
for raw_line in scan_set_file.read_text(encoding="utf-8").splitlines():
    if not raw_line or raw_line.startswith("# full scan:"):
        continue
    scan_files.append(raw_line)

errors = []
for source in scan_files:
    source_kind = source_area(source)
    if source_kind is None:
        continue

    source_path = repo_root / source
    try:
        lines = source_path.read_text(encoding="utf-8").splitlines()
    except UnicodeDecodeError:
        lines = source_path.read_text(encoding="utf-8", errors="ignore").splitlines()
    except OSError:
        continue

    for line_number, line in enumerate(lines, 1):
        for target in iter_links(line):
            normalized = normalize_target(source, target)
            if normalized is None:
                continue

            target_path, suffix = normalized
            if (repo_root / target_path).is_dir():
                if source_kind == "examples":
                    if "#" in suffix:
                        errors.append(
                            f"{source}:{line_number}: examples directory link with anchor must target a file: "
                            f"{target} -> {file_url(target_path, suffix)}"
                        )
                else:
                    errors.append(
                        f"{source}:{line_number}: docs Markdown link must target a file, "
                        f"not a directory: {target} -> {file_url(target_path, suffix)}"
                    )
                continue

            target_kind = source_area(target_path)
            if target_kind is None or target_kind == source_kind:
                continue

            errors.append(
                f"{source}:{line_number}: docs/examples cross-area link must use HTTPS: "
                f"{target} -> {gitcode_url(target_path, suffix)}"
            )

with output_file.open("w", encoding="utf-8") as handle:
    if errors:
        handle.write(
            f"[Markdown check] FAILED: {len(errors)} Markdown link policy item(s), "
            f"{len(scan_files)} file(s) checked, duration={duration}.\n\nFailed:\n"
        )
        for error in errors:
            handle.write(error + "\n")

sys.exit(1 if errors else 0)
PY
CROSS_LINK_RC=$?
set -e
if [ "$CROSS_LINK_RC" -ne 0 ]; then
    cat "$CROSS_LINK_OUTPUT_FILE"
    exit "$CROSS_LINK_RC"
fi

LYCHEE_BIN=$(find_lychee)
LYCHEE_ARGS=(--no-progress --config "$CONFIG_FILE")
if [ "$MARKDOWN_LINK_CHECK_ONLINE" != "1" ]; then
    LYCHEE_ARGS+=(--offline)
fi
set +e
"$LYCHEE_BIN" "${LYCHEE_ARGS[@]}" "${SCAN_FILES[@]}" >"$LYCHEE_OUTPUT_FILE" 2>&1
LYCHEE_RC=$?
set -e

LYCHEE_DURATION=$(markdown_check_duration)
python3 - "$LYCHEE_OUTPUT_FILE" "$FILTERED_OUTPUT_FILE" "$LYCHEE_RC" "${#SCAN_FILES[@]}" "$LYCHEE_DURATION" <<'PY'
import re
import sys
from pathlib import Path

output_path = Path(sys.argv[1])
filtered_path = Path(sys.argv[2])
return_code = int(sys.argv[3])
file_count = int(sys.argv[4])
duration = sys.argv[5]

lines = output_path.read_text(encoding="utf-8", errors="replace").splitlines()
error_events = []
warning_events = []
current_source = None


def classify(line):
    if re.search(r"\[(WARN|WARNING)\]", line, re.IGNORECASE):
        return "warning"
    if re.search(r"\[(ERROR|FAIL|FAILED)\]", line, re.IGNORECASE):
        return "error"

    status_match = re.match(r"^\[(\d{3})\]\s+.+\s\|", line)
    if status_match and int(status_match.group(1)) >= 400:
        return "error"
    return None


def format_event(line, source):
    location_match = re.search(r"\(at (\d+):(\d+)\)", line)
    if source and location_match:
        clean_line = re.sub(r"\s+\(at \d+:\d+\)", "", line)
        return f"{source}:{location_match.group(1)}:{location_match.group(2)}: {clean_line}", True
    if source:
        return f"{source}: {line}", True
    return line, False


for line in lines:
    stripped = line.strip()
    if not stripped:
        continue

    source_match = re.match(r"^\[([^\]]+)\]:$", stripped)
    if source_match and not re.match(r"^\[\d{3}\]", stripped):
        current_source = source_match.group(1)
        continue

    event_type = classify(stripped)
    if event_type == "error":
        error_events.append(format_event(stripped, current_source))
    elif event_type == "warning":
        warning_events.append(format_event(stripped, current_source))


def unique_events(events):
    seen = set()
    result = []
    for text, has_source in events:
        if text in seen:
            continue
        result.append((text, has_source))
        seen.add(text)
    return result


def prefer_sourced(events):
    events = unique_events(events)
    sourced = [event for event in events if event[1]]
    return [text for text, _ in (sourced if sourced else events)]


error_lines = prefer_sourced(error_events)
warning_lines = prefer_sourced(warning_events)

with filtered_path.open("w", encoding="utf-8") as handle:
    if return_code != 0:
        handle.write(
            f"[Markdown check] FAILED: {len(error_lines)} failed item(s), "
            f"{len(warning_lines)} warning item(s), {file_count} file(s) checked, "
            f"duration={duration}.\n"
        )
        if error_lines:
            handle.write("\nFailed:\n")
            for line in error_lines:
                handle.write(f"{line}\n")
        else:
            handle.write("\nFailed:\n")
            for line in lines:
                if line.strip():
                    handle.write(f"{line.strip()}\n")
    elif warning_lines:
        handle.write(
            f"[Markdown check] WARNING: {len(warning_lines)} warning item(s), "
            f"{file_count} file(s) checked, duration={duration}.\n\nWarnings:\n"
        )

    if warning_lines:
        if return_code != 0:
            handle.write("\nWarnings:\n")
        for line in warning_lines:
            handle.write(f"{line}\n")
PY

if [ -s "$FILTERED_OUTPUT_FILE" ]; then
    cat "$FILTERED_OUTPUT_FILE"
elif [ "$LYCHEE_RC" -eq 0 ]; then
    echo "[Markdown check] PASS: ${#SCAN_FILES[@]} file(s) checked. duration=$(markdown_check_duration)"
fi

exit "$LYCHEE_RC"
