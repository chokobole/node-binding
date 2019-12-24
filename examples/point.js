// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const point = require('../bazel-bin/examples/point.node');

const p = new point.Point();
p.x = 1;
p.y = p.x + 2;
console.log(`(${p.x}, ${p.y})`);
const p2 = new point.Point(2, 4);
console.log(`(${p2.x}, ${p2.y})`);