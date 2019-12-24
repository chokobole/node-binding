// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "point.h"

#include "node_binding/constructor.h"
#include "node_binding/typed_call.h"

class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  PointJs(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;

  Point point_;
};

Napi::FunctionReference PointJs::constructor_;

// static
void PointJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "Point", {});

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Point", func);
}

PointJs::PointJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PointJs>(info) {
  if (info.Length() == 0) {
    point_ = node_binding::TypedConstruct(
        info, &node_binding::Constructor<Point>::Call<int, int>, 0, 0);
  } else if (info.Length() == 1) {
    point_ = node_binding::TypedConstruct(
        info, &node_binding::Constructor<Point>::Call<int, int>, 0);
  } else if (info.Length() == 2) {
    point_ = node_binding::TypedConstruct(
        info, &node_binding::Constructor<Point>::Call<int, int>);
  } else {
    Napi::Env env = info.Env();
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  PointJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(2_constructor, Init)