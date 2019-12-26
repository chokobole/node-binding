// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const assert = require('assert');
const test0 = require('./0_function/build/Release/0_function.node');
const test1 =
    require('./1_default_argument/build/Release/1_default_argument.node');
const test2 = require('./2_constructor/build/Release/2_constructor.node');
const test3 =
    require('./3_instance_accessor/build/Release/3_instance_accessor.node');
const test4 =
    require('./4_instance_method/build/Release/4_instance_method.node');
const test5 = require('./5_static_method/build/Release/5_static_method.node');
const test6 = require('./6_stl/build/Release/6_stl.node');

describe('0_function', () => {
  it('add(int arg0, int arg1) bind', () => {
    assert.throws(() => {
      test0.add(1);
    });
    assert.equal(test0.add(1, 2), 3);
    assert.throws(() => {
      test0.add(1, 2, 3);
    });
  });
});

describe('1_default_argument', () => {
  it('add(int arg0 = 1, int arg1 = 2) bind', () => {
    assert.equal(test1.add(), 3);
    assert.equal(test1.add(1), 3);
    assert.equal(test1.add(1, 2), 3);
    assert.throws(() => {
      test1.add(1, 2, 3);
    }, 'call add() more than 2 arguments is invalid.');
  });
});

describe('2_constructor', () => {
  it('Point(int x = 0, int y = 0) bind', () => {
    assert.doesNotThrow(() => new test2.Point());
    assert.doesNotThrow(() => new test2.Point(1));
    assert.doesNotThrow(() => new test2.Point(1, 2));
    assert.throws(() => {
      new test2.Point(1, 2, 3);
    });
  });
});

describe('3_instance_accessor', () => {
  it('Point.x and Point.y bind', () => {
    const p = new test3.Point();
    assert.equal(p.x, 0);
    assert.equal(p.y, 0);
    p.x = 1;
    assert.equal(p.x, 1);
    const p2 = new test3.Point(1, 2);
    assert.equal(p2.x, 1);
    assert.equal(p2.y, 2);
  });
});

describe('4_instance_method', () => {
  it('Rect::size() bind', () => {
    const r = new test4.Rect(5, 2);
    assert.equal(r.size(), 10);
  });
});

describe('5_static_method', () => {
  it('Adder::add(int a, int b) bind', () => {
    assert.equal(test5.Adder.add(), 3);
    assert.equal(test5.Adder.add(1), 3);
    assert.equal(test5.Adder.add(1, 2), 3);
  });
});

describe('6_stl', () => {
  it('std::vector<int> bind', () => {
    assert.equal(test6.sum([1, 2, 3]), 6);
    assert.deepEqual(test6.linSpace(1, 5, 1), [1, 2, 3, 4]);
  });
});
