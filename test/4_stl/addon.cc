// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "node_binding/stl.h"
#include "node_binding/typed_call.h"

int CSum(const std::vector<int>& vec) {
  int ret = 0;
  for (int v : vec) {
    ret += v;
  }
  return ret;
}

std::vector<int> CLinSpace(int from, int to, int step) {
  std::vector<int> ret;
  for (int i = from; i < to; i += step) {
    ret.push_back(i);
  }
  return ret;
}

Napi::Value Sum(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CSum);
}

Napi::Value LinSpace(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CLinSpace);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("sum", Napi::Function::New(env, Sum));
  exports.Set("linSpace", Napi::Function::New(env, LinSpace));
  return exports;
}

NODE_API_MODULE(4_stl, Init)