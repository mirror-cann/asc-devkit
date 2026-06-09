from __future__ import annotations

import unittest
from pathlib import Path

from presmoke.model import Command, ExampleSpec
from presmoke.planner import build_cells_with_skips, rewrite_cmake


class PlannerTest(unittest.TestCase):
    def test_rewrite_cmake_replaces_arch_and_mode(self) -> None:
        cmd = rewrite_cmake("cmake -DSCENARIO_NUM=4 -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=cpu ..", "dav-3510", "sim")
        self.assertIn("-DSCENARIO_NUM=4", cmd)
        self.assertIn("-DCMAKE_ASC_ARCHITECTURES=dav-3510", cmd)
        self.assertIn("-DCMAKE_ASC_RUN_MODE=sim", cmd)
        self.assertNotIn("dav-2201", cmd)
        self.assertNotIn("cpu", cmd)

    def test_rewrite_cmake_omits_npu_mode_flag(self) -> None:
        cmd = rewrite_cmake("cmake ..", "dav-2201", "npu")
        self.assertIn("-DCMAKE_ASC_ARCHITECTURES=dav-2201", cmd)
        self.assertNotIn("CMAKE_ASC_RUN_MODE", cmd)

    def test_unsupported_arch_is_excluded_not_counted_as_skip(self) -> None:
        spec = ExampleSpec(
            Path("/tmp/example"),
            "example",
            [Command("cmake ..", "cmake")],
            ["dav-3510"],
            ["npu"],
            "readme",
        )
        cells, suggestions, skipped = build_cells_with_skips([spec], "dav-2201", ["npu"])
        self.assertEqual(cells, [])
        self.assertEqual(len(suggestions), 1)
        self.assertEqual(skipped, [])
        self.assertIn("Excluded on unsupported arch dav-2201", suggestions[0].message)


if __name__ == "__main__":
    unittest.main()
