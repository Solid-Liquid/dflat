#pragma once

#include "outcome/outcome.hpp"
#include "vector.hpp"

#define TRY OUTCOME_TRY
#define TRY_ OUTCOME_TRYV

namespace dflat {

namespace outcome = OUTCOME_V2_NAMESPACE;

struct Fail {};
Fail const failure;

struct Void {};

using outcome::success;

template <typename T, typename E>
using Result = outcome::checked<T,E>;

template <typename E>
using Result_ = outcome::checked<void, E>;

template <typename T>
using Optional = outcome::checked<T,Fail>;

//using Fallible = outcome::checked<void,Fail>;

//template <typename T>
//Optional<T*> to_optional(T* t) {
//    if (t) {
//        return outcome::success(t);
//    }
//    else {
//        return Fail();
//    }
//}

template <typename T, typename E>
T& operator*(outcome::checked<T,E>& o) {
    return o.value();
}

template <typename T, typename E>
T const& operator*(outcome::checked<T, E> const& o) {
    return o.value();
}

//struct Require {};
//
//#define REQUIRE Require{} %
//
//template <typename T>
//T operator % (Require, T t) {
//    if (!t) {
//        throw std::logic_error("Failed result");
//    }
//    else {
//        return t;
//    }
//}

//template <typename F>
//auto try_until_success(F&& f) {
//    do {
//        auto&& r = f();
//        
//        if (r) {
//            return r;
//        }
//    } while (true);
//}

template <typename T, typename E>
Vector<T> result_to_vector(Result<T,E> const& r) {
    if (r) {
        return {r.value()};
    }
    else {
        return {};
    }
}

//TODO my extension macros aren't as safe as OUTCOME_TRY. I guess.
#define LOOP_TRY(a,b) \
    auto a##__ = b; \
    if (!a##__) { continue; } \
    auto&& a = a##__.value()

/*
#define BOOL_TRY(a,b) \
    auto a##__ = b; \
    if (!a##__) { return false; } \
    auto&& a = a##__.value()

#define POINTER_TRY(a,b) \
    auto a##__ = b; \
    if (!a##__) { return result::fail; } \
    auto&& a = *a##__

#define FAIL_IF(cond) \
    if (cond) { return result::fail; }

#define FALSE_IF(cond) \
    if (cond) { return false; }

#define CONTINUE_IF(cond) \
    if (cond) { continue; }
*/
} //dflat
