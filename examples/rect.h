// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "examples/point.h"

struct Rect {
  Point top_left;
  Point bottom_right;

  Rect() {}
  Rect(const Point& top_left, const Point& bottom_right)
      : top_left(top_left), bottom_right(bottom_right) {}

  int Area() {
    return (top_left.y - bottom_right.y) * (bottom_right.x - top_left.x);
  }
};