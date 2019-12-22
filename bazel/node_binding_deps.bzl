# Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//third_party/py:python_configure.bzl", "python_configure")

def node_binding_deps():
    python_configure(name = "local_config_python")
