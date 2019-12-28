// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>

#include "examples/calculator.h"
#include "napi.h"

class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  CalculatorJs(const Napi::CallbackInfo& info);

  static Napi::Value Add(const Napi::CallbackInfo& info);
  static Napi::Value Sub(const Napi::CallbackInfo& info);

  Napi::Value result(const Napi::CallbackInfo& info);
  void Increment(const Napi::CallbackInfo& info);
  void Decrement(const Napi::CallbackInfo& info);
  void Clear(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;

  std::unique_ptr<Calculator> calculator_;
};
