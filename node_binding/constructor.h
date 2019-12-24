// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_CONSTRUCTOR_H_
#define NODE_BINDING_CONSTRUCTOR_H_

#include "node_binding/typed_call.h"

namespace node_binding {

template <typename Class>
struct Constructor {
  template <typename... Args>
  static Class Call(Args&&... args) {
    return Class(std::forward<Args>(args)...);
  };

  template <typename... Args>
  static Class* CallNew(Args&&... args) {
    return new Class(std::forward<Args>(args)...);
  };
};

template <typename R, typename... Args, typename... DefaultArgs>
R TypedConstruct(const Napi::CallbackInfo& info, R (*f)(Args...),
                 DefaultArgs&&... def_args) {
  constexpr size_t num_args = sizeof...(Args) - sizeof...(DefaultArgs);
  JS_CHECK_NUM_ARGS(info, num_args);

  return internal::Invoke<num_args>(info, f,
                                    std::forward<DefaultArgs>(def_args)...);
}

}  // namespace node_binding

#endif  // NODE_BINDING_CONSTRUCTOR_H_