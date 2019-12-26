// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "node_binding/constructor.h"
#include "node_binding/typed_call.h"
#include "rect.h"

class RectJs : public Napi::ObjectWrap<RectJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  RectJs(const Napi::CallbackInfo& info);

  Napi::Value Size(const Napi::CallbackInfo& info) {
    return node_binding::TypedCall(info, &Rect::size, rect_.get());
  }

 private:
  static Napi::FunctionReference constructor_;

  std::unique_ptr<Rect> rect_;
};

Napi::FunctionReference RectJs::constructor_;

// static
void RectJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "Rect",
                                    {
                                        InstanceMethod("size", &RectJs::Size),
                                    });

  constructor_ = Napi::Persistent(func);
  constructor_.SuppressDestruct();

  exports.Set("Rect", func);
}

RectJs::RectJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<RectJs>(info) {
  rect_ = std::unique_ptr<Rect>(node_binding::TypedConstruct(
      info, &node_binding::Constructor<Rect>::CallNew<int, int>));
  if (info.Env().IsExceptionPending()) rect_.reset();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  RectJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(4_instance_method, Init)