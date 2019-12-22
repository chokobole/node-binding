# Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

def failed_to_find_bin_path(repository_ctx, bin, env_var = None):
    fmt_str = "Cannot find %s in PATH, please make sure %s is installed and add its directory in PATH"
    if env_var != None:
        return fmt_str + ", or --define %s='/path/to/%s'.\nPATH=%s" % (bin, bin, bin, env_var, repository_ctx.os.environ["PATH"])
    else:
        return fmt_str % (bin, bin)

def failed_to_find_bash_bin_path(repository_ctx):
    return failed_to_find_bin_path(repository_ctx, "bash", "BAZEL_SH")

def get_bin_path(repository_ctx, bin, env_var = None):
    bin_path = None
    if env_var != None:
        bin_path = repository_ctx.os.environ.get(env_var)
    if bin_path == None:
        bin_path = repository_ctx.which(bin)
    return bin_path

def get_bash_bin_path(repository_ctx):
    """Gets the bash bin path."""
    return get_bin_path(repository_ctx, "bash", "BAZEL_SH")
