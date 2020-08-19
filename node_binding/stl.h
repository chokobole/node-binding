// Copyright (c) 2019 The NodeBinding Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NODE_BINDING_STL_H_
#define NODE_BINDING_STL_H_

#if defined(_MSVC_LANG)
#define CXX_VER _MSVC_LANG
#else
#define CXX_VER __cplusplus
#endif

#ifdef __cpp_exceptions
#define CXX_EXCEPTIONS
#endif

#if defined(__clang__)
#if __has_feature(cxx_rtti)
#define CXX_RTTI
#endif
#elif defined(__GNUG__)
#if defined(__GXX_RTTI)
#define CXX_RTTI
#endif
#elif defined(_MSC_VER)
#if defined(_CPPRTTI)
#define CXX_RTTI
#endif
#endif

#if CXX_VER >= 201703
#include <any>
#include <unordered_map>
#endif

#include <functional>
#include <future>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

#include "node_binding/type_convertor.h"
#include "node_binding/typed_call.h"

namespace node_binding {

/**
 * @brief std::vector<T> <-> Napi::Array
 *
 * @tparam T
 */
template <typename T>
class TypeConvertor<std::vector<T>> {
 public:
#if CXX_VER >= 201703
  using NativeValueType =
      typename std::invoke_result_t<decltype(&TypeConvertor<T>::ToNativeValue),
                                    Napi::Value>;
#else
  using NativeValueType = typename std::result_of_t<decltype (
      &TypeConvertor<T>::ToNativeValue)(Napi::Value)>;
#endif
  static std::vector<NativeValueType> ToNativeValue(const Napi::Value& value) {
    std::vector<NativeValueType> ret;
    Napi::Array arr = value.As<Napi::Array>();
    ret.reserve(arr.Length());
    for (size_t i = 0; i < arr.Length(); ++i) {
      ret.push_back(TypeConvertor<T>::ToNativeValue(arr[i]));
    }
    return ret;
  }

  static bool IsConvertible(const Napi::Value& value) {
    if (!value.IsArray())
      return false;
    Napi::Array arr = value.As<Napi::Array>();
    for (size_t i = 0; i < arr.Length(); ++i) {
      if (!TypeConvertor<T>::IsConvertible(arr[i]))
        return false;
    }
    return true;
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const std::vector<T>& value) {
    Napi::Array ret = Napi::Array::New(env, value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      ret[i] = TypeConvertor<T>::ToJSValue(env, value[i]);
    }
    return ret;
  }
};

/**
 * @brief function pointer -> Napi::Function
 *
 * @tparam R
 * @tparam Args
 */
template <typename R, typename... Args>
class TypeConvertor<R (*)(Args...)> {
 public:
  static Napi::Value ToJSValue(const Napi::Env& env, R (*value)(Args...)) {
    Napi::Function func = Napi::Function::New(
        env, [value](const Napi::CallbackInfo& info) -> Napi::Value {
          return node_binding::TypedCall(info, value);
        });
    return func;
  }
};

template <typename... Args>
class TypeConvertor<void (*)(Args...)> {
 public:
  static Napi::Value ToJSValue(const Napi::Env& env, void (*value)(Args...)) {
    Napi::Function func =
        Napi::Function::New(env, [value](const Napi::CallbackInfo& info) {
          node_binding::TypedCall(info, value);
        });
    return func;
  }
};

#if (NAPI_VERSION > 3)
/**
 * @brief node_binding::thread_safe_function<function<?>>
 *
 * @tparam T
 */
template <typename Fty_>
class thread_safe_function : public std::function<Fty_> {
  using T = std::function<Fty_>;

 public:
  thread_safe_function() : ptsfn_(nullptr) {}

  thread_safe_function(const thread_safe_function& other)
      : T(*static_cast<const T*>(&other)) {
#ifdef CXX_EXCEPTIONS
    if (!other.ptsfn_)
      throw std::runtime_error("invalid object");
#endif
    ptsfn_ = std::move(other.ptsfn_);
  }

  thread_safe_function(T fn) : T(fn) {}

  thread_safe_function(Napi::ThreadSafeFunction& tsfn, T fn) : T(fn) {
    ptsfn_ = std::make_unique<Napi::ThreadSafeFunction>(tsfn);
  }

  ~thread_safe_function() { release(); }

  thread_safe_function& operator=(const thread_safe_function& rhs) {
    std::unique_lock<std::mutex> lk0(copy_op_mtx_);
    std::unique_lock<std::mutex> lk1(rhs.copy_op_mtx_);

    // 이미 tsfn을 소유한 객체인 경우, 예외를 전달합니다.
    if (ptsfn_) {
#ifdef CXX_EXCEPTIONS
      throw std::runtime_error("already initialized");
#endif
      return *this;
    }

    // tsfn를 소유하지 않은 객체를 대입 복사하는것을 차단합니다.
    if (!rhs.ptsfn_) {
#ifdef CXX_EXCEPTIONS
      throw std::runtime_error("invalid object");
      #endif
      return *this;
    }

    // 이미 같은 tfsn을 공유하는 경우 아무 작업을 하지 않도록 합니다.
    if (tsfn_mtx_ && tsfn_mtx_ == rhs.tsfn_mtx_)
      return *this;

    // function객체를 복사합니다.
    *static_cast<T*>(this) = static_cast<const T&>(rhs);

    ptsfn_ = std::move(rhs.ptsfn_);  // tfsn을 이 객체로 이동합니다.
    if (rhs.tsfn_mtx_) {
      tsfn_mtx_ = rhs.tsfn_mtx_;
    } else {
      tsfn_mtx_ = std::make_shared<std::shared_timed_mutex>();
      rhs.tsfn_mtx_ = tsfn_mtx_;
    }
    tsfn_mtx_->lock_shared();  // 참조를 증가시킵니다.
    return *this;
  }

 public:
  void release() {
    if (ptsfn_) {
      if (tsfn_mtx_) {
        tsfn_mtx_->lock();  // 모든 참조가 해제될때까지 대기합니다.
        tsfn_mtx_->unlock();
        tsfn_mtx_ = nullptr;
      }
      ptsfn_->Release();  // 모든 참조가 해제되었으므로 tsfn을 해제합니다.
      ptsfn_ = nullptr;
    } else {
      if (tsfn_mtx_) {
        tsfn_mtx_->unlock_shared();  // 참조를 감소시킵니다.
        tsfn_mtx_ = nullptr;
      }
    }
  }

 private:
  mutable std::unique_ptr<Napi::ThreadSafeFunction> ptsfn_;
  mutable std::shared_ptr<std::shared_timed_mutex> tsfn_mtx_;
  mutable std::mutex copy_op_mtx_;
};

/**
 * @brief node_binding::thread_safe_function<void(Args...)> <--
 * Napi::ThreadSafeFunction
 *
 * @tparam Args
 */
template <typename... Args>
class TypeConvertor<thread_safe_function<void(Args...)>> {
 public:
  static thread_safe_function<void(Args...)> ToNativeValue(
      const Napi::Value& value) {
    Napi::Function fn = value.As<Napi::Function>();
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        value.Env(), fn, "thread safe function resource", 0, 1);
    // 메인 스레드의 식별자를 저장합니다.
    //  - ToNativeValue 메서드는 메인 스레드에서 호출되는것을 전제 조건으로
    //  합니다.
    std::thread::id tid = std::this_thread::get_id();
    return thread_safe_function<void(Args...)>(
        tsfn, [tid, fn, tsfn](Args... args) {
          // 메인 스레드가 호출했다면 Napi::Function을 직접 호출합니다.
          if (std::this_thread::get_id() == tid) {
            Invoke(fn, std::make_index_sequence<sizeof...(Args)>{},
                   std::forward_as_tuple(args...));
            return;
          }

          tsfn.BlockingCall([args...](Napi::Env env, Napi::Function fn) {
            Invoke(fn, std::make_index_sequence<sizeof...(Args)>{},
                   std::forward_as_tuple(args...));
          });
        });
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }
};

/**
 * @brief node_binding::thread_safe_function<R(Args...)> <--
 * Napi::ThreadSafeFunction
 *
 * @tparam R
 * @tparam Args
 */
template <typename R, typename... Args>
class TypeConvertor<thread_safe_function<R(Args...)>> {
 public:
  static thread_safe_function<R(Args...)> ToNativeValue(
      const Napi::Value& value) {
    Napi::Function fn = value.As<Napi::Function>();
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        value.Env(), fn, "thread safe function resource", 0, 1);
    // 메인 스레드의 식별자를 저장합니다.
    //  - ToNativeValue 메서드는 메인 스레드에서 호출되는것을 전제 조건으로
    //  합니다.
    std::thread::id tid = std::this_thread::get_id();
    return thread_safe_function<R(Args...)>(
        tsfn, [tid, fn, tsfn](Args... args) -> R {
          // 메인 스레드가 호출했다면 Napi::Function을 직접 호출합니다.
          if (std::this_thread::get_id() == tid) {
            return Invoke<R>(fn, std::make_index_sequence<sizeof...(Args)>{},
                             std::forward_as_tuple(args...));
          }

          std::promise<R> p;
          napi_status status = tsfn.BlockingCall(
              [&p, args...](Napi::Env env, Napi::Function fn) {
                p.set_value(
                    Invoke<R>(fn, std::make_index_sequence<sizeof...(Args)>{},
                              std::forward_as_tuple(args...)));
              });
          if (status != napi_ok)
            return R();
          return p.get_future().get();
        });
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }
};
#endif

/**
 * @brief std::function<R(Args...)> <-> Napi::Function
 *
 * @tparam R
 * @tparam Args
 */
template <typename R, typename... Args>
class TypeConvertor<std::function<R(Args...)>> {
 public:
  static std::function<R(Args...)> ToNativeValue(const Napi::Value& value) {
    Napi::Function fn = value.As<Napi::Function>();
    // 메인 스레드의 식별자를 저장합니다.
    //  - ToNativeValue 메서드는 메인 스레드에서 호출되는것을 전제 조건으로
    //  합니다.
    std::thread::id tid = std::this_thread::get_id();
    return [tid, fn](Args... args) -> R {
      // 메인 스레드에서 호출되지 않았다면 예외를 발생시키도록 합니다.
      if (tid != std::this_thread::get_id()) {
#ifdef CXX_EXCEPTIONS
#if defined(CXX_RTTI) && (NAPI_VERSION > 3)
        throw std::runtime_error(
            std::string("invalid call - use ")
                .append(typeid(std::function<R(Args...)>).name())
                .append(" instead of ")
                .append(typeid(node_binding::thread_safe_function<R(Args...)>)
                            .name()));
#else
        throw std::runtime_error(
            "invalid call - use std::function<?> instead of "
            "node_binding::thread_safe_function<?>");
#endif
#endif
        return R();
      }
      return Invoke<R>(fn, std::make_index_sequence<sizeof...(Args)>{},
                       std::forward_as_tuple(args...));
    };
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const std::function<R(Args...)>& value) {
    Napi::Function func = Napi::Function::New(
        env, [value](const Napi::CallbackInfo& info) -> Napi::Value {
          return ::node_binding::TypedCall(info, value);
        });
    return func;
  }
};

/**
 * @brief std::function<void(Args...)> <-> Napi::Function
 *
 * @tparam Args
 */
template <typename... Args>
class TypeConvertor<std::function<void(Args...)>> {
 public:
  static std::function<void(Args...)> ToNativeValue(const Napi::Value& value) {
    Napi::Function f = value.As<Napi::Function>();
    // 메인 스레드의 식별자를 저장합니다.
    //  - ToNativeValue 메서드는 메인 스레드에서 호출되는것을 전제 조건으로
    //  합니다.
    std::thread::id tid = std::this_thread::get_id();
    return [tid, f](Args... args) {
      // 메인 스레드에서 호출되지 않았다면 예외를 발생시키도록 합니다.
      if (tid != std::this_thread::get_id()) {
#ifdef CXX_EXCEPTIONS
#if defined(CXX_RTTI) && (NAPI_VERSION > 3)
        throw std::runtime_error(
            std::string("invalid call - use ")
                .append(typeid(std::function<void(Args...)>).name())
                .append(" instead of ")
                .append(
                    typeid(node_binding::thread_safe_function<void(Args...)>)
                        .name()));
#else
        throw std::runtime_error(
            "invalid call - use std::function<?> instead of "
            "node_binding::thread_safe_function<?>");
#endif
#endif
        return;
      }
      Invoke(f, std::make_index_sequence<sizeof...(Args)>{},
             std::forward_as_tuple(args...));
    };
  }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env,
                               const std::function<void(Args...)>& value) {
    return Napi::Function::New(env, [value](const Napi::CallbackInfo& info) {
      ::node_binding::TypedCall(info, value);
    });
  }
};

/**
 * @brief Function pointer, Lambda function, std::function -->
 * node_binding::function
 *
 */
class function : public std::function<Napi::Value(const Napi::CallbackInfo&)> {
 private:
  using Ft = std::function<Napi::Value(const Napi::CallbackInfo&)>;
  using Ft::Ft;

  template <typename... Args>
  static function define_function_(void (*f)(Args...)) {
    return function([f](const Napi::CallbackInfo& info) -> Napi::Value {
      ::node_binding::TypedCall(info, f);
      return info.Env().Undefined();
    });
  }

  template <typename R, typename... Args>
  static function define_function_(R (*f)(Args...)) {
    return function([f](const Napi::CallbackInfo& info) -> Napi::Value {
      return ::node_binding::TypedCall(info, f);
    });
  }

  template <typename... Args>
  static function define_function_(std::function<void(Args...)> f) {
    return function([f](const Napi::CallbackInfo& info) -> Napi::Value {
      ::node_binding::TypedCall(info, f);
      return info.Env().Undefined();
    });
  }

  template <typename R, typename... Args>
  static function define_function_(std::function<R(Args...)> f) {
    return function([f](const Napi::CallbackInfo& info) -> Napi::Value {
      return ::node_binding::TypedCall(info, f);
    });
  }

 public:
#if CXX_VER >= 201703
  template <typename T>
  static function from(T lambda) {
    return from(std::function(lambda));
  }
#endif
  template <typename T>
  static function from(std::function<T> function) {
    return define_function_(function);
  }

  template <typename... Args>
  static function from(void (&f)(Args...)) {
    return define_function_(f);
  }

  template <typename R, typename... Args>
  static function from(R (&f)(Args...)) {
    return define_function_(f);
  }
};

/**
 * @brief node_binding::function <--> Napi::Function
 *
 */
template <>
class TypeConvertor<function> {
 public:
  static Napi::Value ToNativeValue(const Napi::Value& value) { return value; }

  static bool IsConvertible(const Napi::Value& value) {
    return value.IsFunction();
  }

  static Napi::Value ToJSValue(const Napi::Env& env, const function& value) {
    return Napi::Function::New(env, value);
  }
};

#if defined(_MSC_VER)
#if CXX_VER >= 201703
#define _NODE_BINDING_OBJECT
#endif
#else
#if CXX_VER >= 201703 && defined(CXX_RTTI)
#define _NODE_BINDING_OBJECT
#endif
#endif

#ifdef _NODE_BINDING_OBJECT
using object = std::unordered_map<std::string, std::any>;
template <typename T>
T any_cast(std::any& _Any) {
#if defined(CXX_EXCEPTIONS)
  try {
    Napi::Value value = std::any_cast<Napi::Value>(_Any);
    if (TypeConvertor<T>::IsConvertible(value)) return TypeConvertor<T>::ToNativeValue(value);
  } catch (...) {
    try {
      T value = std::any_cast<T>(_Any);
      return value;
    } catch (...) {
    }
  }
#elif defined(CXX_RTTI) || defined(_MSC_VER)
  if (typeid(Napi::Value) == _Any.type()) {
    Napi::Value value = std::any_cast<Napi::Value>(_Any);
    if (TypeConvertor<T>::IsConvertible(value)) return TypeConvertor<T>::ToNativeValue(value);
  } else if (typeid(T) == _Any.type()) {
    return std::any_cast<T>(_Any);
  }
#endif
  return T();
}

/**
 * @brief std::unordered_map<std::string, std::any> <-> Napi::Object
 *
 */
template <>
class TypeConvertor<object> {
 public:
#define INSERT_NATIVE_ARRAY(_object_, _type_, _name_, _value_)           \
  if (TypeConvertor<std::vector<_type_>>::IsConvertible((_value_))) {    \
    (_object_).insert(                                                   \
        {(_name_).c_str(),                                               \
         TypeConvertor<std::vector<_type_>>::ToNativeValue((_value_))}); \
    continue;                                                            \
  }
#define INSERT_NATIVE_VALUE(_object_, _type_, _name_, _value_)                \
  if (TypeConvertor<_type_>::IsConvertible((value))) {                        \
    (_object_).insert(                                                        \
        {(_name_).c_str(), TypeConvertor<_type_>::ToNativeValue((_value_))}); \
    continue;                                                                 \
  }
  static object ToNativeValue(const Napi::Value& value) {
    object ret;
    const Napi::Object object = value.As<Napi::Object>();
    for (std::string& name :
         TypeConvertor<std::vector<std::string>>::ToNativeValue(
             object.GetPropertyNames())) {
      const Napi::Value& value = object.Get(name);
      if (value.IsArray()) {
        INSERT_NATIVE_ARRAY(ret, std::string, name, value);
        INSERT_NATIVE_ARRAY(ret, bool, name, value);
        INSERT_NATIVE_ARRAY(ret, short, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned short, name, value);
        INSERT_NATIVE_ARRAY(ret, int, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned int, name, value);
        INSERT_NATIVE_ARRAY(ret, long, name, value);
        INSERT_NATIVE_ARRAY(ret, unsigned long, name, value);
        INSERT_NATIVE_ARRAY(ret, int32_t, name, value);
        INSERT_NATIVE_ARRAY(ret, uint32_t, name, value);
        INSERT_NATIVE_ARRAY(ret, int64_t, name, value);
        INSERT_NATIVE_ARRAY(ret, uint64_t, name, value);
        INSERT_NATIVE_ARRAY(ret, float, name, value);
        INSERT_NATIVE_ARRAY(ret, double, name, value);
        INSERT_NATIVE_ARRAY(ret, node_binding::function, name, value);
        INSERT_NATIVE_ARRAY(ret, node_binding::object, name, value);
      } else if (value.IsFunction()) {
        INSERT_NATIVE_VALUE(ret, node_binding::function, name, value);
        continue;
      } else if (value.IsObject()) {
        INSERT_NATIVE_VALUE(ret, node_binding::object, name, value);
      } else if (value.IsUndefined()) {
        ret.insert({name.c_str(), std::any()});
        continue;
      } else if (value.IsNull()) {
        ret.insert({name.c_str(), nullptr});
        continue;
      } else {
        INSERT_NATIVE_VALUE(ret, std::string, name, value);
        INSERT_NATIVE_VALUE(ret, bool, name, value);
        INSERT_NATIVE_VALUE(ret, short, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned short, name, value);
        INSERT_NATIVE_VALUE(ret, int, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned int, name, value);
        INSERT_NATIVE_VALUE(ret, long, name, value);
        INSERT_NATIVE_VALUE(ret, unsigned long, name, value);
        INSERT_NATIVE_VALUE(ret, int32_t, name, value);
        INSERT_NATIVE_VALUE(ret, uint32_t, name, value);
        INSERT_NATIVE_VALUE(ret, int64_t, name, value);
        INSERT_NATIVE_VALUE(ret, uint64_t, name, value);
        INSERT_NATIVE_VALUE(ret, float, name, value);
        INSERT_NATIVE_VALUE(ret, double, name, value);
        INSERT_NATIVE_VALUE(ret, node_binding::function, name, value);
      }
      ret.insert({name.c_str(), std::any()});
    }
    return ret;
  }

  static bool IsConvertible(const Napi::Value& value) {
    return ((!value.IsFunction()) && value.IsObject());
  }

#define INSERT_JS_VALUE(_env_, _object_, _type_, _member_)                   \
  if (typeid(_type_) == (_member_).second.type()) {                          \
    (_object_)[(_member_).first.c_str()] = TypeConvertor<_type_>::ToJSValue( \
        (_env_), std::any_cast<_type_>((_member_).second));                  \
    continue;                                                                \
  }
#define INSERT_JS_STR_VALUE(_env_, _object_, _type_, _member_)  \
  if (typeid(_type_) == (_member_).second.type()) {             \
    (_object_)[(_member_).first.c_str()] =                      \
        TypeConvertor<std::string>::ToJSValue(                  \
            (_env_), std::any_cast<_type_>((_member_).second)); \
    continue;                                                   \
  }
#define INSERT_JS_ARRAY(_env_, _object_, _type_, _member_)                   \
  if (typeid(std::vector<_type_>) == (_member_).second.type()) {             \
    (_object_)[(_member_).first.c_str()] =                                   \
        TypeConvertor<std::vector<_type_>>::ToJSValue(                       \
            (_env_), std::any_cast<std::vector<_type_>>((_member_).second)); \
    continue;                                                                \
  }
  static Napi::Value ToJSValue(const Napi::Env& env, const object& value) {
    Napi::Object ret = Napi::Object::New(env);

    for (auto member : value) {
      INSERT_JS_STR_VALUE(env, ret, char*, member);
      INSERT_JS_STR_VALUE(env, ret, const char*, member);
      INSERT_JS_VALUE(env, ret, std::string, member);
      INSERT_JS_VALUE(env, ret, bool, member);
      INSERT_JS_VALUE(env, ret, short, member);
      INSERT_JS_VALUE(env, ret, unsigned short, member);
      INSERT_JS_VALUE(env, ret, int, member);
      INSERT_JS_VALUE(env, ret, unsigned int, member);
      INSERT_JS_VALUE(env, ret, long, member);
      INSERT_JS_VALUE(env, ret, unsigned long, member);
      INSERT_JS_VALUE(env, ret, int32_t, member);
      INSERT_JS_VALUE(env, ret, uint32_t, member);
      INSERT_JS_VALUE(env, ret, int64_t, member);
      INSERT_JS_VALUE(env, ret, uint64_t, member);
      INSERT_JS_VALUE(env, ret, float, member);
      INSERT_JS_VALUE(env, ret, double, member);
      INSERT_JS_VALUE(env, ret, node_binding::function, member);
      INSERT_JS_VALUE(env, ret, node_binding::object, member);

      INSERT_JS_ARRAY(env, ret, std::string, member);
      INSERT_JS_ARRAY(env, ret, bool, member);
      INSERT_JS_ARRAY(env, ret, short, member);
      INSERT_JS_ARRAY(env, ret, unsigned short, member);
      INSERT_JS_ARRAY(env, ret, int, member);
      INSERT_JS_ARRAY(env, ret, unsigned int, member);
      INSERT_JS_ARRAY(env, ret, long, member);
      INSERT_JS_ARRAY(env, ret, unsigned long, member);
      INSERT_JS_ARRAY(env, ret, int32_t, member);
      INSERT_JS_ARRAY(env, ret, uint32_t, member);
      INSERT_JS_ARRAY(env, ret, int64_t, member);
      INSERT_JS_ARRAY(env, ret, uint64_t, member);
      INSERT_JS_ARRAY(env, ret, float, member);
      INSERT_JS_ARRAY(env, ret, double, member);
      INSERT_JS_ARRAY(env, ret, node_binding::function, member);
      INSERT_JS_ARRAY(env, ret, node_binding::object, member);

      ret[member.first.c_str()] = env.Undefined();
    }
    return ret;
  }
};
#endif  // _NODE_BINDING_OBJECT
}  // namespace node_binding

#endif  // NODE_BINDING_STL_H_