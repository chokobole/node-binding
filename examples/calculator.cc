// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include "node_binding/typed_call.h"

namespace node_binding {

class Calculator {
 public:
  static int Add(int a, int b) { return a + b; }
  static int Sub(int a, int b) { return a - b; }

  int result() const { return result_; }
  void Increment(int a = 1) { result_ += a; }
  void Decrement(int a = 1) { result_ -= a; }
  void Clear() { result_ = 0; }

 private:
  int result_ = 0;
};

class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  CalculatorJs(const Napi::CallbackInfo& info);

  static Napi::Value Add(const Napi::CallbackInfo& info) {
    return TypedCall(info, &Calculator::Add);
  }

  static Napi::Value Sub(const Napi::CallbackInfo& info) {
    return TypedCall(info, &Calculator::Sub);
  }

  Napi::Value result(const Napi::CallbackInfo& info) {
    return TypedCall(info, &Calculator::result, &calculator_);
  }

  void Increment(const Napi::CallbackInfo& info) {
    if (info.Length() == 0) {
      TypedCall(info, &Calculator::Increment, &calculator_, 1);
    } else {
      TypedCall(info, &Calculator::Increment, &calculator_);
    }
  }

  void Decrement(const Napi::CallbackInfo& info) {
    if (info.Length() == 0) {
      TypedCall(info, &Calculator::Decrement, &calculator_, 1);
    } else {
      TypedCall(info, &Calculator::Decrement, &calculator_);
    }
  }

  void Clear(const Napi::CallbackInfo& info) {
    TypedCall(info, &Calculator::Clear, &calculator_);
  }

 private:
  static Napi::FunctionReference constructor_;

  Calculator calculator_;
};

Napi::FunctionReference CalculatorJs::constructor_;

// static
void CalculatorJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func =
      DefineClass(env, "Calculator",
                  {
                      StaticMethod("add", &CalculatorJs::Add),
                      StaticMethod("sub", &CalculatorJs::Sub),
                      InstanceMethod("result", &CalculatorJs::result),
                      InstanceMethod("increment", &CalculatorJs::Increment),
                      InstanceMethod("decrement", &CalculatorJs::Decrement),
                      InstanceMethod("clear", &CalculatorJs::Clear),
                  });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Calculator", func);
}

CalculatorJs::CalculatorJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CalculatorJs>(info) {}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  CalculatorJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(calculator, Init)

}  // namespace node_binding