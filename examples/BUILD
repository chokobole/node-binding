# Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:node_binding.bzl", "node_binding")
load("//bazel:node_binding_cc.bzl", "node_binding_copts")

node_binding(
    name = "calculator",
    srcs = [
        "calculator.cc",
        "calculator.h",
    ],
    copts = node_binding_copts(),
    deps = [
        "//:node_binding",
    ],
)

node_binding(
    name = "point",
    srcs = [
        "point.cc",
        "point.h",
    ],
    copts = node_binding_copts(),
    deps = [
        "//:node_binding",
    ],
)