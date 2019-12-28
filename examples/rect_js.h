// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "examples/point_js.h"
#include "examples/rect.h"
#include "napi.h"

class RectJs : public Napi::ObjectWrap<RectJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  RectJs(const Napi::CallbackInfo& info);

  void SetTopLeft(const Napi::CallbackInfo& info, const Napi::Value& v);
  void SetBottomRight(const Napi::CallbackInfo& info, const Napi::Value& v);

  Napi::Value GetTopLeft(const Napi::CallbackInfo& info);
  Napi::Value GetBottomRight(const Napi::CallbackInfo& info);

  Napi::Value Area(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;

  Rect rect_;
};
