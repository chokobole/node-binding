// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const calculator = require("../bazel-bin/examples/calculator.node");

console.log(`1 + 2 = ${calculator.Calculator.add(1, 2)}`);
console.log(`1 - 2 = ${calculator.Calculator.sub(1, 2)}`);
const c = new calculator.Calculator();
console.log(`${c.result()} + 1 = ${c.increment(), c.result()}`);
console.log(`${c.result()} + 3 = ${c.increment(3), c.result()}`);
console.log(`${c.result()} - 1 = ${c.decrement(), c.result()}`);
console.log(`${c.result()} - 5 = ${c.decrement(5), c.result()}`);
