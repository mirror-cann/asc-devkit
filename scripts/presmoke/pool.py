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

import threading
import time
from typing import Callable

from .model import Cell, Command


class NpuSlotPool:
    def __init__(self, slots: int) -> None:
        if slots <= 0:
            raise ValueError("npu slots must be positive")
        self._sem = threading.BoundedSemaphore(slots)

    def gate(self, cell: Cell, command: Command, fn: Callable[[], int]) -> tuple[int, float]:
        started = time.monotonic()
        with self._sem:
            wait_s = time.monotonic() - started
            return fn(), wait_s
