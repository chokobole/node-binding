// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_TYPED_CALL_H_
#define NODE_BINDING_TYPED_CALL_H_

#include "napi.h"

#include "node_binding/template_util.h"
#include "node_binding/type_convertor.h"

namespace node_binding {

namespace internal {

template <typename T, std::enable_if_t<std::is_same<bool, T>::value>* = nullptr>
Napi::Boolean ToJs(Napi::Env env, T value) {
  return Napi::Boolean::New(env, value);
}

template <typename T,
          std::enable_if_t<std::is_arithmetic<T>::value &&
                           !std::is_same<bool, T>::value>* = nullptr>
Napi::Number ToJs(Napi::Env env, T value) {
  return Napi::Number::New(env, value);
}

template <typename T,
          std::enable_if_t<std::is_same<std::string, T>::value>* = nullptr>
Napi::String ToJs(Napi::Env env, const T& value) {
  return Napi::String::New(env, value);
}

#define ARG(N) \
  TypeConvertor<internal::PickTypeListItem<N, ArgList>>::ToNativeValue(info[N])

template <typename R, typename... Args,
          std::enable_if_t<0 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  return f();
}

template <typename R, typename... Args,
          std::enable_if_t<1 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0));
}

template <typename R, typename... Args,
          std::enable_if_t<2 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1));
}

template <typename R, typename... Args,
          std::enable_if_t<3 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2));
}

template <typename R, typename... Args,
          std::enable_if_t<4 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3));
}

template <typename R, typename... Args,
          std::enable_if_t<5 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
}

template <typename R, typename... Args,
          std::enable_if_t<6 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5));
}

template <typename R, typename... Args,
          std::enable_if_t<7 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6));
}

template <typename R, typename... Args,
          std::enable_if_t<8 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6), ARG(7));
}

template <typename R, typename... Args,
          std::enable_if_t<9 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  using ArgList = internal::TypeList<Args...>;
  return f(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6), ARG(7),
           ARG(8));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<0 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  return ((*c).*f)();
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<1 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<2 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<3 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<4 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<5 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<6 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<7 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<8 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<9 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7), ARG(8));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<0 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  return ((*c).*f)();
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<1 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<2 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<3 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<4 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<5 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<6 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<7 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<8 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<9 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7), ARG(8));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<0 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  return ((*c).*f)();
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<1 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<2 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<3 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<4 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<5 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<6 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<7 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<8 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<9 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5), ARG(6),
                   ARG(7), ARG(8));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<0 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  return (std::move(*c).*f)();
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<1 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<2 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<3 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<4 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<5 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<6 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<7 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5),
                            ARG(6));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<8 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5),
                            ARG(6), ARG(7));
}

template <typename R, typename Class, typename... Args,
          std::enable_if_t<9 == sizeof...(Args)>* = nullptr>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4), ARG(5),
                            ARG(6), ARG(7), ARG(8));
}

}  // namespace internal

#define THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)          \
  Napi::TypeError::New(env, "Wrong number of arguments") \
      .ThrowAsJavaScriptException()

#define JS_CHECK_NUM_ARGS(env, num_args) \
  if (info.Length() != num_args) THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)

template <typename R, typename... Args>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (*f)(Args...)) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return TypeConvertor<std::decay_t<R>>::ToJSValue(env,
                                                   internal::Invoke(info, f));
}

template <typename... Args>
void TypedCall(const Napi::CallbackInfo& info, void (*f)(Args...)) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f);
}

template <typename R, typename Class, typename... Args>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (Class::*f)(Args...),
                      Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...),
               Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

template <typename R, typename Class, typename... Args>
Napi::Value TypedCall(const Napi::CallbackInfo& info,
                      R (Class::*f)(Args...) const, const Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) const,
               const Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

template <typename R, typename Class, typename... Args>
Napi::Value TypedCall(const Napi::CallbackInfo& info,
                      R (Class::*f)(Args...) const&, const Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) const&,
               const Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

template <typename R, typename Class, typename... Args>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&,
                      Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  return TypeConvertor<std::decay_t<R>>::ToJSValue(
      env, internal::Invoke(info, f, c));
}

template <typename Class, typename... Args>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) &&,
               Class* c) {
  Napi::Env env = info.Env();
  constexpr size_t num_args = sizeof...(Args);
  JS_CHECK_NUM_ARGS(env, num_args);

  internal::Invoke(info, f, c);
}

}  // namespace node_binding

#endif  // NODE_BINDING_TYPED_CALL_H_