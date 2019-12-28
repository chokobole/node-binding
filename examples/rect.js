// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const binding = require('../bazel-bin/examples/binding.node');

function printPoint(p) {
  console.log(`(${p.x}, ${p.y})`);
}

const topLeft = new binding.Point(1, 5);
const bottomRight = new binding.Point(5, 1);
const rect = new binding.Rect(topLeft, bottomRight);
printPoint(rect.topLeft);
printPoint(rect.bottomRight);
console.log(rect.area());