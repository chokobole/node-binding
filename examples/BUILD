# Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:node_binding.bzl", "node_binding")
load("//bazel:node_binding_cc.bzl", "node_binding_copts")

cc_library(
    name = "calculator",
    hdrs = ["calculator.h"],
)

cc_library(
    name = "point",
    hdrs = ["point.h"],
)

cc_library(
    name = "rect",
    hdrs = ["rect.h"],
    deps = [":point"],
)

cc_library(
    name = "calculator_js",
    srcs = ["calculator_js.cc"],
    hdrs = ["calculator_js.h"],
    copts = node_binding_copts(),
    deps = [
        ":calculator",
        "//:node_binding",
    ],
)

cc_library(
    name = "point_js",
    srcs = ["point_js.cc"],
    hdrs = ["point_js.h"],
    copts = node_binding_copts(),
    deps = [
        ":point",
        "//:node_binding"
    ]
)

cc_library(
    name = "rect_js",
    srcs = ["rect_js.cc"],
    hdrs = ["rect_js.h"],
    copts = node_binding_copts(),
    deps = [
        ":rect",
        ":point_js",
        "//:node_binding"
    ]
)

node_binding(
    name = "binding",
    srcs = [
        "binding.cc",
    ],
    copts = node_binding_copts(),
    deps = [
        ":calculator_js",
        ":point_js",
        ":rect_js",
    ],
)