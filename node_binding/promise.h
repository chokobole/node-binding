// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_PROMISE_H_
#define NODE_BINDING_PROMISE_H_

#include <functional>
#include <string>

#include "node_binding/stl.h"
#include "node_binding/type_convertor.h"
#include "node_binding/typed_call.h"

namespace node_binding {

namespace internal {

/**
 * @brief
 *
 */
class async_worker : public Napi::AsyncWorker {
 public:
  async_worker(Napi::Env env, std::function<void()> onExecute)
      : Napi::AsyncWorker(env), onExecute_(onExecute) {
    Napi::AsyncWorker::Queue();
  }

  async_worker(Napi::Env env) : Napi::AsyncWorker(env) {}

  ~async_worker() {
    if (onDestory_)
      onDestory_();
  }

  void Execute() override {
    if (onExecute_)
      onExecute_();
  }

  void Queue(std::function<void()> onExecute,
             std::function<void()> onDestory = nullptr) {
    onExecute_ = onExecute;
    onDestory_ = onDestory;
    Napi::AsyncWorker::Queue();
  }

 private:
  std::function<void()> onExecute_;
  std::function<void()> onDestory_;
};

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
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        node_binding::TypedCall(
            info, std::function<void(Args...)>([tsfn, deferred,
                                                f](Args... args) mutable {
              try {
                new internal::async_worker(
                    deferred.Env(), [tsfn, deferred, f, args...]() mutable {
                      try {
                        f(args...);
                        tsfn.BlockingCall(
                            [deferred](Napi::Env env, Napi::Function) {
                              deferred.Resolve(env.Undefined());
                            });
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));
        return deferred.Promise();
      });
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
  return Napi::Function::New(
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        node_binding::TypedCall(
            info, std::function<void(Args...)>([deferred, tsfn,
                                                f](Args... args) mutable {
              try {
                new internal::async_worker(
                    deferred.Env(), [deferred, tsfn, f, args...]() mutable {
                      try {
                        R ret = f(args...);
                        tsfn.BlockingCall([ret, deferred](Napi::Env env,
                                                          Napi::Function) {
                          deferred.Resolve(node_binding::ToJSValue(env, ret));
                        });
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));

        return deferred.Promise();
      });
}

class cancel_context {
 public:
  cancel_context() : canceled_(false) {}

  bool canceled() { return canceled_; }
  void cancel() { canceled_ = true; }

 private:
  std::atomic_bool canceled_;
};

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
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        internal::async_worker* wk = new internal::async_worker(env);
        std::shared_ptr<std::atomic_bool> wk_destroyed =
            std::make_shared<std::atomic_bool>(false);
        node_binding::TypedCall(
            info, std::function<void(Args...)>([wk_destroyed, wk, deferred,
                                                tsfn, f](Args... args) mutable {
              try {
                wk->Queue(
                    [deferred, tsfn, f, args...]() mutable {
                      try {
                        R ret = f(args...);
                        tsfn.BlockingCall([ret, deferred](Napi::Env env,
                                                          Napi::Function) {
                          deferred.Resolve(node_binding::ToJSValue(env, ret));
                        });
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    },
                    [wk_destroyed]() mutable { *wk_destroyed = true; });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));
        Napi::Object object = Napi::Object::New(env);
        object.Set("promise", deferred.Promise());
        object.Set(
            "cancel",
            Napi::Function::New(env, [wk_destroyed, tsfn, deferred,
                                      wk](const Napi::CallbackInfo&) mutable {
              if (*wk_destroyed)
                return;
              try {
                wk->Cancel();
                Napi::Env env = deferred.Env();
#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
                if (env.IsExceptionPending()) {
                  env.GetAndClearPendingException();
                  return;
                }
#endif
                tsfn.Release();
                Napi::Object obj = Napi::Object::New(env);
                obj.Set("status", Napi::String::New(env, "canceled"));
                deferred.Reject(obj);
              } catch (Napi::Error&) {
              }
            }));
        return object;
      });
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
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        internal::async_worker* wk = new internal::async_worker(env);
        std::shared_ptr<std::atomic_bool> wk_destroyed =
            std::make_shared<std::atomic_bool>(false);
        node_binding::TypedCall(
            info, std::function<void(Args...)>([wk_destroyed, wk, deferred,
                                                tsfn, f](Args... args) mutable {
              try {
                wk->Queue(
                    [wk_destroyed, deferred, tsfn, f, args...]() mutable {
                      try {
                        f(args...);
                        tsfn.BlockingCall(
                            [deferred](Napi::Env env, Napi::Function) {
                              deferred.Resolve(env.Undefined());
                            });
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    },
                    [wk_destroyed]() mutable { *wk_destroyed = true; });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));
        Napi::Object object = Napi::Object::New(env);
        object.Set("promise", deferred.Promise());
        object.Set(
            "cancel",
            Napi::Function::New(env, [wk_destroyed, tsfn, deferred,
                                      wk](const Napi::CallbackInfo&) mutable {
              if (*wk_destroyed)
                return;
              try {
                wk->Cancel();
                Napi::Env env = deferred.Env();
#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
                if (env.IsExceptionPending()) {
                  env.GetAndClearPendingException();
                  return;
                }
#endif
                tsfn.Release();
                Napi::Object obj = Napi::Object::New(env);
                obj.Set("status", Napi::String::New(env, "canceled"));
                deferred.Reject(obj);
              } catch (Napi::Error&) {
              }
            }));
        return object;
      });
}

using cancel_context_ptr = std::shared_ptr<cancel_context>;

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
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        internal::async_worker* wk = new internal::async_worker(env);
        std::shared_ptr<std::atomic_bool> wk_destroyed =
            std::make_shared<std::atomic_bool>(false);
        cancel_context_ptr ctx = std::make_shared<cancel_context>();
        node_binding::TypedCall(
            info, std::function<void(Args...)>([wk_destroyed, ctx, wk, deferred,
                                                tsfn, f](Args... args) mutable {
              try {
                wk->Queue(
                    [ctx, deferred, tsfn, f, args...]() mutable {
                      try {
                        R ret = f(ctx, args...);
                        if (ctx->canceled()) {
                          tsfn.BlockingCall(
                              [ret, deferred](Napi::Env env, Napi::Function) {
                                Napi::Object obj = Napi::Object::New(env);
                                obj.Set("status",
                                        Napi::String::New(env, "canceled"));
                                obj.Set("native", true);
                                obj.Set("result",
                                        node_binding::ToJSValue(env, ret));
                                deferred.Reject(obj);
                              });
                        } else {
                          tsfn.BlockingCall([ret, deferred](Napi::Env env,
                                                            Napi::Function) {
                            deferred.Resolve(node_binding::ToJSValue(env, ret));
                          });
                        }
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    },
                    [wk_destroyed]() mutable { *wk_destroyed = true; });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));
        Napi::Object object = Napi::Object::New(env);
        object.Set("promise", deferred.Promise());
        object.Set(
            "cancel",
            Napi::Function::New(env, [wk_destroyed, tsfn, ctx, deferred,
                                      wk](const Napi::CallbackInfo&) mutable {
              ctx->cancel();
              if (*wk_destroyed)
                return;
              try {
                wk->Cancel();
                Napi::Env env = deferred.Env();
#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
                if (env.IsExceptionPending()) {
                  env.GetAndClearPendingException();
                  return;
                }
#endif
                tsfn.Release();
                Napi::Object obj = Napi::Object::New(env);
                obj.Set("status", Napi::String::New(env, "canceled"));
                deferred.Reject(obj);
              } catch (Napi::Error&) {
              }
            }));
        return object;
      });
}

template <typename... Args>
static Napi::Value ToCancellablePromise(const Napi::Env& env,
                                        void (*f)(cancel_context_ptr,
                                                  Args...)) {
  return Napi::Function::New(
      env, [f](const Napi::CallbackInfo& info) -> Napi::Value {
        Napi::Env env = info.Env();
        Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
        Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function(), "ToPromise resource", 0, 1);
        internal::async_worker* wk = new internal::async_worker(env);
        std::shared_ptr<std::atomic_bool> wk_destroyed =
            std::make_shared<std::atomic_bool>(false);
        cancel_context_ptr ctx = std::make_shared<cancel_context>();
        node_binding::TypedCall(
            info, std::function<void(Args...)>([wk_destroyed, ctx, wk, deferred,
                                                tsfn, f](Args... args) mutable {
              try {
                wk->Queue(
                    [wk_destroyed, ctx, deferred, tsfn, f, args...]() mutable {
                      try {
                        f(ctx, args...);
                        if (ctx->canceled()) {
                          tsfn.BlockingCall(
                              [deferred](Napi::Env env, Napi::Function) {
                                Napi::Object obj = Napi::Object::New(env);
                                obj.Set("status",
                                        Napi::String::New(env, "canceled"));
                                obj.Set("native", true);
                                deferred.Reject(obj);
                              });
                        } else {
                          tsfn.BlockingCall(
                              [deferred](Napi::Env env, Napi::Function) {
                                deferred.Resolve(env.Undefined());
                              });
                        }
                      } catch (const std::exception& e) {
                        std::string info = e.what();
                        tsfn.BlockingCall([info, deferred](Napi::Env env,
                                                           Napi::Function) {
                          Napi::Object obj = Napi::Object::New(env);
                          obj.Set("status", Napi::String::New(env, "error"));
                          obj.Set("native", true);
                          obj.Set("result", Napi::String::New(env, info));
                          deferred.Reject(obj);
                        });
                      }
                      tsfn.Release();
                    },
                    [wk_destroyed]() mutable { *wk_destroyed = true; });
              } catch (const std::exception& e) {
                std::string info = e.what();
                tsfn.BlockingCall(
                    [info, deferred](Napi::Env env, Napi::Function) {
                      Napi::Object obj = Napi::Object::New(env);
                      obj.Set("status", Napi::String::New(env, "error"));
                      obj.Set("native", true);
                      obj.Set("result", Napi::String::New(env, info));
                      deferred.Reject(obj);
                    });
                tsfn.Release();
              }
            }));
        Napi::Object object = Napi::Object::New(env);
        object.Set("promise", deferred.Promise());
        object.Set(
            "cancel",
            Napi::Function::New(env, [wk_destroyed, tsfn, ctx, deferred,
                                      wk](const Napi::CallbackInfo&) mutable {
              ctx->cancel();
              if (*wk_destroyed)
                return;
              try {
                wk->Cancel();
                Napi::Env env = deferred.Env();
#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
                if (env.IsExceptionPending()) {
                  env.GetAndClearPendingException();
                  return;
                }
#endif
                tsfn.Release();
                Napi::Object obj = Napi::Object::New(env);
                obj.Set("status", Napi::String::New(env, "canceled"));
                deferred.Reject(obj);
              } catch (Napi::Error&) {
              }
            }));
        return object;
      });
}
}  // namespace node_binding

#endif  // NODE_BINDING_PROMISE_H_