// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_OBJECT_H_
#define NODE_BINDING_OBJECT_H_

#include "node_binding/stl.h"

#if CXX_VER >= 201703
#include <any>
#include <unordered_map>
#endif

namespace node_binding {
#if defined(_MSC_VER)
#if CXX_VER >= 201703
#define _NODE_BINDING_OBJECT
#endif
#else
#if CXX_VER >= 201703 && defined(CXX_RTTI)
#define _NODE_BINDING_OBJECT
#endif
#endif

#ifdef _NODE_BINDING_OBJECT
using object = std::unordered_map<std::string, std::any>;
template <typename T>
T any_cast(std::any& _Any) {
#if defined(CXX_EXCEPTIONS)
  try {
    Napi::Value value = std::any_cast<Napi::Value>(_Any);
    if (TypeConvertor<T>::IsConvertible(value))
      return TypeConvertor<T>::ToNativeValue(value);
  } catch (...) {
    try {
      T value = std::any_cast<T>(_Any);
      return value;
    } catch (...) {
    }
  }
#elif defined(CXX_RTTI) || defined(_MSC_VER)
  if (typeid(Napi::Value) == _Any.type()) {
    Napi::Value value = std::any_cast<Napi::Value>(_Any);
    if (TypeConvertor<T>::IsConvertible(value))
      return TypeConvertor<T>::ToNativeValue(value);
  } else if (typeid(T) == _Any.type()) {
    return std::any_cast<T>(_Any);
  }
#endif
  return T();
}

/**
 * @brief std::unordered_map<std::string, std::any> <-> Napi::Object
 *
 */
template <>
class TypeConvertor<object> {
 public:
#define INSERT_NATIVE_ARRAY(_object_, _type_, _name_, _value_)           \
  if (TypeConvertor<std::vector<_type_>>::IsConvertible((_value_))) {    \
    (_object_).insert(                                                   \
        {(_name_).c_str(),                                               \
         TypeConvertor<std::vector<_type_>>::ToNativeValue((_value_))}); \
    continue;                                                            \
  }
#define INSERT_NATIVE_VALUE(_object_, _type_, _name_, _value_)                \
  if (TypeConvertor<_type_>::IsConvertible((value))) {                        \
    (_object_).insert(                                                        \
        {(_name_).c_str(), TypeConvertor<_type_>::ToNativeValue((_value_))}); \
    continue;                                                                 \
  }
  static object ToNativeValue(const Napi::Value& value) {
    object ret;
    const Napi::Object object = value.As<Napi::Object>();
    for (std::string& name :
         TypeConvertor<std::vector<std::string>>::ToNativeValue(
             object.GetPropertyNames())) {
      const Napi::Value& value = object.Get(name);
      if (value.IsArray()) {
        INSERT_NATIVE_ARRAY(ret, std::string, name, value);
        INSERT_NATIVE_ARRAY(ret, bool, name, value);
        INSERT_NATIVE_ARRAY(ret, short, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned short, name, value);
        INSERT_NATIVE_ARRAY(ret, int, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned int, name, value);
        INSERT_NATIVE_ARRAY(ret, long, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned long, name, value);
        INSERT_NATIVE_ARRAY(ret, int32_t, name, value);
        INSERT_NATIVE_ARRAY(ret, uint32_t, name, value);
        INSERT_NATIVE_ARRAY(ret, int64_t, name, value);
        INSERT_NATIVE_ARRAY(ret, uint64_t, name, value);
        INSERT_NATIVE_ARRAY(ret, float, name, value);
        INSERT_NATIVE_ARRAY(ret, double, name, value);
        INSERT_NATIVE_ARRAY(ret, node_binding::function, name, value);
        INSERT_NATIVE_ARRAY(ret, node_binding::async_function, name, value);
        INSERT_NATIVE_ARRAY(ret, node_binding::cancellable_async_function, name,
                            value);
        INSERT_NATIVE_ARRAY(ret, node_binding::object, name, value);
      } else if (value.IsFunction()) {
        INSERT_NATIVE_VALUE(ret, node_binding::function, name, value);
        continue;
      } else if (value.IsObject()) {
        INSERT_NATIVE_VALUE(ret, node_binding::object, name, value);
      } else if (value.IsUndefined()) {
        ret.insert({name.c_str(), std::any()});
        continue;
      } else if (value.IsNull()) {
        ret.insert({name.c_str(), nullptr});
        continue;
      } else {
        INSERT_NATIVE_VALUE(ret, std::string, name, value);
        INSERT_NATIVE_VALUE(ret, bool, name, value);
        INSERT_NATIVE_VALUE(ret, short, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned short, name, value);
        INSERT_NATIVE_VALUE(ret, int, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned int, name, value);
        INSERT_NATIVE_VALUE(ret, long, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned long, name, value);
        INSERT_NATIVE_VALUE(ret, int32_t, name, value);
        INSERT_NATIVE_VALUE(ret, uint32_t, name, value);
        INSERT_NATIVE_VALUE(ret, int64_t, name, value);
        INSERT_NATIVE_VALUE(ret, uint64_t, name, value);
        INSERT_NATIVE_VALUE(ret, float, name, value);
        INSERT_NATIVE_VALUE(ret, double, name, value);
        INSERT_NATIVE_VALUE(ret, node_binding::function, name, value);
        INSERT_NATIVE_VALUE(ret, node_binding::async_function, name, value);
        INSERT_NATIVE_VALUE(ret, node_binding::cancellable_async_function, name,
                            value);
        INSERT_NATIVE_VALUE(ret, node_binding::object, name, value);
      }
      ret.insert({name.c_str(), std::any()});
    }
    return ret;
  }

  static bool IsConvertible(const Napi::Value& value) {
    return ((!value.IsFunction()) && value.IsObject());
  }

#define INSERT_JS_VALUE(_env_, _object_, _type_, _member_)                   \
  if (typeid(_type_) == (_member_).second.type()) {                          \
    (_object_)[(_member_).first.c_str()] = TypeConvertor<_type_>::ToJSValue( \
        (_env_), std::any_cast<_type_>((_member_).second));                  \
    continue;                                                                \
  }
#define INSERT_JS_STR_VALUE(_env_, _object_, _type_, _member_)  \
  if (typeid(_type_) == (_member_).second.type()) {             \
    (_object_)[(_member_).first.c_str()] =                      \
        TypeConvertor<std::string>::ToJSValue(                  \
            (_env_), std::any_cast<_type_>((_member_).second)); \
    continue;                                                   \
  }
#define INSERT_JS_ARRAY(_env_, _object_, _type_, _member_)                   \
  if (typeid(std::vector<_type_>) == (_member_).second.type()) {             \
    (_object_)[(_member_).first.c_str()] =                                   \
        TypeConvertor<std::vector<_type_>>::ToJSValue(                       \
            (_env_), std::any_cast<std::vector<_type_>>((_member_).second)); \
    continue;                                                                \
  }
  static Napi::Value ToJSValue(const Napi::Env& env, const object& value) {
    Napi::Object ret = Napi::Object::New(env);

    for (auto member : value) {
      INSERT_JS_STR_VALUE(env, ret, char*, member);
      INSERT_JS_STR_VALUE(env, ret, const char*, member);
      INSERT_JS_VALUE(env, ret, std::string, member);
      INSERT_JS_VALUE(env, ret, bool, member);
      INSERT_JS_VALUE(env, ret, short, member);
      INSERT_JS_VALUE(env, ret, unsigned short, member);
      INSERT_JS_VALUE(env, ret, int, member);
      INSERT_JS_VALUE(env, ret, unsigned int, member);
      INSERT_JS_VALUE(env, ret, long, member);
      INSERT_JS_VALUE(env, ret, unsigned long, member);
      INSERT_JS_VALUE(env, ret, int32_t, member);
      INSERT_JS_VALUE(env, ret, uint32_t, member);
      INSERT_JS_VALUE(env, ret, int64_t, member);
      INSERT_JS_VALUE(env, ret, uint64_t, member);
      INSERT_JS_VALUE(env, ret, float, member);
      INSERT_JS_VALUE(env, ret, double, member);
      INSERT_JS_VALUE(env, ret, node_binding::cancellable_async_function,
                      member);
      INSERT_JS_VALUE(env, ret, node_binding::async_function, member);
      INSERT_JS_VALUE(env, ret, node_binding::function, member);
      INSERT_JS_VALUE(env, ret, node_binding::object, member);

      INSERT_JS_ARRAY(env, ret, std::string, member);
      INSERT_JS_ARRAY(env, ret, bool, member);
      INSERT_JS_ARRAY(env, ret, short, member);
      INSERT_JS_ARRAY(env, ret, unsigned short, member);
      INSERT_JS_ARRAY(env, ret, int, member);
      INSERT_JS_ARRAY(env, ret, unsigned int, member);
      INSERT_JS_ARRAY(env, ret, long, member);
      INSERT_JS_ARRAY(env, ret, unsigned long, member);
      INSERT_JS_ARRAY(env, ret, int32_t, member);
      INSERT_JS_ARRAY(env, ret, uint32_t, member);
      INSERT_JS_ARRAY(env, ret, int64_t, member);
      INSERT_JS_ARRAY(env, ret, uint64_t, member);
      INSERT_JS_ARRAY(env, ret, float, member);
      INSERT_JS_ARRAY(env, ret, double, member);
      INSERT_JS_ARRAY(env, ret, node_binding::cancellable_async_function,
                      member);
      INSERT_JS_ARRAY(env, ret, node_binding::async_function, member);
      INSERT_JS_ARRAY(env, ret, node_binding::function, member);
      INSERT_JS_ARRAY(env, ret, node_binding::object, member);

      ret[member.first.c_str()] = env.Undefined();
    }
    return ret;
  }
};
#endif  // _NODE_BINDING_OBJECT
}  // namespace node_binding

#endif  // NODE_BINDING_OBJECT_H_