#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from __future__ import annotations

import contextlib
import io
import tempfile
import threading
import time
import unittest
from pathlib import Path

from presmoke.case_runners import CaseRunnerOptions, build_case_runner_cells_with_skips
from presmoke.cli import parse_args, presmoke_run_lock
from presmoke.model import Cell, Command, ExampleSpec, RunReport
from presmoke.pool import NpuSlotPool
from presmoke.report import write_json, write_markdown
from presmoke.runner import PipelineOptions, RunOptions
from presmoke.runner import run_cell_with_options
from presmoke.runner import run_cells_pipeline_with_options


def cell(tmp: Path, name: str, mode: str = "npu", commands=None) -> Cell:
    spec = ExampleSpec(tmp / name, name, commands or [], ["dav-2201"], [mode], "test")
    spec.path.mkdir(parents=True, exist_ok=True)
    return Cell(spec, "dav-2201", mode, commands or [], spec.path / f"build_{mode}")


class RunnerPoolTest(unittest.TestCase):
    def test_cli_default_timeout_is_short_for_presmoke(self) -> None:
        args = parse_args([])

        self.assertEqual(args.timeout, 120)
        self.assertEqual(args.cpu_run_timeout, "300")

    def test_removed_cli_options_are_rejected(self) -> None:
        removed_options = [
            ["--preflight"],
            ["--baseline"],
            ["--update-baseline"],
            ["--allowlist", "x"],
            ["--changed-only"],
            ["--since", "HEAD~1"],
            ["--host-timeout", "1"],
        ]

        for option in removed_options:
            with self.subTest(option=option):
                with contextlib.redirect_stderr(io.StringIO()):
                    with self.assertRaises(SystemExit):
                        parse_args(option)

    def test_presmoke_run_lock_removes_stale_lock(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            lock_dir = root / ".presmoke_locks" / "presmoke_run.lock"
            lock_dir.mkdir(parents=True)
            (lock_dir / "pid").write_text("999999999", encoding="utf-8")

            with presmoke_run_lock(root):
                self.assertEqual((lock_dir / "pid").read_text(encoding="utf-8"), str(__import__("os").getpid()))

            self.assertFalse(lock_dir.exists())

    def test_runner_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))
        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.rc, 124)

    def test_host_stages_do_not_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(
                Path(tmp),
                "x",
                commands=[
                    Command("sleep 1.2", "build"),
                    Command(":", "run"),
                    Command("sleep 1.2", "verify"),
                ],
            )
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")
        self.assertEqual([step.rc for step in result.steps], [0, 0, 0])

    def test_cpu_run_stage_uses_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", mode="cpu", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")
        self.assertEqual(result.rc, 0)

    def test_cpu_run_timeout_can_be_overridden(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", mode="cpu", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, cpu_run_timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.rc, 124)

    def test_npu_gate_limits_concurrency(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            active = 0
            max_active = 0
            lock = threading.Lock()
            pool = NpuSlotPool(2)

            def work(c):
                nonlocal active, max_active
                command = Command("./demo", "run")

                def gated() -> int:
                    nonlocal active, max_active
                    with lock:
                        active += 1
                        max_active = max(max_active, active)
                    time.sleep(0.05)
                    with lock:
                        active -= 1
                    return 0

                pool.gate(c, command, gated)
                return c.key

            cells = [cell(Path(tmp), f"x{i}") for i in range(5)]
            threads = [threading.Thread(target=work, args=(c,)) for c in cells]
            for thread in threads:
                thread.start()
            for thread in threads:
                thread.join()
        self.assertLessEqual(max_active, 2)

    def test_manifest_skip_does_not_create_cell(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":true,"skip_reason":"needs TensorFlow 2.6.5","supported_archs":["dav-2201"],"supported_modes":["npu"]}]',
                encoding="utf-8",
            )
            cells, suggestions, skipped = build_case_runner_cells_with_skips(
                root,
                CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[]),
            )
        self.assertEqual(cells, [])
        self.assertEqual(len(skipped), 1)
        self.assertEqual(skipped[0].status, "SKIP")
        self.assertIn("TensorFlow 2.6.5", skipped[0].reason)
        self.assertEqual(len(suggestions), 1)

    def test_unsupported_arch_is_excluded_not_counted_as_skip(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":false,"supported_archs":["dav-3510"],"supported_modes":["npu"]}]',
                encoding="utf-8",
            )
            cells, suggestions, skipped = build_case_runner_cells_with_skips(
                root,
                CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[]),
            )
        self.assertEqual(cells, [])
        self.assertEqual(skipped, [])
        self.assertEqual(len(suggestions), 1)
        self.assertIn("Excluded on unsupported arch dav-2201", suggestions[0].message)

    def test_case_runner_cell_starts_with_clean_step(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":false,"supported_archs":["dav-2201"],"supported_modes":["npu"]}]',
                encoding="utf-8",
            )

            cells, _, _ = build_case_runner_cells_with_skips(
                root,
                CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[]),
            )

        self.assertEqual([command.kind for command in cells[0].commands], ["clean", "build", "run", "verify"])
        self.assertTrue(cells[0].commands[0].raw.endswith("run.sh clean"))

    def test_single_worker_run_removes_stale_build_dir_before_commands(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(
                root,
                "stale",
                commands=[
                    Command("test ! -e build_npu/stale", "build"),
                ],
            )
            c.build_dir.mkdir()
            (c.build_dir / "stale").write_text("old", encoding="utf-8")

            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")

    def test_custom_op_provider_build_dir_is_preserved_for_dependent_cases(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[
                    Command("mkdir -p build_npu && touch build_npu/custom_opp_test.run", "build"),
                    Command(":", "run"),
                    Command(":", "verify"),
                ],
            )

            result = run_cell_with_options(c, RunOptions(root / "logs", timeout=1, keep_artifacts=False))

            self.assertEqual(result.status, "PASS")
            self.assertTrue((c.build_dir / "custom_opp_test.run").exists())

    def test_pipeline_keeps_build_workers_running_while_npu_slot_is_busy(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker_dir = root / "markers"
            marker_dir.mkdir()
            cells = [
                cell(
                    root,
                    "slow-run",
                    commands=[
                        Command(":", "build"),
                        Command(f"while [ ! -f {marker_dir / 'third_build_started'} ]; do sleep 0.02; done", "run"),
                        Command(":", "verify"),
                    ],
                ),
                cell(
                    root,
                    "npu-queued",
                    commands=[
                        Command(":", "build"),
                        Command("sleep 0.05", "run"),
                        Command(":", "verify"),
                    ],
                ),
                cell(
                    root,
                    "third-build",
                    commands=[
                        Command(f"touch {marker_dir / 'third_build_started'}", "build"),
                        Command(":", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]
            run = run_cells_pipeline_with_options(cells, PipelineOptions(Path(tmp) / "logs", timeout=2, keep_artifacts=True, jobs=2, npu_slots=1))
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["slow-run"].status, "PASS")
        self.assertEqual(by_name["npu-queued"].status, "PASS")
        self.assertEqual(by_name["third-build"].status, "PASS")

    def test_pipeline_does_not_block_next_npu_run_on_verify(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker = root / "second_run_started"
            cells = [
                cell(
                    root,
                    "verify-waits-for-next-run",
                    commands=[
                        Command(":", "build"),
                        Command("sleep 0.01", "run"),
                        Command(f"while [ ! -f {marker} ]; do sleep 0.02; done", "verify"),
                    ],
                ),
                cell(
                    root,
                    "touches-verify-marker",
                    commands=[
                        Command("sleep 0.05", "build"),
                        Command(f"touch {marker}", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]

            run = run_cells_pipeline_with_options(cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["verify-waits-for-next-run"].status, "PASS")
        self.assertEqual(by_name["touches-verify-marker"].status, "PASS")

    def test_cpu_pipeline_runs_run_stages_with_job_parallelism(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker = root / "second_cpu_run_started"
            cells = [
                cell(
                    root,
                    "waits-for-other-cpu-run",
                    mode="cpu",
                    commands=[
                        Command(":", "build"),
                        Command(
                            f"for i in $(seq 1 50); do [ -f {marker} ] && exit 0; sleep 0.02; done; exit 7",
                            "run",
                        ),
                        Command(":", "verify"),
                    ],
                ),
                cell(
                    root,
                    "starts-other-cpu-run",
                    mode="cpu",
                    commands=[
                        Command(":", "build"),
                        Command(f"touch {marker}", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]

            run = run_cells_pipeline_with_options(cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["waits-for-other-cpu-run"].status, "PASS")
        self.assertEqual(by_name["starts-other-cpu-run"].status, "PASS")

    def test_pipeline_reports_npu_idle_and_busy_time(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "slow-build",
                    commands=[
                        Command("sleep 0.05", "build"),
                        Command("sleep 0.05", "run"),
                        Command(":", "verify"),
                    ],
                )
            ]

            run = run_cells_pipeline_with_options(cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))

        self.assertEqual(run.npu_stats.slots, 1)
        self.assertEqual(run.npu_stats.queue_model, "pipeline")
        self.assertGreater(run.npu_stats.idle_s, 0)
        self.assertGreater(run.npu_stats.busy_s, 0)
        self.assertGreater(run.npu_stats.utilization, 0)

    def test_report_writes_npu_stats(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "slow-build",
                    commands=[
                        Command("sleep 0.01", "build"),
                        Command("sleep 0.01", "run"),
                        Command(":", "verify"),
                    ],
                )
            ]
            run = run_cells_pipeline_with_options(cells, PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))
            report = RunReport("dav-2201", ["npu"], "start", "finish", run.results, [], run.npu_stats)
            json_path = root / "report.json"
            md_path = root / "report.md"

            write_json(report, json_path)
            write_markdown(report, md_path)

            json_text = json_path.read_text(encoding="utf-8")
            md_text = md_path.read_text(encoding="utf-8")

        self.assertIn('"npu_stats"', json_text)
        self.assertIn('"idle_s"', json_text)
        self.assertIn('"utilization"', json_text)
        self.assertIn("npu_idle_s", md_text)
        self.assertIn("npu_utilization", md_text)

    def test_cpu_report_labels_run_queue_stats(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "cpu-stats",
                    mode="cpu",
                    commands=[
                        Command(":", "build"),
                        Command("sleep 0.01", "run"),
                        Command(":", "verify"),
                    ],
                )
            ]
            run = run_cells_pipeline_with_options(cells, PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))
            report = RunReport("dav-2201", ["cpu"], "start", "finish", run.results, [], run.npu_stats)
            md_path = root / "report.md"

            write_markdown(report, md_path)
            md_text = md_path.read_text(encoding="utf-8")

        self.assertIn("cpu_run_queue_idle_s", md_text)
        self.assertIn("cpu_run_queue_utilization", md_text)
        self.assertNotIn("npu_idle_s", md_text)

    def test_pipeline_writes_per_stage_logs(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(
                root,
                "stage-log-case",
                commands=[
                    Command("echo build-stage", "build"),
                    Command("echo run-stage", "run"),
                    Command("echo verify-stage", "verify"),
                ],
            )

            run = run_cells_pipeline_with_options([c], PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1))
            result = run.results[0]

            build_log = root / "logs/stages/stage-log-case__npu__build.log"
            run_log = root / "logs/stages/stage-log-case__npu__run.log"
            verify_log = root / "logs/stages/stage-log-case__npu__verify.log"

            self.assertEqual(result.status, "PASS")
            self.assertEqual(set(result.stage_log_files), {str(build_log), str(run_log), str(verify_log)})
            self.assertIn("build-stage", build_log.read_text(encoding="utf-8"))
            self.assertIn("run-stage", run_log.read_text(encoding="utf-8"))
            self.assertIn("verify-stage", verify_log.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
