// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_MACROS_H_
#define NODE_BINDING_MACROS_H_

#include "napi.h"

#include "node_binding/template_util.h"
#include "node_binding/type_convertor.h"

namespace node_binding {

#define THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)            \
  ::Napi::TypeError::New(env, "Wrong number of arguments") \
      .ThrowAsJavaScriptException()

#define JS_CHECK_NUM_ARGS(env, num_args) \
  if (info.Length() != num_args) THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env)

#define ARG(N) \
  TypeConvertor<internal::PickTypeListItem<N, ArgList>>::ToNativeValue(info[N])

}  // namespace node_binding

#endif  // NODE_BINDING_MACROS_H_