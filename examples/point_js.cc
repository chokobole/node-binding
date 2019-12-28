// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/point_js.h"

#include "node_binding/constructor.h"
#include "node_binding/type_convertor.h"

using namespace node_binding;

Napi::FunctionReference PointJs::constructor_;

// static
void PointJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env, "Point",
                  {
                      InstanceAccessor("x", &PointJs::GetX, &PointJs::SetX),
                      InstanceAccessor("y", &PointJs::GetY, &PointJs::SetY),
                  });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Point", func);
}

// static
Napi::Object PointJs::New(Napi::Env env, const Point& p) {
  Napi::EscapableHandleScope scope(env);

  Napi::Object object = constructor_.New({
      Napi::Number::New(env, p.x),
      Napi::Number::New(env, p.y),
  });

  return scope.Escape(napi_value(object)).ToObject();
}

PointJs::PointJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PointJs>(info) {
  if (info.Length() == 0) {
    point_ = TypedConstruct(info, &Constructor<Point>::Call<int, int>, 0, 0);
  } else if (info.Length() == 1) {
    point_ = TypedConstruct(info, &Constructor<Point>::Call<int, int>, 0);
  } else if (info.Length() == 2) {
    point_ = TypedConstruct(info, &Constructor<Point>::Call<int, int>);
  } else {
    Napi::Env env = info.Env();
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
  }
}

void PointJs::SetX(const Napi::CallbackInfo& info, const Napi::Value& v) {
  point_.x = ToNativeValue<int>(v);
}

void PointJs::SetY(const Napi::CallbackInfo& info, const Napi::Value& v) {
  point_.y = ToNativeValue<int>(v);
}

Napi::Value PointJs::GetX(const Napi::CallbackInfo& info) {
  return ToJSValue(info, point_.x);
}

Napi::Value PointJs::GetY(const Napi::CallbackInfo& info) {
  return ToJSValue(info, point_.y);
}
