// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

class Calculator {
 public:
  Calculator() : result_(0) {}
  explicit Calculator(int result) : result_(result) {}

  static int Add(int a, int b) { return a + b; }
  static int Sub(int a, int b) { return a - b; }

  int result() const { return result_; }
  void Increment(int a = 1) { result_ += a; }
  void Decrement(int a = 1) { result_ -= a; }
  void Clear() { result_ = 0; }

 private:
  int result_;
};