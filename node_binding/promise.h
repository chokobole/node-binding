// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_PROMISE_H_
#define NODE_BINDING_PROMISE_H_

#include <atomic>
#include <functional>
#include <string>

#include "node_binding/stl.h"

namespace node_binding {
/**
 * @brief
 *
 */
class cancel_context {
 public:
  cancel_context() : canceled_(false) {}

  bool canceled() const { return canceled_; }
  void cancel() { canceled_ = true; }

 private:
  std::atomic_bool canceled_;
};

using cancel_context_ptr = std::shared_ptr<cancel_context>;

namespace internal {
template <typename... Args>
static auto ToPromise(std::function<void(Args...)>);

template <typename R, typename... Args>
static auto ToPromise(std::function<R(Args...)>);

template <typename... Args>
static auto ToCancellablePromise(
    std::function<void(cancel_context_ptr, Args...)> f);

template <typename R, typename... Args>
static auto ToCancellablePromise(
    std::function<R(cancel_context_ptr, Args...)> f);
}  // namespace internal

/**
 * @brief Function pointer, Lambda function, std::function -->
 * node_binding::async_function
 *
 */
class async_function
    : public std::function<Napi::Value(const Napi::CallbackInfo&)> {
 private:
  using Ft = std::function<Napi::Value(const Napi::CallbackInfo&)>;
  using Ft::Ft;

 public:
#if CXX_VER >= 201703
  template <typename T>
  static async_function from(T lambda) {
    return from(std::function(lambda));
  }
#endif
  template <typename T>
  static async_function from(std::function<T> function) {
    return internal::ToPromise(function);
  }

  template <typename... Args>
  static async_function from(void (&f)(Args...)) {
    return internal::ToPromise(std::function<void(Args...)>(f));
  }

  template <typename R, typename... Args>
  static async_function from(R (&f)(Args...)) {
    return internal::ToPromise(std::function<R(Args...)>(f));
  }
};

/**
 * @brief node_binding::async_function <--> Napi::Function
 *
 */
template <>
class TypeConvertor<async_function> {
 public:
  static Napi::Value ToNativeValue(const Napi::Value& value) { return value; }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const async_function& value) {
    return Napi::Function::New(env, value);
  }
};

/**
 * @brief Function pointer, Lambda function, std::function -->
 * node_binding::cancellable_async_function
 *
 */
class cancellable_async_function
    : public std::function<Napi::Value(const Napi::CallbackInfo&)> {
 private:
  using Ft = std::function<Napi::Value(const Napi::CallbackInfo&)>;
  using Ft::Ft;

 public:
#if CXX_VER >= 201703
  template <typename T>
  static cancellable_async_function from(T lambda) {
    return from(std::function(lambda));
  }
#endif
  template <typename T>
  static cancellable_async_function from(std::function<T> function) {
    return internal::ToCancellablePromise(function);
  }

  template <typename... Args>
  static cancellable_async_function from(void (&f)(Args...)) {
    return internal::ToCancellablePromise(std::function<void(Args...)>(f));
  }

  template <typename R, typename... Args>
  static cancellable_async_function from(R (&f)(Args...)) {
    return internal::ToCancellablePromise(std::function<R(Args...)>(f));
  }
};

/**
 * @brief node_binding::cancellable_async_function <--> Napi::Function
 *
 */
template <>
class TypeConvertor<cancellable_async_function> {
 public:
  static Napi::Value ToNativeValue(const Napi::Value& value) { return value; }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const cancellable_async_function& value) {
    return Napi::Function::New(env, value);
  }
};
}  // namespace node_binding

namespace node_binding {
namespace internal {

static Napi::Object MakeRejectValue(const Napi::Env& env,
                                    const std::string& status, bool native,
                                    const Napi::Value& result) {
  Napi::Object obj = Napi::Object::New(env);
  if (!result.IsEmpty())
    obj.DefineProperty(Napi::PropertyDescriptor::Value("result", result));
  if (native)
    obj.DefineProperty(Napi::PropertyDescriptor::Value(
        "native", Napi::Boolean::New(env, native)));
  obj.DefineProperty(Napi::PropertyDescriptor::Value(
      "status", Napi::String::New(env, status)));
  return obj;
}

static void Reject(const Napi::ThreadSafeFunction& tsfn,
                   const Napi::Promise::Deferred& deferred,
                   const std::string& status, const std::string& message = {}) {
  tsfn.BlockingCall([deferred, status, message](Napi::Env env, Napi::Function) {
    Napi::String result;
    if (!message.empty()) result = Napi::String::New(env, message);
    deferred.Reject(MakeRejectValue(env, status, true, result));
  });
}

static void Reject(const Napi::Env& env,
                   const Napi::Promise::Deferred& deferred,
                   const std::string& status, const std::string& message = {}) {
  Napi::String result;
  if (!message.empty()) result = Napi::String::New(env, message);
  deferred.Reject(MakeRejectValue(env, status, false, result));
}

template <typename T>
static void Reject(const Napi::ThreadSafeFunction& tsfn,
                   const Napi::Promise::Deferred& deferred,
                   const std::string& status, const T& result) {
  tsfn.BlockingCall([deferred, status, result](Napi::Env env, Napi::Function) {
    deferred.Reject(MakeRejectValue(env, status, true,
                                    node_binding::ToJSValue(env, result)));
  });
}

template <typename T>
static void Reject(const Napi::Env& env,
                   const Napi::Promise::Deferred& deferred,
                   const std::string& status, const T& result) {
  deferred.Reject(MakeRejectValue(env, status, false,
                                  node_binding::ToJSValue(env, result)));
}

static void Resolve(const Napi::ThreadSafeFunction& tsfn,
                    const Napi::Promise::Deferred& deferred) {
  tsfn.BlockingCall([deferred](Napi::Env env, Napi::Function) {
    deferred.Resolve(env.Undefined());
  });
}

template <typename T>
static void Resolve(const Napi::ThreadSafeFunction& tsfn,
                    const Napi::Promise::Deferred& deferred, const T& value) {
  tsfn.BlockingCall([value, deferred](Napi::Env env, Napi::Function) {
    deferred.Resolve(node_binding::ToJSValue(env, value));
  });
}

/**
 * @brief
 *
 */
class AsyncWorker : public Napi::AsyncWorker {
 public:
  AsyncWorker(Napi::Env env, std::function<void()> on_execute)
      : Napi::AsyncWorker(env), on_execute_(on_execute) {
    Napi::AsyncWorker::Queue();
  }

  AsyncWorker(Napi::Env env) : Napi::AsyncWorker(env) {}

  ~AsyncWorker() {
    if (on_destory_) on_destory_();
  }

  void Execute() override {
    if (on_execute_) on_execute_();
  }

  void Queue(std::function<void()> on_execute,
             std::function<void()> on_destory = nullptr) {
    on_execute_ = on_execute;
    on_destory_ = on_destory;
    Napi::AsyncWorker::Queue();
  }

 private:
  std::function<void()> on_execute_;
  std::function<void()> on_destory_;
};

/**
 * @brief
 *
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename... Args>
static auto ToPromise(std::function<void(Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(), "ToPromise<void(Args...)> resource", 0, 1);
    node_binding::TypedCall(
        info,
        std::function<void(Args...)>([tsfn, deferred, f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            new AsyncWorker(deferred.Env(),
                            [tsfn, deferred, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
                              try {
#endif
                                f(args...);
                                Resolve(tsfn, deferred);
#ifdef CXX_EXCEPTIONS
                              } catch (const std::exception& e) {
                                Reject(tsfn, deferred, "error", e.what());
                              }
#endif
                              tsfn.Release();
                            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return deferred.Promise();
  };
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename R, typename... Args>
static auto ToPromise(std::function<R(Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(), "ToPromise<R(Args...)> resource", 0, 1);
    node_binding::TypedCall(
        info,
        std::function<void(Args...)>([deferred, tsfn, f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            new AsyncWorker(deferred.Env(),
                            [deferred, tsfn, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
                              try {
#endif
                                Resolve(tsfn, deferred, f(args...));
#ifdef CXX_EXCEPTIONS
                              } catch (const std::exception& e) {
                                Reject(tsfn, deferred, "error", e.what());
                              }
#endif
                              tsfn.Release();
                            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return deferred.Promise();
  };
}

/**
 * @brief
 *
 */
class AsyncWorkerWithMutex {
 public:
  AsyncWorkerWithMutex(Napi::Env env) : ptr(new AsyncWorker(env)) {}

  bool Cancel() {
    std::unique_lock<std::mutex> lk(mtx);
    if (ptr == nullptr) return false;
    ptr->Cancel();
    return true;
  }

  void Queue(std::function<void()> on_execute) {
    ptr->Queue(on_execute, [this]() {
      std::unique_lock<std::mutex> lk(mtx);
      ptr = nullptr;
    });
  }

 private:
  AsyncWorker* ptr;
  std::mutex mtx;
};

/**
 * @brief
 *
 * @param worker
 * @param tsfn
 * @param deferred
 * @param context
 * @return Napi::Object
 */
static Napi::Object MakeCancellablePromiseObject(
    std::shared_ptr<AsyncWorkerWithMutex> worker,
    Napi::ThreadSafeFunction& tsfn, const Napi::Promise::Deferred& deferred,
    cancel_context_ptr context = nullptr) {
  Napi::Object obj = Napi::Object::New(deferred.Env());
  obj.DefineProperties(
      {Napi::PropertyDescriptor::Value("promise", deferred.Promise()),
       Napi::PropertyDescriptor::Function(
           "cancel", [worker, tsfn, deferred,
                      context](const Napi::CallbackInfo&) mutable {
             if (context) context->cancel();
#ifdef CXX_EXCEPTIONS
             try {
#endif
               if (!worker->Cancel()) return;
               Napi::Env env = deferred.Env();
#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
               if (env.IsExceptionPending()) {
                 env.GetAndClearPendingException();
                 return;
               }
#endif
               tsfn.Release();
               Reject(env, deferred, "canceled");
#ifdef CXX_EXCEPTIONS
             } catch (Napi::Error&) {
             }
#endif
           })});
  return obj;
}

/**
 * @brief
 *
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename... Args>
static auto ToCancellablePromise(std::function<void(Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(),
        "ToCancellablePromise<void(ctx, Args...)> resource", 0, 1);
    std::shared_ptr<AsyncWorkerWithMutex> worker =
        std::make_shared<AsyncWorkerWithMutex>(env);
    node_binding::TypedCall(
        info, std::function<void(Args...)>([worker, deferred, tsfn,
                                            f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            worker->Queue([worker, deferred, tsfn, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
              try {
#endif
                f(args...);
                Resolve(tsfn, deferred);
#ifdef CXX_EXCEPTIONS
              } catch (const std::exception& e) {
                Reject(tsfn, deferred, "error", e.what());
              }
#endif
              tsfn.Release();
            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return MakeCancellablePromiseObject(worker, tsfn, deferred);
  };
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename R, typename... Args>
static auto ToCancellablePromise(std::function<R(Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(), "ToCancellablePromise<R(ctx, Args...)> resource",
        0, 1);
    std::shared_ptr<AsyncWorkerWithMutex> worker =
        std::make_shared<AsyncWorkerWithMutex>(env);
    node_binding::TypedCall(
        info, std::function<void(Args...)>([worker, deferred, tsfn,
                                            f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            worker->Queue([worker, deferred, tsfn, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
              try {
#endif
                Resolve(tsfn, deferred, f(args...));
#ifdef CXX_EXCEPTIONS
              } catch (const std::exception& e) {
                Reject(tsfn, deferred, "error", e.what());
              }
#endif
              tsfn.Release();
            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return MakeCancellablePromiseObject(worker, tsfn, deferred);
  };
}

/**
 * @brief
 *
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename... Args>
static auto ToCancellablePromise(
    std::function<void(cancel_context_ptr, Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(),
        "ToCancellablePromise<void(ctx, Args...)> resource", 0, 1);
    std::shared_ptr<AsyncWorkerWithMutex> worker =
        std::make_shared<AsyncWorkerWithMutex>(env);
    cancel_context_ptr ctx = std::make_shared<cancel_context>();
    node_binding::TypedCall(
        info, std::function<void(Args...)>([worker, ctx, deferred, tsfn,
                                            f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            worker->Queue([worker, ctx, deferred, tsfn, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
              try {
#endif
                f(ctx, args...);
                if (ctx->canceled()) {
                  Reject(tsfn, deferred, "canceled");
                } else {
                  Resolve(tsfn, deferred);
                }
#ifdef CXX_EXCEPTIONS
              } catch (const std::exception& e) {
                Reject(tsfn, deferred, "error", e.what());
              }
#endif
              tsfn.Release();
            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return MakeCancellablePromiseObject(worker, tsfn, deferred, ctx);
  };
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param f
 * @return auto
 */
template <typename R, typename... Args>
static auto ToCancellablePromise(
    std::function<R(cancel_context_ptr, Args...)> f) {
  return [f](const Napi::CallbackInfo& info) -> Napi::Value {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function(), "ToCancellablePromise<R(ctx, Args...)> resource",
        0, 1);
    std::shared_ptr<AsyncWorkerWithMutex> worker =
        std::make_shared<AsyncWorkerWithMutex>(env);
    cancel_context_ptr ctx = std::make_shared<cancel_context>();
    node_binding::TypedCall(
        info, std::function<void(Args...)>([worker, ctx, deferred, tsfn,
                                            f](Args... args) mutable {
#ifdef CXX_EXCEPTIONS
          try {
#endif
            worker->Queue([worker, ctx, deferred, tsfn, f, args...]() mutable {
#ifdef CXX_EXCEPTIONS
              try {
#endif
                R ret = f(ctx, args...);
                if (ctx->canceled()) {
                  Reject(tsfn, deferred, "canceled", ret);
                } else {
                  Resolve(tsfn, deferred, ret);
                }
#ifdef CXX_EXCEPTIONS
              } catch (const std::exception& e) {
                Reject(tsfn, deferred, "error", e.what());
              }
#endif
              tsfn.Release();
            });
#ifdef CXX_EXCEPTIONS
          } catch (const std::exception& e) {
            Reject(tsfn, deferred, "error", e.what());
            tsfn.Release();
          }
#endif
        }));
    return MakeCancellablePromiseObject(worker, tsfn, deferred, ctx);
  };
}
}  // namespace internal

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToPromise(const Napi::Env& env, void (*f)(Args...)) {
  return Napi::Function::New(
      env, internal::ToPromise(std::function<void(Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToPromise(const Napi::Env& env,
                             std::function<void(Args...)> f) {
  return Napi::Function::New(env, internal::ToPromise(f));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename R, typename... Args>
static Napi::Value ToPromise(const Napi::Env& env, R (*f)(Args...)) {
  return Napi::Function::New(env,
                             internal::ToPromise(std::function<R(Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename R, typename... Args>
static Napi::Value ToPromise(const Napi::Env& env,
                             std::function<R(Args...)> f) {
  return Napi::Function::New(env, internal::ToPromise(f));
}

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        void (*f)(Args...)) {
  return Napi::Function::New(
      env, internal::ToCancellablePromise(std::function<void(Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        std::function<void(Args...)> f) {
  return Napi::Function::New(env, internal::ToCancellablePromise(f));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename R, typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env, R (*f)(Args...)) {
  return Napi::Function::New(
      env, internal::ToCancellablePromise(std::function<R(Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename R, typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        std::function<R(Args...)> f) {
  return Napi::Function::New(env, internal::ToCancellablePromise(f));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */

template <typename R, typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        R (*f)(cancel_context_ptr, Args...)) {
  return Napi::Function::New(
      env, internal::ToCancellablePromise(
               std::function<R(cancel_context_ptr, Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam R
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */

template <typename R, typename... Args>
static Napi::Value ToCancellablePromise(
    const Napi::Env& env, std::function<R(cancel_context_ptr, Args...)> f) {
  return Napi::Function::New(env, internal::ToCancellablePromise(f));
}

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        void (*f)(cancel_context_ptr,
                                                  Args...)) {
  return Napi::Function::New(
      env, internal::ToCancellablePromise(
               std::function<void(cancel_context_ptr, Args...)>(f)));
}

/**
 * @brief
 *
 * @tparam Args
 * @param env
 * @param f
 * @return Napi::Value
 */
template <typename... Args>
static Napi::Value ToCancellablePromise(
    const Napi::Env& env, std::function<void(cancel_context_ptr, Args...)> f) {
  return Napi::Function::New(env, internal::ToCancellablePromise(f));
}
}  // namespace node_binding
#endif  // NODE_BINDING_PROMISE_H_