// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "adder.h"
#include "node_binding/typed_call.h"

class AdderJs : public Napi::ObjectWrap<AdderJs> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  using Napi::ObjectWrap<AdderJs>::ObjectWrap;

  static Napi::Value Add(const Napi::CallbackInfo& info) {
    if (info.Length() == 0) {
      return node_binding::TypedCall(info, &Adder::Add, 1, 2);
    } else if (info.Length() == 1) {
      return node_binding::TypedCall(info, &Adder::Add, 2);
    } else {
      return node_binding::TypedCall(info, &Adder::Add);
    }
  }
};

// static
void AdderJs::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "Adder",
                                    {
                                        StaticMethod("add", &AdderJs::Add),
                                    });

  exports.Set("Adder", func);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  AdderJs::Init(env, exports);

  return exports;
}

NODE_API_MODULE(5_static_method, Init)