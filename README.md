# Node Binding

This is a helper to bind `c++` to `nodejs` using [node-addon-api](https://github.com/nodejs/node-addon-api) easily.

## Contents
- [Node Binding](#node-binding)
  - [Contents](#contents)
  - [Overview](#overview)
  - [How to use](#how-to-use)
    - [bazel](#bazel)
    - [node-gyp](#node-gyp)
  - [Usages](#usages)
    - [InstanceMethod with default arguments](#instancemethod-with-default-arguments)
    - [Constructor](#constructor)
    - [InstanceAccessor](#instanceaccessor)
    - [STL containers](#stl-containers)
    - [Conversion](#conversion)
    - [Custom Conversion](#custom-conversion)

## Overview

Please don't repeat `argument checking` or `type conversion`.

```c++
#include "napi.h"

double CAdd(double arg0, double arg1) { return arg0 + arg1; }

Napi::Value Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!info[0].IsNumber() || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }
  double arg0 = info[0].As<Napi::Number>().DoubleValue();
  double arg1 = info[1].As<Napi::Number>().DoubleValue();
  Napi::Number num = Napi::Number::New(env, CAdd(arg0, arg1);
  return num;
}
```

Instead do like below! Just this one line does everything above.

```c++
#include "node_binding/typed_call.h"

double CAdd(double arg0, double arg1) { return arg0 + arg1; }

Napi::Value Add(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CAdd);
}
```

## How to use

### bazel

To use `node_binding`, add the followings to your `WORKSPACE` file.

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "node_binding",
    sha256 = "<sha256>",
    strip_prefix = "node-binding-<commit>",
    urls = [
        "https://github.com/chokobole/node-binding/archive/<commit>.tar.gz",
    ],
)

load("@node_binding//bazel:node_binding_deps.bzl", "node_binding_deps")

node_binding_deps()

load("@node_binding//third_party/node_addon_api:install_node_addon_api.bzl", "install_node_addon_api")

install_node_addon_api(name = "node_addon_api")
```

Then, in your `BUILD` files, import and use the rules.

```python
load("@node_binding//bazel:node_binding.bzl", "node_binding")
load("@node_binding//bazel:node_binding_cc.bzl", "node_binding_copts")

node_binding(
    name = "name",
    srcs = [
      ...
    ],
    copts = node_binding_copts(),
    deps = [
        "@node_binding",
    ],
)
```

Then this rule generates `name.node`.

### node-gyp

To use `node-binding`, install package via `npm install`.

```bash
npm install node-binding
```

Follow [examples](https://github.com/nodejs/node-addon-examples)! But in `include_dirs`, fill like below.

```python
{
  "targets": [
    {
      ...
      "include_dirs": [
        "<!@(node -p \"require('node-binding').include\")",
      ],
    }
  ]
}
```

## Usages

### InstanceMethod with default arguments

```c++
// examples/calculator.h
class Calculator {
 public:
  void Increment(int a = 1) { result_ += a; }

 private:
  int result_;
};
```

```c++
// examples/calculator_js.h
class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  void Increment(const Napi::CallbackInfo& info);
};
```

```c++
// examples/calculator_js.cc
void CalculatorJs::Increment(const Napi::CallbackInfo& info) {
  if (info.Length() == 0) {
    TypedCall(info, &Calculator::Increment, calculator_.get(), 1);
  } else {
    TypedCall(info, &Calculator::Increment, calculator_.get());
  }
}
```

```js
// examples/calculator.js
const c = new calculator.Calculator();
c.increment();
c.increment(1);
```

### Constructor

To bind constructor, you have to include `#include "node_binding/constructor.h"`.

`Constructor<Class>::CallNew<Args>` calls `new Class(Args)`, whereas, `Constructor<Class>::Calls<Args>` calls `Class(Args)`.

```c++
// examples/calculator.h
class Calculator {
 public:
  Calculator() : result_(0) {}
  explicit Calculator(int result) : result_(result) {}

 private:
  int result_;
};
```

```c++
// examples/calculator_js.h
class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  CalculatorJs(const Napi::CallbackInfo& info);
};
```

```c++
// examples/calculator_js.cc
#include "node_binding/constructor.h"

CalculatorJs::CalculatorJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CalculatorJs>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) {
    calculator_ = std::unique_ptr<Calculator>(
        TypedConstruct(info, &Constructor<Calculator>::CallNew<>));
  } else if (info.Length() == 1) {
    calculator_ = std::unique_ptr<Calculator>(
        TypedConstruct(info, &Constructor<Calculator>::CallNew<int>));
  } else {
    THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
  }

  if (env.IsExceptionPending()) calculator_.reset();
}
```

```js
// examples/calculator.js
new Calculator();
new Calculator(0);
new Calculator(0, 0);  // Throws exception!
```

### InstanceAccessor

```c++
// examples/point.h
struct Point {
  int x;
  int y;

  Point(int x = 0, int y = 0) : x(x), y(y) {}
};
```

```c++
// examples/point_js.h
class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  void SetX(const Napi::CallbackInfo& info, const Napi::Value& v);
  void SetY(const Napi::CallbackInfo& info, const Napi::Value& v);

  Napi::Value GetX(const Napi::CallbackInfo& info);
  Napi::Value GetY(const Napi::CallbackInfo& info);

 private:
  Point point_;
};
```

```c++
// examples/point_js.cc
void PointJs::SetX(const Napi::CallbackInfo& info, const Napi::Value& v) {
  point_.x = ToNativeValue<int>(v);
}

void PointJs::SetY(const Napi::CallbackInfo& info, const Napi::Value& v) {
  point_.y = ToNativeValue<int>(v);
}

Napi::Value PointJs::GetX(const Napi::CallbackInfo& info) {
  return ToJSValue(info, point_.x);
}

Napi::Value PointJs::GetY(const Napi::CallbackInfo& info) {
  return ToJSValue(info, point_.y);
}

```

```js
// examples/point.js
const p = new Point();
p.x = 1;
p.y = p.x * 2;
```

### STL containers

To bind `std::vector<T>`, you have to include `#include "node_binding/stl.h"`.

```c++
// test/4_stl/addon.cc
#include "node_binding/stl.h"

int Sum(const std::vector<int>& vec) {
  int ret = 0;
  for (int v: vec) {
    ret += v;
  }
  return ret;
}

std::vector<int> LinSpace(int from, int to, int step) {
  std::vector<int> ret;
  for (int i = from ;i < to; i += step) {
    ret.push_back(i);
  }
  return ret;
}
```

```js
// test/test.js
console.log(sum([1, 2, 3]));  // 6
console.log(linSpace(1, 5, 1));  // [1, 2, 3, 4]
```

### Conversion

| c++         | js                | REFERENCE                          |
| ----------: | ----------------: | ---------------------------------: |
| bool        | boolean           |                                    |
| uint8_t     | number            |                                    |
| int8_t      | number            |                                    |
| uint16_t    | number            |                                    |
| int16_t     | number            |                                    |
| uint32_t    | number            |                                    |
| int32_t     | number            |                                    |
| uint64_t    | number or BigInt  | BigInt if NAPI_EXPERIMENTAL is on  |
| uint64_t    | number or BigInt  | BigInt if NAPI_EXPERIMENTAL is on  |
| float       | number            |                                    |
| double      | number            |                                    |
| std::string | string            |                                    |
| std::vector | Array             |                                    |

### Custom Conversion

To bind your own class, you have to include `#include "node_binding/type_convertor.h"` and write your own specialized template class `TypeConvertor<>` for your own class.

```c++
// examples/point_js.h
#include "node_binding/type_convertor.h"

class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  static Napi::Object New(Napi::Env env, const Point& p);
};

namespace node_binding {

template <>
class TypeConvertor<Point> {
 public:
  static Point ToNativeValue(const Napi::Value& value) {
    Napi::Object obj = value.As<Napi::Object>();

    return {
        TypeConvertor<int>::ToNativeValue(obj["x"]),
        TypeConvertor<int>::ToNativeValue(obj["y"]),
    };
  }

  static bool IsConvertible(const Napi::Value& value) {
    if (!value.IsObject()) return false;

    Napi::Object obj = value.As<Napi::Object>();

    return TypeConvertor<int>::IsConvertible(obj["x"]) &&
           TypeConvertor<int>::IsConvertible(obj["y"]);
  }

  static Napi::Value ToJSValue(const Napi::CallbackInfo& info,
                               const Point& value) {
    return PointJs::New(info.Env(), value);
  }
};

}  // namespace node_binding
```

```c++
// examples/point_js.cc
Napi::Object PointJs::New(Napi::Env env, const Point& p) {
  Napi::EscapableHandleScope scope(env);

  Napi::Object object = constructor_.New({
      Napi::Number::New(env, p.x),
      Napi::Number::New(env, p.y),
  });

  return scope.Escape(napi_value(object)).ToObject();
}
```

```c++
// examples/rect.h
#include "examples/point.h"

struct Rect {
  Point top_left;
  Point bottom_right;
};
```

```c++
// examples/rect_js.h
class RectJs : public Napi::ObjectWrap<RectJs> {
 public:
  void SetTopLeft(const Napi::CallbackInfo& info, const Napi::Value& v);
  void SetBottomRight(const Napi::CallbackInfo& info, const Napi::Value& v);

  Napi::Value GetTopLeft(const Napi::CallbackInfo& info);
  Napi::Value GetBottomRight(const Napi::CallbackInfo& info);
};
```

```c++
// examples/rect_js.cc
#include "examples/point_js.h"

void RectJs::SetTopLeft(const Napi::CallbackInfo& info, const Napi::Value& v) {
  if (IsConvertible<Point>(v)) {
    rect_.top_left = ToNativeValue<Point>(v);
  }
}

void RectJs::SetBottomRight(const Napi::CallbackInfo& info,
                            const Napi::Value& v) {
  if (IsConvertible<Point>(v)) {
    rect_.bottom_right = ToNativeValue<Point>(v);
  }
}

Napi::Value RectJs::GetTopLeft(const Napi::CallbackInfo& info) {
  return ToJSValue(info, rect_.top_left);
}

Napi::Value RectJs::GetBottomRight(const Napi::CallbackInfo& info) {
  return ToJSValue(info, rect_.bottom_right);
}
```

```js
// examples/rect.js
const topLeft = new Point(1, 5);
const bottomRight = new Point(5, 1);
const rect = new Rect(topLeft, bottomRight);
```