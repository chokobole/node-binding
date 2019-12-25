# Node Binding

This is a helper to bind `c++` to `nodejs` using [node-addon-api](https://github.com/nodejs/node-addon-api) easily.

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

## Examples

Please don't do like below!

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

Instead please do like below!

```c++
#include "node_binding/typed_call.h"

double CAdd(double arg0, double arg1) { return arg0 + arg1; }

Napi::Value Add(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CAdd);
}
```

### InstanceMethod / StaticMethod with Default Arguments

You can find full code at [examples/calculator.cc](examples/calculator.cc).

```c++
class Calculator {
 public:
  void Increment(int a = 1) { result_ += a; }

 private:
  int result_;
};

namespace node_binding {

class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  void Increment(const Napi::CallbackInfo& info) {
    if (info.Length() == 0) {
      TypedCall(info, &Calculator::Increment, calculator_.get(), 1);
    } else {
      TypedCall(info, &Calculator::Increment, calculator_.get());
    }
  }
};

}  // namespace node_binding
```

### Constructor

You can find full code at [examples/calculator.cc](examples/calculator.cc).

`Constructor<Class>::CallNew<Args>` calls `new Class(Args)`, whereas, `Constructor<Class>::Calls<Args>` calls `Class(Args)`.

```c++
class Calculator {
 public:
  Calculator() : result_(0) {}
  explicit Calculator(int result) : result_(result) {}

 private:
  int result_;
};

namespace node_binding {

class CalculatorJs : public Napi::ObjectWrap<CalculatorJs> {
 public:
  CalculatorJs(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CalculatorJs>(info) {
    if (info.Length() == 0) {
      calculator_ = std::unique_ptr<Calculator>(
          TypedConstruct(info, &Constructor<Calculator>::CallNew<>));
    } else if (info.Length() == 1) {
      calculator_ = std::unique_ptr<Calculator>(
          TypedConstruct(info, &Constructor<Calculator>::CallNew<int>));
    } else {
      Napi::Env env = info.Env();
      THROW_JS_WRONG_NUMBER_OF_ARGUMENTS(env);
    }
  }

 private:
  std::unique_ptr<Calculator> calculator_;
};

}  // namespace node_binding
```

### InstanceAccessor

You can find full code at [examples/point.cc](examples/point.cc).

```c++
struct Point {
  int x;
  int y;

  Point(int x = 0, int y = 0) : x(x), y(y) {}
};

namespace node_binding {

class PointJs : public Napi::ObjectWrap<PointJs> {
 public:
  void SetX(const Napi::CallbackInfo& info, const Napi::Value& v) {
    point_.x = ToNativeValue<int>(v);
  }

  void SetY(const Napi::CallbackInfo& info, const Napi::Value& v) {
    point_.y = ToNativeValue<int>(v);
  }

  Napi::Value GetX(const Napi::CallbackInfo& info) {
    return ToJSValue(info, point_.x);
  }

  Napi::Value GetY(const Napi::CallbackInfo& info) {
    return ToJSValue(info, point_.y);
  }

 private:
  Point point_;
};

}  // namespace node_binding
```