// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_TYPED_CALL_H_
#define NODE_BINDING_TYPED_CALL_H_

#include <functional>
#include <utility>

#include "napi.h"
#include "node_binding/arg_type_checker.h"
#include "node_binding/macros.h"
#include "node_binding/template_util.h"
#include "node_binding/type_convertor.h"

namespace node_binding {

namespace internal {

template <size_t Idx, typename ArgList>
auto Arg(const Napi::CallbackInfo& info) {
  return TypeConvertor<internal::PickTypeListItem<Idx, ArgList>>::ToNativeValue(
      info[Idx]);
}

template <typename R, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, R (*f)(Args...),
         std::index_sequence<Indices...>, DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return f(Arg<Indices, ArgList>(info)...,
           std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, std::function<R(Args...)> f,
         std::index_sequence<Indices...>, DefaultArgs&& ... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return f(Arg<Indices, ArgList>(info)...,
           std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...), Class* c,
         std::index_sequence<Indices...>, DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(Arg<Indices, ArgList>(info)...,
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const,
         const Class* c, std::index_sequence<Indices...>,
         DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(Arg<Indices, ArgList>(info)...,
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) const&,
         const Class* c, std::index_sequence<Indices...>,
         DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return ((*c).*f)(Arg<Indices, ArgList>(info)...,
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&, Class* c,
         std::index_sequence<Indices...>, DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return (std::move(*c).*f)(Arg<Indices, ArgList>(info)...,
                            std::forward<DefaultArgs>(def_args)...);
}

}  // namespace internal

// 호출된 함수에서 throw 숫자;처럼 오브젝트나 함수가 아닌 것을 예외로 던지는
// 경우 에외 발생으로 인해서 Napi::Error 생성 될때
// napi_create_reference함수(node_api.cc:2547)에서
// napi_object_expected 오류가 발생하여
//  if (!(v8_value->IsObject() || v8_value->IsFunction())) {
//    return napi_set_last_error(env, napi_object_expected);
//  }
// Napi::Eror에서 Error::Fatal메서드가 호출되어 중단되는 현상이 발생했습니다.
// 그래서 우선 napi_call_function함수를 직접 호출하여 처리하였습니다.
// #define __USE_FUNCTION_CALL_METHOD__
template <typename R, std::size_t... I, class _Tuple>
R Invoke(const Napi::Function& fn, std::index_sequence<I...>, _Tuple&& tup) {
#ifdef __USE_FUNCTION_CALL_METHOD__
  try {
    Napi::HandleScope scope(fn.Env());
    return TypeConvertor<R>::ToNativeValue(
        fn.Call({{ToJSValue<decltype(std::get<I>(tup))>(
            fn.Env(), std::get<I>(tup))...}}));
  } catch (...) {
    return R();
  }
#else
  Napi::Env env = fn.Env();
  Napi::HandleScope scope(env);
  std::vector<napi_value> args = {
      {ToJSValue<decltype(std::get<I>(tup))>(env, std::get<I>(tup))...}};
  napi_value result;
  napi_status status = napi_call_function(env, env.Undefined(), fn, args.size(),
                                          args.data(), &result);
  if ((status) != napi_ok) return R();
  Napi::Value ret(env, result);
  if (ret.IsUndefined()) return R();
  return TypeConvertor<R>::ToNativeValue(ret);
#endif
}

template <std::size_t... I, class _Tuple>
void Invoke(const Napi::Function& fn, std::index_sequence<I...>, _Tuple&& tup) {
#ifdef __USE_FUNCTION_CALL_METHOD__
  try {
    Napi::HandleScope scope(fn.Env());
    fn.Call({{ToJSValue<decltype(std::get<I>(tup))>(fn.Env(),
                                                    std::get<I>(tup))...}});
  } catch (...) {
  }
#else
  Napi::Env env = fn.Env();
  Napi::HandleScope scope(env);
  std::vector<napi_value> args = {
      {ToJSValue<decltype(std::get<I>(tup))>(env, std::get<I>(tup))...}};
  napi_call_function(env, env.Undefined(), fn, args.size(), args.data(),
                     nullptr);
#endif
}

template <typename R, typename... Args, size_t... Indices,
          typename... DefaultArgs>
R Invoke(const Napi::CallbackInfo& info, std::function<R(Args...)> f,
         std::index_sequence<Indices...>, DefaultArgs&&... def_args) {
  using ArgList = internal::TypeList<Args...>;
  return f(internal::Arg<Indices, ArgList>(info)...,
           std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info,
                      std::function<R(Args...)> f, DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  Napi::EscapableHandleScope scope(info.Env());
  return scope.Escape(ToJSValue(
      env, internal::Invoke(info, f, std::make_index_sequence<num_args>(),
                            std::forward<DefaultArgs>(def_args)...)));
}

template <typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info,
                      std::function<void(Args...)> f, DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, std::make_index_sequence<num_args>(),
                            std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (*f)(Args...),
                      DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  Napi::EscapableHandleScope scope(info.Env());
  return scope.Escape(ToJSValue(
      env, internal::Invoke(info, f, std::make_index_sequence<num_args>(),
                            std::forward<DefaultArgs>(def_args)...)));
}

template <typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info, void (*f)(Args...),
               DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, std::make_index_sequence<num_args>(),
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (Class::*f)(Args...),
                      Class* c, DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  return ToJSValue(
      info.Env(),
      internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                       std::forward<DefaultArgs>(def_args)...));
}

template <typename Class, typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...),
               Class* c, DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info,
                      R (Class::*f)(Args...) const, const Class* c,
                      DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  return ToJSValue(
      info.Env(),
      internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                       std::forward<DefaultArgs>(def_args)...));
}

template <typename Class, typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) const,
               const Class* c, DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info,
                      R (Class::*f)(Args...) const&, const Class* c,
                      DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  return ToJSValue(
      info.Env(),
      internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                       std::forward<DefaultArgs>(def_args)...));
}

template <typename Class, typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) const&,
               const Class* c, DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                   std::forward<DefaultArgs>(def_args)...);
}

template <typename R, typename Class, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (Class::*f)(Args...) &&,
                      Class* c, DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  return ToJSValue(
      info.Env(),
      internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                       std::forward<DefaultArgs>(def_args)...));
}

template <typename Class, typename... Args, typename... DefaultArgs>
void TypedCall(const Napi::CallbackInfo& info, void (Class::*f)(Args...) &&,
               Class* c, DefaultArgs&&... def_args) {
  RETURN_IF_FAILED_TO_CHECK_ARGS();
  internal::Invoke(info, f, c, std::make_index_sequence<num_args>(),
                   std::forward<DefaultArgs>(def_args)...);
}

}  // namespace node_binding

#endif  // NODE_BINDING_TYPED_CALL_H_