// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const binding = require('../bazel-bin/examples/binding.node');

console.log(`1 + 2 = ${binding.Calculator.add(1, 2)}`);
console.log(`1 - 2 = ${binding.Calculator.sub(1, 2)}`);
const c = new binding.Calculator();
console.log(`${c.result()} + 1 = ${c.increment(), c.result()}`);
console.log(`${c.result()} + 3 = ${c.increment(3), c.result()}`);
console.log(`${c.result()} - 1 = ${c.decrement(), c.result()}`);
console.log(`${c.result()} - 5 = ${c.decrement(5), c.result()}`);
const d = new binding.Calculator(5);
console.log(`${d.result()} + 1 = ${d.increment(), d.result()}`);
