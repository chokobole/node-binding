// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_STL_H_
#define NODE_BINDING_STL_H_

#include <functional>
#include <vector>

#include "node_binding/type_convertor.h"

namespace node_binding {

template <std::size_t... I, class _Tuple>
Napi::Value Invoke(Napi::Function func, std::index_sequence<I...>,
                   _Tuple&& tup) {
  return func.Call({{ToJSValue<decltype(std::get<I>(tup))>(
      func.Env(), std::get<I>(tup))...}});
}

template <typename R, typename... Args>
class TypeConvertor<std::function<R(Args...)>> {
 public:
  static std::function<R(Args...)> ToNativeValue(const Napi::Value& value) {
    Napi::Function f = value.As<Napi::Function>();
    return [f](Args... args) -> R {
      return TypeConvertor<R>::ToNativeValue(
          Invoke(f, std::make_index_sequence<sizeof...(Args)>{},
                 std::forward_as_tuple(args...)));
    };
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const std::function<R(Args...)>& value) {
    return Napi::Function::New(env, value);
  }
};

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