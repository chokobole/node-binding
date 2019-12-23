# Node Binding

This is a helper to bind `c++` to `nodejs` using [node-addon-api](https://github.com/nodejs/node-addon-api) easily.

## How to use

### bazel

```python
load("@com_github_chokobole_node_binding//bazel:node_binding.bzl", "node_binding")

node_binding(
    name = "...",
    srcs = [
      ...
    ],
    copts = select({
        "//:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    }),
    deps = [
        "@com_github_chokobole_node_binding//:node_binding",
    ],
)
```

### node-gyp

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

### Default Arguments

```c++
double CAdd(double arg0, double arg1 = 1) { return arg0 + arg1; }

Napi::Value Add(const Napi::CallbackInfo& info) {
  if (info.Length() == 0) {
    return node_binding::TypedCall(info, &CAdd, 1);
  } else {
    return node_binding::TypedCall(info, &CAdd);
  }
}
```