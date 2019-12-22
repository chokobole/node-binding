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
  void AddInplace(int a) { result_ += a; }
  void SubInplace(int a) { result_ -= a; }
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

  void AddInplace(const Napi::CallbackInfo& info) {
    TypedCall(info, &Calculator::AddInplace, &calculator_);
  }

  void SubInplace(const Napi::CallbackInfo& info) {
    TypedCall(info, &Calculator::SubInplace, &calculator_);
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
                      InstanceMethod("addInplace", &CalculatorJs::AddInplace),
                      InstanceMethod("subInplace", &CalculatorJs::SubInplace),
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