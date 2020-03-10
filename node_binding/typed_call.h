// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_TYPED_CALL_H_
#define NODE_BINDING_TYPED_CALL_H_

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

template <typename R, typename... Args, typename... DefaultArgs>
Napi::Value TypedCall(const Napi::CallbackInfo& info, R (*f)(Args...),
                      DefaultArgs&&... def_args) {
  RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS();
  return ToJSValue(
      info.Env(),
      internal::Invoke(info, f, std::make_index_sequence<num_args>(),
                       std::forward<DefaultArgs>(def_args)...));
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