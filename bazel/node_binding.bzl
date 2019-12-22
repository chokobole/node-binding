# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

def node_binding(
        name,
        srcs,
        **kwargs):
    native.cc_binary(
        name = name + ".so",
        srcs = srcs,
        linkshared = 1,
        **kwargs
    )

    native.genrule(
        name = name + "_node",
        srcs = [":" + name + ".so"],
        outs = [name + ".node"],
        cmd = "cp -f $< $@",
        visibility = ["//visibility:private"],
    )
