from __future__ import annotations

import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


class OrchestrateShellTest(unittest.TestCase):
    def base_env(self, root: Path, **overrides: str) -> dict[str, str]:
        env = {
            **os.environ,
            "PATH": f"{root / 'bin'}{os.pathsep}{os.environ.get('PATH', '')}",
            "REAL_PYTHON": shutil.which("python3") or "python3",
        }
        env.update(overrides)
        return env

    def copy_orchestrate_fixture(self, root: Path) -> Path:
        project_root = Path(__file__).resolve().parents[3]
        script = root / "scripts/run_presmoke_v2.sh"
        script.parent.mkdir(parents=True)
        shutil.copy(project_root / "scripts/run_presmoke_v2.sh", script)
        script.chmod(0o755)

        presmoke_pkg = root / "scripts/presmoke"
        presmoke_pkg.mkdir(parents=True)
        shutil.copy(project_root / "scripts/presmoke/orchestrate_report.py", presmoke_pkg / "orchestrate_report.py")
        (presmoke_pkg / "__init__.py").write_text("", encoding="utf-8")

        fake_bin = root / "bin"
        fake_bin.mkdir()
        fake_python = fake_bin / "python3"
        fake_python.write_text(
            """#!/usr/bin/env bash
set -euo pipefail
if [[ "${1:-}" != "-m" || "${2:-}" != "presmoke" ]]; then
    exec "${REAL_PYTHON}" "$@"
fi
shift 2
results=""
while [[ $# -gt 0 ]]; do
    if [[ "$1" == "--results" ]]; then
        results="$2"
        shift 2
        continue
    fi
    shift
done
mkdir -p "$results"
if [[ "${PRESMOKE_STUB_RC:-0}" == "0" ]]; then
cat > "$results/report.json" <<'JSON'
{"summary":{"PASS":0,"FAIL":0,"SKIP":0},"results":[],"npu_stats":{"busy_s":9,"idle_s":1,"utilization":0.9}}
JSON
else
cat > "$results/report.json" <<'JSON'
{"summary":{"PASS":0,"FAIL":1,"SKIP":0},"results":[{"example":"stub/fail","arch":"dav-2201","mode":"npu","status":"FAIL","reason":"nonzero rc","rc":1,"duration_s":1,"steps":[]}],"npu_stats":{"busy_s":0,"idle_s":0,"utilization":0}}
JSON
fi
cat > "$results/report.md" <<'MD'
# stub
MD
exit "${PRESMOKE_STUB_RC:-0}"
""",
            encoding="utf-8",
        )
        fake_python.chmod(0o755)
        return script

    def test_full_run_cleans_cann_vendors_before_presmoke(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(["bash", str(script)], text=True, capture_output=True, env=env, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertFalse(stale.exists())
            self.assertIn("vendors_clean_done", result.stdout)
            self.assertIn("Presmoke Summary:", result.stdout)
            self.assertIn("Elapsed:", result.stdout)
            self.assertIn("Cases: total=0 pass=0 fail=0 skip=0", result.stdout)
            self.assertIn("npu: busy=9.000s idle=1.000s util=90.000%", result.stdout)
            self.assertIn("execute samples success", result.stdout)
            self.assertTrue((root / "out/junit.xml").exists())

    def test_default_output_root_is_presmoke_reports(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                ARCH="dav-2201",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(["bash", str(script), "--dry-run"], text=True, capture_output=True, env=env, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn(f"out_root={root / 'presmoke_reports' / 'presmoke_dav-2201_'}", result.stdout)
            self.assertFalse((root / "presmoke_results").exists())

    def test_cpu_mode_uses_default_schedule(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                MODES="cpu",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(["bash", str(script), "--dry-run"], text=True, capture_output=True, env=env, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("schedule=default", result.stdout)

    def test_mode_alias_enables_cpu_mode(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                MODE="cpu",
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(["bash", str(script), "--dry-run"], text=True, capture_output=True, env=env, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("modes=cpu", result.stdout)
            self.assertIn("schedule=default", result.stdout)

    def test_filtered_run_does_not_clean_cann_vendors(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--filter", "x"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(stale.exists())
            self.assertIn("vendors_clean_skip reason=non_full_run", result.stdout)

    def test_dry_run_does_not_clean_cann_vendors(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            vendors = root / "opp/vendors"
            stale = vendors / "stale_vendor"
            stale.mkdir(parents=True)
            (stale / "old.txt").write_text("old", encoding="utf-8")
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
            )

            result = subprocess.run(
                ["bash", str(script), "--dry-run"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(stale.exists())
            self.assertIn("vendors_clean_skip reason=non_full_run", result.stdout)

    def test_failed_presmoke_keeps_legacy_failure_marker(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            script = self.copy_orchestrate_fixture(root)
            env = self.base_env(
                root,
                PROJECT_ROOT=str(root),
                OUT_ROOT=str(root / "out"),
                ASCEND_OPP_PATH=str(root / "opp"),
                PRESMOKE_STUB_RC="1",
            )

            result = subprocess.run(["bash", str(script)], text=True, capture_output=True, env=env, check=False)

            self.assertEqual(result.returncode, 1)
            self.assertIn("Presmoke Summary:", result.stdout)
            self.assertIn("Cases: total=1 pass=0 fail=1 skip=0", result.stdout)
            self.assertIn("execute samples failed", result.stderr)

    def test_make_wrapper_limits_explicit_parallel_jobs(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            fake_bin = root / "bin"
            fake_bin.mkdir()
            capture = root / "args.txt"
            fake_make = fake_bin / "make"
            fake_make.write_text(
                f"#!/usr/bin/env bash\nprintf '%s\\n' \"$@\" > {capture}\n",
                encoding="utf-8",
            )
            fake_make.chmod(0o755)
            project_root = Path(__file__).resolve().parents[3]
            wrapper = project_root / "scripts/presmoke/wrappers/make"
            env = {
                **os.environ,
                "PATH": f"{wrapper.parent}{os.pathsep}{fake_bin}{os.pathsep}{os.environ.get('PATH', '')}",
                "PRESMOKE_MAKE_JOBS": "16",
            }

            result = subprocess.run(
                [str(wrapper), "-j", "binary", "package"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(capture.read_text(encoding="utf-8").splitlines(), ["-j16", "binary", "package"])

            result = subprocess.run(
                [str(wrapper), "-j", "8", "binary"],
                text=True,
                capture_output=True,
                env=env,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(capture.read_text(encoding="utf-8").splitlines(), ["-j16", "binary"])


if __name__ == "__main__":
    unittest.main()
