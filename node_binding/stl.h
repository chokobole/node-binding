// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_STL_H_
#define NODE_BINDING_STL_H_

#include <vector>

#include "node_binding/type_convertor.h"

namespace node_binding {

template <typename T>
class TypeConvertor<std::vector<T>> {
 public:
  static std::vector<T> ToNativeValue(const Napi::Value& value) {
    std::vector<T> ret;
    Napi::Array arr = value.As<Napi::Array>();
    ret.reserve(arr.Length());
    for (size_t i = 0; i < arr.Length(); ++i) {
      ret.push_back(TypeConvertor<T>::ToNativeValue(arr[i]));
    }
    return ret;
  }

  static bool IsConvertible(const Napi::Value& value) {
    if (!value.IsArray()) return false;
    Napi::Array arr = value.As<Napi::Array>();
    for (size_t i = 0; i < arr.Length(); ++i) {
      if (!TypeConvertor<T>::IsConvertible(arr[i])) return false;
    }
    return true;
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const std::vector<T>& value) {
    Napi::Array ret = Napi::Array::New(env, value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      ret[i] = TypeConvertor<T>::ToJSValue(env, value[i]);
    }
    return ret;
  }
};

}  // namespace node_binding

#endif  // NODE_BINDING_STL_H_