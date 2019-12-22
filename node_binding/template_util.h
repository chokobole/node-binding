// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_TEMPLATE_UTIL_H_
#define NODE_BINDING_TEMPLATE_UTIL_H_

#include <stddef.h>

#include <type_traits>

namespace node_binding {
namespace internal {

template <typename... Types>
struct TypeList {};

template <size_t n, typename List>
struct PickTypeListItemImpl;

template <size_t n, typename T, typename... List>
struct PickTypeListItemImpl<n, TypeList<T, List...>>
    : PickTypeListItemImpl<n - 1, TypeList<List...>> {};

template <typename T, typename... List>
struct PickTypeListItemImpl<0, TypeList<T, List...>> {
  using Type = std::decay_t<T>;
};

template <size_t n, typename List>
using PickTypeListItem = typename PickTypeListItemImpl<n, List>::Type;

}  // namespace internal
}  // namespace node_binding

#endif  // NODE_BINDING_TEMPLATE_UTIL_H_