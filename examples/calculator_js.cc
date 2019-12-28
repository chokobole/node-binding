// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/calculator_js.h"

#include "node_binding/constructor.h"
#include "node_binding/typed_call.h"

using namespace node_binding;

Napi::FunctionReference CalculatorJs::constructor_;

// static
void CalculatorJs::Init(Napi::Env env, Napi::Object exports) {
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
    : Napi::ObjectWrap<CalculatorJs>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) {
    calculator_ = std::unique_ptr<Calculator>(
        TypedConstruct(info, &Constructor<Calculator>::CallNew<>));
  } else if (info.Length() == 1) {
    calculator_ = std::unique_ptr<Calculator>(
        TypedConstruct(info, &Constructor<Calculator>::CallNew<int>));
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
  }

  if (env.IsExceptionPending()) calculator_.reset();
}

// static
Napi::Value CalculatorJs::Add(const Napi::CallbackInfo& info) {
  return TypedCall(info, &Calculator::Add);
}

// static
Napi::Value CalculatorJs::Sub(const Napi::CallbackInfo& info) {
  return TypedCall(info, &Calculator::Sub);
}

Napi::Value CalculatorJs::result(const Napi::CallbackInfo& info) {
  return TypedCall(info, &Calculator::result, calculator_.get());
}

void CalculatorJs::Increment(const Napi::CallbackInfo& info) {
  if (info.Length() == 0) {
    TypedCall(info, &Calculator::Increment, calculator_.get(), 1);
  } else {
    TypedCall(info, &Calculator::Increment, calculator_.get());
  }
}

void CalculatorJs::Decrement(const Napi::CallbackInfo& info) {
  if (info.Length() == 0) {
    TypedCall(info, &Calculator::Decrement, calculator_.get(), 1);
  } else {
    TypedCall(info, &Calculator::Decrement, calculator_.get());
  }
}

void CalculatorJs::Clear(const Napi::CallbackInfo& info) {
  TypedCall(info, &Calculator::Clear, calculator_.get());
}