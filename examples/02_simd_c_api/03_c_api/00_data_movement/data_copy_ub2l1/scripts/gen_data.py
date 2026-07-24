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

import argparse
import os

import numpy as np


M = 128
K = 128
N = 128
C0_SIZE = 16


def nd_to_nz(data_nd):
    rows, columns = data_nd.shape
    return data_nd.reshape(
        rows // C0_SIZE, C0_SIZE, columns // C0_SIZE, C0_SIZE
    ).transpose(2, 0, 1, 3)


def generate_data(scenario_num=1):
    np.random.seed(9)
    a_nd = np.random.uniform(-1, 1, (M, K)).astype(np.float16)
    b_nd = np.random.uniform(-1, 1, (K, N)).astype(np.float16)
    golden = np.matmul(a_nd.astype(np.float32), b_nd.astype(np.float32)).astype(
        np.float32
    )

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    if scenario_num == 1:
        nd_to_nz(a_nd).tofile("./input/x1_gm.bin")
        nd_to_nz(b_nd).tofile("./input/x2_gm.bin")
    else:
        a_nd.tofile("./input/x1_gm.bin")
        b_nd.tofile("./input/x2_gm.bin")
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=(1, 2))
    args = parser.parse_args()
    generate_data(args.scenarioNum)
