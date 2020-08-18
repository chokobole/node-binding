// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <iostream>

#include "node_binding/type_convertor.h"
#include "point.h"

class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, const Point& p);
  PointJs(const Napi::CallbackInfo& info);

  void SetX(const Napi::CallbackInfo& info, const Napi::Value& v);
  void SetY(const Napi::CallbackInfo& info, const Napi::Value& v);

  Napi::Value GetX(const Napi::CallbackInfo& info);
  Napi::Value GetY(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor_;

  Point point_;
};

namespace node_binding {

template <>
class TypeConvertor<Point> {
 public:
  static Point ToNativeValue(const Napi::Value& value) {
    Napi::Object obj = value.As<Napi::Object>();

    return {
        TypeConvertor<int>::ToNativeValue(obj["x"]),
        TypeConvertor<int>::ToNativeValue(obj["y"]),
    };
  }

  static bool IsConvertible(const Napi::Value& value) {
    if (!value.IsObject()) return false;

    Napi::Object obj = value.As<Napi::Object>();

    return TypeConvertor<int>::IsConvertible(obj["x"]) &&
           TypeConvertor<int>::IsConvertible(obj["y"]);
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const Point& value) {
    return PointJs::New(env, value);
  }
};

}  // namespace node_binding