#!/usr/bin/env bash

set -e

BAZEL_VERSION=2.0.0
OS=`uname -s | tr '[:upper:]' '[:lower:]'`
ARCH=`uname -m`
FILE="bazel-${BAZEL_VERSION}-installer-${OS}-${ARCH}.sh"

wget https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/${FILE}
chmod +x ${FILE}
./${FILE} --user