// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_TYPE_CONVERTOR_H_
#define NODE_BINDING_TYPE_CONVERTOR_H_

#include <type_traits>

#include "napi.h"

namespace node_binding {

template <typename T, typename SFINAE = void>
class TypeConvertor;

template <>
class TypeConvertor<bool> {
 public:
  static bool ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Boolean>().Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, bool value) {
    return Napi::Boolean::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_integral<T>::value &&
                                        std::is_signed<T>::value &&
                                        sizeof(T) <= sizeof(int32_t)>> {
 public:
  static T ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().Int32Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<
    T, std::enable_if_t<
           std::is_integral<T>::value && !std::is_same<bool, T>::value &&
           !std::is_signed<T>::value && sizeof(T) <= sizeof(int32_t)>> {
 public:
  static T ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().Uint32Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<int64_t, T>::value>> {
 public:
  static int64_t ToNativeValue(const Napi::Value& value) {
#ifdef NAPI_EXPERIMENTAL
    return value.As<Napi::BigInt>().Int64Value();
#else
    return value.As<Napi::Number>().Int64Value();
#endif
  }

  static Napi::Value ToJSValue(Napi::Env env, int64_t value) {
#ifdef NAPI_EXPERIMENTAL
    return Napi::BigInt::New(env, value);
#else
    return Napi::Number::New(env, value);
#endif
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<uint64_t, T>::value>> {
 public:
  static uint64_t ToNativeValue(const Napi::Value& value) {
#ifdef NAPI_EXPERIMENTAL
    return value.As<Napi::BigInt>().Uint64Value();
#else
    return static_cast<uint64_t>(value.As<Napi::Number>().Int64Value());
#endif
  }

  static Napi::Value ToJSValue(Napi::Env env, uint64_t value) {
#ifdef NAPI_EXPERIMENTAL
    return Napi::BigInt::New(env, value);
#else
    return Napi::Number::New(env, value);
#endif
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<float, T>::value>> {
 public:
  static float ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().FloatValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, float value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<double, T>::value>> {
 public:
  static double ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().DoubleValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, double value) {
    return Napi::Number::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<std::string, T>::value>> {
 public:
  static std::string ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::String>().Utf8Value();
  }

  static Napi::Value ToJSValue(Napi::Env env, const std::string& value) {
    return Napi::String::New(env, value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_enum<T>::value>> {
 public:
  static std::underlying_type_t<T> ToNativeValue(const Napi::Value& value) {
    return TypeConvertor<std::underlying_type_t<T>>::ToNativeValue();
  }

  static Napi::Value ToJSValue(Napi::Env env, T value) {
    return Napi::Number::New(env,
                             static_cast<std::underlying_type_t<T>>(value));
  }
};

template <typename T>
auto ToNativeValue(const Napi::Value& value) {
  return TypeConvertor<T>::ToNativeValue(value);
}

template <typename T>
Napi::Value ToJSValue(Napi::Env env, T&& value) {
  return TypeConvertor<std::decay_t<T>>::ToJSValue(env, std::forward<T>(value));
}

}  // namespace node_binding

#endif  // NODE_BINDING_TYPE_CONVERTOR_H_