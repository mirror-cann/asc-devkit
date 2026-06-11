#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import os
import torch
import numpy as np
import argparse

def gen_golden_data_simple(scenario):
    dtype = np.float32
    np.random.seed(123321)
    src_type = dtype
    src_shape = [16]
    src1 = np.random.uniform(-10, 10, src_shape).astype(src_type)
    src2 = np.random.uniform(-10, 10, src_shape).astype(src_type)
    golden = np.zeros(src_shape).astype(src_type)
    src1_tmp = src1.astype(np.float32)
    src1_tensor = torch.from_numpy(src1_tmp)
    src2_tmp = src2.astype(np.float32)
    src2_tensor = torch.from_numpy(src2_tmp)

    if scenario == 0:
        golden = torch.pow(src1_tensor, src2_tensor).numpy()
    elif scenario == 1:
        golden = torch.pow(src1_tensor, src2_tensor[0]).numpy()
    elif scenario == 2:
        golden = torch.pow(src1_tensor[0], src2_tensor).numpy()
    golden = golden.astype(src1.dtype)

    os.makedirs("input", exist_ok=True)
    src1.tofile("./input/input_base.bin")
    src2.tofile("./input/input_exp.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--scenario', type=int, choices=range(3), default=0, help='指定场景，取值0~2')
    args = parser.parse_args()
    gen_golden_data_simple(args.scenario)
