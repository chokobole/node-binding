#!/usr/bin/env bash

set -e

node-gyp rebuild -C test/0_function
node-gyp rebuild -C test/1_default_argument
node-gyp rebuild -C test/2_constructor
node-gyp rebuild -C test/3_instance_accessor