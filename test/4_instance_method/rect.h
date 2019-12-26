// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

class Rect {
 public:
  Rect(int width, int height) : width_(width), height_(height) {}

  int size() const { return width_ * height_; }

 private:
  int width_;
  int height_;
};