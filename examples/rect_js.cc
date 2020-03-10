// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/rect_js.h"

#include "node_binding/constructor.h"
#include "node_binding/type_convertor.h"

using namespace node_binding;

Napi::FunctionReference RectJs::constructor_;

// static
void RectJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "Rect",
      {
          InstanceAccessor("topLeft", &RectJs::GetTopLeft, &RectJs::SetTopLeft),
          InstanceAccessor("bottomRight", &RectJs::GetBottomRight,
                           &RectJs::SetBottomRight),
          InstanceMethod("area", &RectJs::Area),
      });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Rect", func);
}

RectJs::RectJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<RectJs>(info) {
  if (info.Length() == 0) {
    rect_ = TypedConstruct(info, &Constructor<Rect>::Call<>);
  } else if (info.Length() == 2) {
    rect_ = TypedConstruct(
        info, &Constructor<Rect>::Call<const Point&, const Point&>);
  } else {
    Napi::Env env = info.Env();
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
  }
}

void RectJs::SetTopLeft(const Napi::CallbackInfo& info, const Napi::Value& v) {
  if (IsConvertible<Point>(v)) {
    rect_.top_left = ToNativeValue<Point>(v);
  }
}

void RectJs::SetBottomRight(const Napi::CallbackInfo& info,
                            const Napi::Value& v) {
  if (IsConvertible<Point>(v)) {
    rect_.bottom_right = ToNativeValue<Point>(v);
  }
}

Napi::Value RectJs::GetTopLeft(const Napi::CallbackInfo& info) {
  return ToJSValue(info.Env(), rect_.top_left);
}

Napi::Value RectJs::GetBottomRight(const Napi::CallbackInfo& info) {
  return ToJSValue(info.Env(), rect_.bottom_right);
}

Napi::Value RectJs::Area(const Napi::CallbackInfo& info) {
  return TypedCall(info.Env(), &Rect::Area, &rect_);
}