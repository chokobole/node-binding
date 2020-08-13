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
  const callback_data = '안녕';
  const count = 20;
  const stopIndex = 1;
  const delay = 100;
  const timeout = 5000;
  const promise_test_repeat_count = 10000;
  const promise_test_cancel_delay = timeout - 2000;

  it('std::vector<int> bind', () => {
    assert.equal(test6.sum([1, 2, 3]), 6);
    assert.deepEqual(test6.linSpace(1, 5, 1), [1, 2, 3, 4]);
  });

  function test_obj(obj, name) {
    assert.equal(obj.name, name);
    assert.equal(obj[name + '_bool_array'].length, 4);
    assert.equal(obj[name + '_bool_array'][0], true);
    assert.equal(obj[name + '_bool_array'][1], false);
    assert.equal(obj[name + '_bool_array'][2], true);
    assert.equal(obj[name + '_bool_array'][3], false);

    assert.equal(obj[name + '_int_array'].length, 4);
    assert.equal(obj[name + '_int_array'][0], 1);
    assert.equal(obj[name + '_int_array'][1], 2);
    assert.equal(obj[name + '_int_array'][2], 3);
    assert.equal(obj[name + '_int_array'][3], 4);

    assert.equal(obj[name + '_str_array'].length, 4);
    assert.equal(obj[name + '_str_array'][0], '1');
    assert.equal(obj[name + '_str_array'][1], '2');
    assert.equal(obj[name + '_str_array'][2], '3');
    assert.equal(obj[name + '_str_array'][3], '4');

    assert.equal(obj[name + '_double_array'].length, 4);
    assert.equal(obj[name + '_double_array'][0], 1.2);
    assert.equal(obj[name + '_double_array'][1], 3.4);
    assert.equal(obj[name + '_double_array'][2], 5.6);
    assert.equal(obj[name + '_double_array'][3], 7.8);

    assert.equal(obj[name + '_fn_array'].length, 2);
    assert.equal(obj[name + '_fn_array'][0]('hi'), 'called (fn[0]) : hi');
    assert.equal(obj[name + '_fn_array'][1]('hi'), 'called (fn[1]) : hi');

    let ret = obj.callback(1, callback_data);
    assert.equal(ret.arg0, 1);
    assert.equal(ret.arg1, callback_data);
  }

  describe('std::std::unordered_map<std::string, std::any> bind', () => {
    if (test6.getName) {
      it('Get property', () => {
        let obj = { 'name': 'test' };
        assert.equal(test6.getName(obj), obj.name);
      });
    }
    // Not yet supported
    // if (test6.setName) {
    //   it('Get property', () => {
    //     let obj = { 'name': 'test' };
    //     assert.ok(test6.setName(obj, 'obj1'));
    //     assert.ok(obj.name == 'obj1);
    //   });
    // }

    if (test6.getObject) {
      it('return object', () => {
        let obj = test6.getObject('test');
        test_obj(obj, 'test');
        assert.equal(obj.test_obj_array.length, 4);
        test_obj(obj.test_obj_array[0], 'test');
        test_obj(obj.test_obj_array[1], 'test');
        test_obj(obj.test_obj_array[2], 'test');
        test_obj(obj.test_obj_array[3], 'test');
      });
    }
    if (test6.invokeCallback) {
      it('return object & invoke callback', () => {
        let obj = test6.getObject('callback_test');
        assert.equal(obj.name, 'callback_test');
        test_obj(obj, 'callback_test');

        let ret = test6.invokeCallback(obj);
        let ret2 = obj.callback(1, obj.name);

        assert.equal(ret.arg0, 1);
        assert.equal(ret.arg1, obj.name);
        assert.equal(ret2.arg0, 1);
        assert.equal(ret2.arg1, obj.name);
        assert.equal(ret2.arg0, ret.arg0);
        assert.equal(ret2.arg1, ret.arg1);
      });
    }
  });

  describe('Napi::Object bind', () => {
    it('Get property', () => {
      let obj = { 'name': 'test' };
      assert.equal(test6.getNameWithNapi(obj), obj.name);
    });
    it('Set property', () => {
      let obj = { 'name': 'test' };
      assert.ok(test6.setNameWithNapi(obj, 'obj1'));
      assert.equal(obj.name, 'obj1');
    });
    it('return object', () => {
      let obj = test6.getObjectWithNapi('test');
      test_obj(obj, 'test');
      assert.equal(obj.test_obj_array.length, 4);
      test_obj(obj.test_obj_array[0], 'test');
      test_obj(obj.test_obj_array[1], 'test');
      test_obj(obj.test_obj_array[2], 'test');
      test_obj(obj.test_obj_array[3], 'test');
    });
    it('return object & invoke callback', () => {
      let obj = test6.getObjectWithNapi('callback_test');
      assert.equal(obj.name, 'callback_test');
      test_obj(obj, 'callback_test');

      let ret = test6.invokeCallbackWithNapi(obj);
      let ret2 = obj.callback(1, obj.name);

      assert.equal(ret.arg0, 1);
      assert.equal(ret.arg1, obj.name);
      assert.equal(ret2.arg0, 1);
      assert.equal(ret2.arg1, obj.name);
      assert.equal(ret2.arg0, ret.arg0);
      assert.equal(ret2.arg1, ret.arg1);
    });
  });

  if (test6.napiVersion() > 3) {
    // 오류가 발생하지 않으면 성공한것으로 판단하면 됩니다.
    describe('tsfn move test', () => {
      // promise가 오래 동작(5*100)하기때문에 native thread 보다 늦게 종료되는
      // 경우입니다. (일반적인 경우와 동일함)
      it('tsfn1', () => {
        test6
          .beginMoveTsfnCallbackTest(
            'tsfn1', 5, 100,
            (str, num) => {
              // console.log(str + " " + num);
              return true;
            })
          .then(
            (result) => {
              // console.log('tsfn1 - resolved : ' + result);
            })
          .finally(() => {
            test6.endMoveTsfnCallbackTest();
          });
      });

      // 시간을 짧게 설정하여 promise가 natiave thread보다 빠르게 종료되어도,
      // native thread에서 안전하게 콜백 함수가 호출되는 것을 검증합니다.
      it('tsfn2',
        () => {
          test6
            .beginMoveTsfnCallbackTest(
              'tsfn2', 0, 0,
              (str, num) => {
                // console.log(str + " " + num);
                return true;
              })
            .then(
              (result) => {
                // console.log('tsfn2 - resolved : ' + result);
              })
            .finally(() => {
              test6.endMoveTsfnCallbackTest();
            })
        });
    });
  }
  describe('std::function<?> bind', () => {
    it('std::function<void(std::string, int)>', () => {
      test6.callbackTest(callback_data, count, delay, (str, num) => {
        // throw 111 // <-- object나 function아닌 내용을 throw하게 되면,
        // node_binding/typed_call.h의 __USE_FUNCTION_CALL_METHOD__ 활성화 시
        // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
        assert.equal(str, callback_data + ' - callbackTest');
        assert.ok(num <= count);
        assert.ok(num >= 0);
      });
    }).timeout(timeout);

    it('std::function<bool(std::string, int)>', () => {
      test6.callbackTest2(callback_data, count, delay, (str, num) => {
        // throw 111 // <-- object나 function아닌 내용을 throw하게 되면,
        // node_binding/typed_call.h의 __USE_FUNCTION_CALL_METHOD__ 활성화 시
        // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
        assert.equal(str, callback_data + ' - callbackTest2');
        assert.ok(num <= count);
        assert.ok(num >= stopIndex);
        if (num > stopIndex) return true;
        return false;
      });
    }).timeout(timeout);
  });

  if (test6.napiVersion() > 3) {
    describe(
      'node_binding::ToPromise - node_binding::thread_safe_function<?> bind',
      () => {
        it('node_binding::thread_safe_function<void(std::string, int)>',
          (done) => {
            test6
              .promiseCallbackTest(
                callback_data, count, delay,
                (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을
                  // throw하게 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류
                  // 발생함.
                  try {
                    assert.equal(
                      str,
                      callback_data + ' - promiseCallbackTest');
                    assert.ok(num <= count);
                    assert.ok(num >= 0);
                  } catch (error) {
                    done(error);
                  }
                })
              .then((result) => {
                try {
                  assert.equal(result, undefined);
                } catch (error) {
                  done(error);
                  return;
                }
                done();
              })
          })
          .timeout(timeout);

        it('node_binding::thread_safe_function<bool(std::string, int)>',
          (done) => {
            test6
              .promiseCallbackTest2(
                callback_data, count, delay,
                (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을
                  // throw하게 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
                  try {
                    assert.equal(
                      str, callback_data + ' - promiseCallbackTest2');
                    assert.ok(num <= count);
                    assert.ok(num >= stopIndex);
                    if (num > stopIndex) return true;
                    done();
                  } catch (error) {
                    done(error);
                  }
                  return false;
                })
              .then((result) => {
                try {
                  assert.equal(
                    result,
                    'stopped at ' + callback_data +
                    ' - promiseCallbackTest2' + stopIndex);
                } catch (error) {
                  done(error);
                }
              });
          })
          .timeout(timeout);
      });

    describe(
      'node_binding::ToCancellablePromise - node_binding::thread_safe_function<?> bind',
      () => {
        it('node_binding::thread_safe_function<void(std::string, int)>',
          (done) => {
            let jobs = new Map();
            for (let index = 0; index < promise_test_repeat_count; ++index) {
              let ret = test6.cancellablePromiseCallbackTest(
                callback_data, count, delay, (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을 throw하게
                  // 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
                  try {
                    assert.equal(
                      str,
                      callback_data + ' - cancellablePromiseCallbackTest');
                    assert.ok(num <= count);
                    assert.ok(num >= 0);
                  } catch (error) {
                    done(error);
                  }
                });
              jobs.set(index, ret);
              ret.promise
                .then((result) => {
                  assert.equal(result, undefined);
                })
                .catch((reason) => {
                  try {
                    assert.equal(reason.status, 'canceled');
                  } catch (error) {
                    assert.fail(error);
                  }
                })
                .finally(() => {
                  jobs.delete(index);
                  if (jobs.size == 0) {
                    done();
                  }
                });
              setTimeout(() => ret.cancel(), promise_test_cancel_delay);
            }
          })
          .timeout(timeout);

        it('node_binding::thread_safe_function<bool(std::string, int)>',
          (done) => {
            let jobs = new Map();
            for (let index = 0; index < promise_test_repeat_count; ++index) {
              let ret = test6.cancellablePromiseCallbackTest2(
                callback_data, count, delay, (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을 throw하게
                  // 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
                  try {
                    assert.equal(
                      str,
                      callback_data +
                      ' - cancellablePromiseCallbackTest2');
                    assert.ok(num <= count);
                    assert.ok(num >= stopIndex);
                    if (num > stopIndex) return true;
                  } catch (error) {
                    done(error);
                  }
                  return false;
                });
              jobs.set(index, ret);
              ret.promise
                .then((result) => {
                  try {
                    assert.equal(
                      result,
                      'stopped at ' + callback_data +
                      ' - cancellablePromiseCallbackTest2' +
                      stopIndex);
                  } catch (error) {
                    done(error);
                  }
                })
                .catch((reason) => {
                  try {
                    assert.equal(reason.status, 'canceled');
                  } catch (error) {
                  }
                })
                .finally(() => {
                  jobs.delete(index);
                  if (jobs.size == 0) done();
                });
              setTimeout(() => ret.cancel(), promise_test_cancel_delay);
            }
          })
          .timeout(timeout)
      });

    describe(
      'node_binding::ToCancellablePromise - node_binding::cancel_context_ptr and node_binding::thread_safe_function<?> bind',
      () => {
        it('node_binding::cancel_context_ptr and node_binding::thread_safe_function<void(std::string, int)>',
          (done) => {
            let jobs = new Map();
            let t = 0;
            for (let index = 0; index < promise_test_repeat_count; ++index) {
              let ret = test6.cancellablePromiseCallbackTestWithCancelContext(
                callback_data, count, delay, (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을 throw하게
                  // 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
                  try {
                    assert.equal(
                      str,
                      callback_data +
                      ' - cancellablePromiseCallbackTestWithCancelContext');
                    assert.ok(num <= count);
                    assert.ok(num >= 0);
                  } catch (error) {
                    done(error);
                  }
                });
              jobs.set(index, ret);
              ret.promise
                .then((result) => {
                  assert.equal(result, undefined);
                })
                .catch((reason) => {
                  try {
                    assert.equal(reason.status, 'canceled');
                  } catch (error) {
                    assert.fail(error);
                  }
                })
                .finally(() => {
                  jobs.delete(index);
                  if (jobs.size == 0) done();
                });
              setTimeout(() => ret.cancel(), promise_test_cancel_delay);
            }
          })
          .timeout(timeout);

        it('node_binding::cancel_context_ptr and node_binding::thread_safe_function<bool(std::string, int)>',
          (done) => {
            let jobs = new Map();
            let stopped = false;
            for (let index = 0; index < promise_test_repeat_count; ++index) {
              let ret = test6.cancellablePromiseCallbackTestWithCancelContext2(
                callback_data, count, delay, (str, num) => {
                  // throw 111 // <-- object나 function아닌 내용을 throw하게
                  // 되면, node_binding/typed_call.h의
                  // __USE_FUNCTION_CALL_METHOD__ 활성화 시
                  // Napi::Error::Fatal메서드가 호출되어 오류 발생함.
                  try {
                    assert.equal(
                      str,
                      callback_data +
                      ' - cancellablePromiseCallbackTestWithCancelContext2');
                    assert.ok(num <= count);
                    assert.ok(num >= stopIndex);
                    if (num > stopIndex) return true;
                  } catch (error) {
                    done(error);
                  }
                  return false;
                });
              jobs.set(index, ret);
              ret.promise
                .then((result) => {
                  try {
                    assert.equal(
                      result,
                      'stopped at ' + callback_data +
                      ' - cancellablePromiseCallbackTestWithCancelContext2' +
                      stopIndex);
                  } catch (error) {
                    done(error);
                  }
                })
                .catch((reason) => {
                  try {
                    assert.equal(reason.status, 'canceled');
                  } catch (error) {
                  }
                })
                .finally(() => {
                  jobs.delete(index);
                  if (jobs.size == 0) done();
                });
              setTimeout(() => ret.cancel(), promise_test_cancel_delay);
            }
          })
          .timeout(timeout)
      });
  }
});