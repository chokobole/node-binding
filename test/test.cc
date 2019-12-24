#include "node_binding/typed_call.h"

double CAdd(double arg0 = 1, double arg1 = 2) { return arg0 + arg1; }

Napi::Value Add(const Napi::CallbackInfo& info) {
  if (info.Length() == 0) {
    return node_binding::TypedCall(info, &CAdd, 1, 2);
  } else if (info.Length() == 1) {
    return node_binding::TypedCall(info, &CAdd, 2);
  } else {
    return node_binding::TypedCall(info, &CAdd);
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("add", Napi::Function::New(env, Add));
  return exports;
}

NODE_API_MODULE(test, Init)