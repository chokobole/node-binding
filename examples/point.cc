// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/point.h"

#include "node_binding/constructor.h"
#include "node_binding/typed_call.h"

namespace node_binding {

class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  PointJs(const Napi::CallbackInfo& info);

  void SetX(const Napi::CallbackInfo& info, const Napi::Value& v) {
    point_.x = ToNativeValue<int>(v);
  }

  void SetY(const Napi::CallbackInfo& info, const Napi::Value& v) {
    point_.y = ToNativeValue<int>(v);
  }

  Napi::Value GetX(const Napi::CallbackInfo& info) {
    return ToJSValue(info, point_.x);
  }

  Napi::Value GetY(const Napi::CallbackInfo& info) {
    return ToJSValue(info, point_.y);
  }

 private:
  static Napi::FunctionReference constructor_;

  Point point_;
};

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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  PointJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(point, Init)

}  // namespace node_binding