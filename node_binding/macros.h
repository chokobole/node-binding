// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_MACROS_H_
#define NODE_BINDING_MACROS_H_

#include "napi.h"

#include "node_binding/template_util.h"
#include "node_binding/type_convertor.h"

#define THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)            \
  ::Napi::TypeError::New(env, "Wrong number of arguments") \
      .ThrowAsJavaScriptException()

#define JS_CHECK_NUM_ARGS(info, num_args) \
  if (info.Length() != num_args) THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(info.Env())

#ifdef NAPI_CPP_EXCEPTIONS
#define RETURN_IF_HAS_PENDING_EXCEPTION(env)
#define RETURN_UNDEFINED_IF_HAS_PENDING_EXCEPTION(env)
#define RETURN_NULL_IF_HAS_PENDING_EXCEPTION(env)
#else
#define RETURN_IF_HAS_PENDING_EXCEPTION(env) \
  if (env.IsExceptionPending()) return
#define RETURN_UNDEFINED_IF_HAS_PENDING_EXCEPTION(env) \
  if (env.IsExceptionPending()) return env.Undefined()
#define RETURN_NULL_IF_HAS_PENDING_EXCEPTION(env) \
  if (env.IsExceptionPending()) return env.Null()
#endif

#define RETURN_UNDEFINED_IF_FAILED_TO_CHECK_ARGS()                      \
  ::Napi::Env env = info.Env();                                         \
  constexpr size_t num_args = sizeof...(Args) - sizeof...(DefaultArgs); \
  JS_CHECK_NUM_ARGS(info, num_args);                                    \
  RETURN_UNDEFINED_IF_HAS_PENDING_EXCEPTION(env);                       \
  ::node_binding::ArgTypeChecker<Args...>::Check(info, 0, num_args);    \
  RETURN_UNDEFINED_IF_HAS_PENDING_EXCEPTION(env)

#define RETURN_IF_FAILED_TO_CHECK_ARGS()                                \
  ::Napi::Env env = info.Env();                                         \
  constexpr size_t num_args = sizeof...(Args) - sizeof...(DefaultArgs); \
  JS_CHECK_NUM_ARGS(info, num_args);                                    \
  RETURN_IF_HAS_PENDING_EXCEPTION(env);                                 \
  ::node_binding::ArgTypeChecker<Args...>::Check(info, 0, num_args);    \
  RETURN_IF_HAS_PENDING_EXCEPTION(env)

#endif  // NODE_BINDING_MACROS_H_