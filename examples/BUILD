# Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:node_binding.bzl", "node_binding")

node_binding(
    name = "calculator",
    srcs = ["calculator.cc"],
    copts = select({
        "//:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    }),
    deps = [
        "//:node_binding",
    ],
)
