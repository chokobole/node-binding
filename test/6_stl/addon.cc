// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "node_binding/stl.h"

int CSum(const std::vector<int>& vec) {
  int ret = 0;
  for (int v : vec) {
    ret += v;
  }
  return ret;
}

std::vector<int> CLinSpace(int from, int to, int step) {
  std::vector<int> ret;
  for (int i = from; i < to; i += step) {
    ret.push_back(i);
  }
  return ret;
}

Napi::Value Sum(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CSum);
}

Napi::Value LinSpace(const Napi::CallbackInfo& info) {
  return node_binding::TypedCall(info, &CLinSpace);
}

#ifdef _NODE_BINDING_OBJECT
using object = std::unordered_map<std::string, std::any>;

std::string getName(object object) {
  return std::any_cast<std::string>(object["name"]);
}
// Not yet supported
// bool setName(object object, std::string name) {
//   object["name"] = name;
//   return true;
// }

object getObject(const std::string value) {
  object ret(
      {{"name", value},
       {value + "_bool_array", std::vector<bool>({true, false, true, false})},
       {value + "_int_array", std::vector<int>({1, 2, 3, 4})},
       {value + "_str_array", std::vector<std::string>({"1", "2", "3", "4"})},
       {value + "_double_array", std::vector<double>({1.2, 3.4, 5.6, 7.8})},
       {value + "_fn_array",
        std::vector<node_binding::function>(
#if defined(__clang__)
            {node_binding::function::from<std::string(std::string)>([](std::string arg0) {
#else
            {node_binding::function::from([](std::string arg0) {
#endif
               return std::string("called (fn[0]) : ") + arg0;
             }),
#if defined(__clang__)
             node_binding::function::from<std::string(std::string)>([](std::string arg0) {
#else
             node_binding::function::from([](std::string arg0) {
#endif
               return std::string("called (fn[1]) : ") + arg0;
             })})},
       {"callback",
#if defined(__clang__)
        node_binding::function::from<object(int, std::string)>([](int arg0, std::string arg1) -> object {
#else
        node_binding::function::from([](int arg0, std::string arg1) -> object {
#endif          
          return object({{"arg0", arg0}, {"arg1", arg1}});
        })}});

  object obj;
  obj = ret;
  ret[value + "_obj_array"] = std::vector<object>({obj, obj, obj, obj});
  return ret;
}

object invokeCallback(object data) {
  if (data.end() != data.find("callback")) {
    auto callback =
        node_binding::any_cast<std::function<object(int, std::string)>>(
            data["callback"]);
    if (callback) {
      return callback(1, std::any_cast<std::string>(data["name"]));
    }
  }
  return object();
}

object callback(int arg0, std::string arg1) {
  return object({{"arg0", arg0}, {"arg1", arg1}});
}
#else
Napi::Object callback(int arg0, std::string arg1) {
  Napi::Object obj;
  obj["arg0"] = arg0;
  obj["arg1"] = arg1;
  return obj;
}
#endif

std::string getNameWithNapi(Napi::Object object) {
  return object.Get("name").As<Napi::String>();
}

bool setNameWithNapi(Napi::Object object, std::string name) {
  bool result =
      (std::string)(((Napi::Value)object["name"]).As<Napi::String>()) == "test";
  object["name"] = name;
  return result;
}

Napi::Object getObjectWithNapi(Napi::Value value) {
  const std::string val = value.ToString();
  Napi::Env env = value.Env();

  Napi::Object ret = Napi::Object::New(env);
  ret["name"] = value.ToString();

  ret[val + "_bool_array"] = ::node_binding::ToJSValue(
      env, std::vector<bool>({true, false, true, false}));
  ret[val + "_int_array"] =
      ::node_binding::ToJSValue(env, std::vector<int>({1, 2, 3, 4}));
  ret[val + "_str_array"] = ::node_binding::ToJSValue(
      env, std::vector<std::string>({"1", "2", "3", "4"}));
  ret[val + "_double_array"] =
      ::node_binding::ToJSValue(env, std::vector<double>({1.2, 3.4, 5.6, 7.8}));
  ret[val + "_fn_array"] = ::node_binding::ToJSValue(
      env,
      std::vector<node_binding::function>(
          {node_binding::function::from(
               std::function<std::string(std::string)>([](std::string arg0) {
                 return std::string("called (fn[0]) : ") + arg0;
               })),
           node_binding::function::from(
               std::function<std::string(std::string)>([](std::string arg0) {
                 return std::string("called (fn[1]) : ") + arg0;
               }))}));
  ret["callback"] = ::node_binding::ToJSValue(
      env, node_binding::function::from(
               std::function<Napi::Object(int, std::string)>(
                   [env](int arg0, std::string arg1) -> Napi::Object {
                     Napi::Object ret = Napi::Object::New(env);
                     ret["arg0"] = ::node_binding::ToJSValue(env, arg0);
                     ret["arg1"] = ::node_binding::ToJSValue(env, arg1);
                     return ret;
                   })));

  Napi::Object obj;
  obj = ret;
  ret[val + "_obj_array"] = ::node_binding::ToJSValue(
      env, std::vector<Napi::Object>({obj, obj, obj, obj}));
  return ret;
}

Napi::Object invokeCallbackWithNapi(Napi::Object data) {
  if (data.Has("callback")) {
    return static_cast<Napi::Value>(data["callback"])
        .As<Napi::Function>()
        .Call({node_binding::ToJSValue(data.Env(), 1),
               node_binding::ToJSValue(data.Env(),
                                       static_cast<Napi::Value>(data["name"]))})
        .ToObject();
  }
  return Napi::Object::New(data.Env());
}

#include <chrono>
void delay_operation(std::string msg, long delay) {
  msg.clear();
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void callbackTest(std::string data,
                  int count,
                  int delay,
                  std::function<void(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    callback(data, count--);
  }
}

void callbackTest2(std::string data,
                   int count,
                   int delay,
                   std::function<bool(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    if (!callback(data, count--))
      break;
  }
}

#if (NAPI_VERSION > 3)
#include <iostream>

std::thread callbackThread_;
std::thread callbackThread2_;

bool started = false;
node_binding::thread_safe_function<bool(std::string, int)> callback_;
node_binding::thread_safe_function<bool(std::string, int)> callback2_;

std::string beginMoveTsfnCallbackTest(
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<bool(std::string, int)> callback) {
  if (!started) {
    started = true;
    callback_ = callback;  // move a tsfn object and copy a function object

    callbackThread_ = std::thread([data]() {
      callbackThread_.detach();
#ifdef CXX_EXCEPTIONS
      try {
        callback_ = callback2_;  // Copying objects without tsfn is not allowed.
      } catch (std::exception& e) {
        std::cerr << "[C++ exception] " << data << " " << e.what() << "\n"
                  << std::endl;
      }
#endif
      callback2_ = callback_;  // Move a tsfn object and copy a function object.
      callback_ = callback2_;  // No change

      callbackThread2_ = std::thread([data]() {
        callbackThread2_.detach();

        for (int i = 0; i < 10; i++)
          callback_("native thread 2 - " + data, i);

        callback2_.release();  // 전역 객체이므로 release메서드를 호출하여 직접
                               // 해제해줍니다.
      });

      for (int i = 0; i < 5; i++)
        callback_("native thread 1 " + data, i);

      callback_.release();  // 전역 객체이므로 release메서드를 호출하여 직접
                            // 해제해줍니다.
    });
  }

  while (count) {
    delay_operation(data, delay);
    if (!callback(data, count--)) {
      return std::string("stopped at ")
          .append(data)
          .append(std::to_string(count + 1));
    }
  }
  return "completed";
}

void endMoveTsfnCallbackTest() {
  started = false;
}

void promiseCallbackTest(
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<void(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    callback(data, count--);
  }
}

std::string promiseCallbackTest2(
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<bool(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    if (!callback(data, count--)) {
      return std::string("stopped at ")
          .append(data)
          .append(std::to_string(count + 1));
    }
  }
  return "completed";
}

void cancellablePromiseCallbackTest(
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<void(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    callback(data, count--);
  }
}

std::string cancellablePromiseCallbackTest2(
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<bool(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    delay_operation(data, delay);
    if (!callback(data, count--)) {
      return std::string("stopped at ")
          .append(data)
          .append(std::to_string(count + 1));
    }
  }
  return "completed";
}

void cancellablePromiseCallbackTestWithCancelContext(
    node_binding::cancel_context_ptr ctx,
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<void(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    if (ctx->canceled())
      return;
    delay_operation(data, delay);
    callback(data, count--);
  }
}

std::string cancellablePromiseCallbackTestWithCancelContext2(
    node_binding::cancel_context_ptr ctx,
    std::string data,
    int count,
    int delay,
    node_binding::thread_safe_function<bool(std::string, int)> callback) {
  data.append(" - ").append(__FUNCTION__);
  while (count) {
    if (ctx->canceled()) {
      return std::string("canceled at ")
          .append(data)
          .append(std::to_string(count));
    }
    delay_operation(data, delay);
    if (!callback(data, count--)) {
      return std::string("stopped at ")
          .append(data)
          .append(std::to_string(count + 1));
    }
  }
  return "completed";
}
#endif

#define FN_ENTRY(_env_, _functionName_) \
#_functionName_, ::node_binding::ToJSValue((_env_), _functionName_)

#define PROMISE_FN_ENTRY(_env_, _functionName_) \
#_functionName_, ::node_binding::ToPromise((_env_), _functionName_)

#define CANCELLABLE_PROMISE_FN_ENTRY(_env_, _functionName_) \
#_functionName_, ::node_binding::ToCancellablePromise((_env_), _functionName_)

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("sum", Napi::Function::New(env, Sum));
  exports.Set("linSpace", Napi::Function::New(env, LinSpace));

#ifdef _NODE_BINDING_OBJECT
  exports.Set(FN_ENTRY(env, getName));
  // exports.Set(FN_ENTRY(env, setName));   // Not yet supported
  exports.Set(FN_ENTRY(env, getObject));
  exports.Set(FN_ENTRY(env, invokeCallback));
#endif
  exports.Set(FN_ENTRY(env, getNameWithNapi));
  exports.Set(FN_ENTRY(env, setNameWithNapi));
  exports.Set(FN_ENTRY(env, getObjectWithNapi));
  exports.Set(FN_ENTRY(env, invokeCallbackWithNapi));
  exports.Set(FN_ENTRY(env, callbackTest));
  exports.Set(FN_ENTRY(env, callbackTest2));
#if (NAPI_VERSION > 3)
  exports.Set(PROMISE_FN_ENTRY(env, promiseCallbackTest));
  exports.Set(PROMISE_FN_ENTRY(env, promiseCallbackTest2));
  exports.Set(
      CANCELLABLE_PROMISE_FN_ENTRY(env, cancellablePromiseCallbackTest));
  exports.Set(
      CANCELLABLE_PROMISE_FN_ENTRY(env, cancellablePromiseCallbackTest2));
  exports.Set(CANCELLABLE_PROMISE_FN_ENTRY(
      env, cancellablePromiseCallbackTestWithCancelContext));
  exports.Set(CANCELLABLE_PROMISE_FN_ENTRY(
      env, cancellablePromiseCallbackTestWithCancelContext2));

  exports.Set(PROMISE_FN_ENTRY(env, beginMoveTsfnCallbackTest));
  exports.Set(PROMISE_FN_ENTRY(env, endMoveTsfnCallbackTest));
#endif
  return exports;
}

NODE_API_MODULE(6_stl, Init)