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

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsBoolean();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, bool value) {
    return Napi::Boolean::New(info.Env(), value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_integral<T>::value &&
                                        std::is_signed<T>::value &&
                                        sizeof(T) <= sizeof(int32_t)>> {
 public:
  static T ToNativeValue(const Napi::Value& value) {
    return static_cast<T>(value.As<Napi::Number>().Int32Value());
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsNumber();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, T value) {
    return Napi::Number::New(info.Env(), value);
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

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsNumber();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, T value) {
    return Napi::Number::New(info.Env(), value);
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

  static bool IsConvertible(const Napi::Value& value) {
#ifdef NAPI_EXPERIMENTAL
    return value.IsBigInt();
#else
    return value.IsNumber();
#endif
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, int64_t value) {
#ifdef NAPI_EXPERIMENTAL
    return Napi::BigInt::New(info.Env(), value);
#else
    return Napi::Number::New(info.Env(), value);
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

  static bool IsConvertible(const Napi::Value& value) {
#ifdef NAPI_EXPERIMENTAL
    return value.IsBigInt();
#else
    return value.IsNumber();
#endif
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, uint64_t value) {
#ifdef NAPI_EXPERIMENTAL
    return Napi::BigInt::New(info.Env(), value);
#else
    return Napi::Number::New(info.Env(), value);
#endif
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<float, T>::value>> {
 public:
  static float ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().FloatValue();
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsNumber();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, float value) {
    return Napi::Number::New(info.Env(), value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<double, T>::value>> {
 public:
  static double ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::Number>().DoubleValue();
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsNumber();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, double value) {
    return Napi::Number::New(info.Env(), value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_same<std::string, T>::value>> {
 public:
  static std::string ToNativeValue(const Napi::Value& value) {
    return value.As<Napi::String>().Utf8Value();
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsString();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info,
                               const std::string& value) {
    return Napi::String::New(info.Env(), value);
  }
};

template <typename T>
class TypeConvertor<T, std::enable_if_t<std::is_enum<T>::value>> {
 public:
  static std::underlying_type_t<T> ToNativeValue(const Napi::Value& value) {
    return TypeConvertor<std::underlying_type_t<T>>::ToNativeValue();
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsNumber();
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info, T value) {
    return Napi::Number::New(info.Env(),
                             static_cast<std::underlying_type_t<T>>(value));
  }
};

template <typename T>
auto ToNativeValue(const Napi::Value& value) {
  return TypeConvertor<T>::ToNativeValue(value);
}

template <typename T>
Napi::Value ToJSValue(const Napi::CallbackInfo& info, T&& value) {
  return TypeConvertor<std::decay_t<T>>::ToJSValue(info,
                                                   std::forward<T>(value));
}

}  // namespace node_binding

#endif  // NODE_BINDING_TYPE_CONVERTOR_H_