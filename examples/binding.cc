// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/calculator_js.h"
#include "examples/point_js.h"
#include "examples/rect_js.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  CalculatorJs::Init(env, exports);
  PointJs::Init(env, exports);
  RectJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(bindings, Init)
