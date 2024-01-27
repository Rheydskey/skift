#pragma once

#include <karm-meta/cvrp.h>

#include "bool.h"
#include "std.h"

#if defined(__ck_debug__) && !defined(KARM_DISABLE_TRY_FAIL_HOOK)
// Give us a symbole to break one when debbuging error
// handling.
extern "C" void __try_failled();
#    define __tryFail() __try_failled()
#else
#    define __tryFail() /* NOP */
#endif

#define try$(EXPR)                           \
    ({                                       \
        auto __expr = (EXPR);                \
        if (not static_cast<bool>(__expr)) { \
            __tryFail();                     \
            return __expr.none();            \
        }                                    \
        __expr.take();                       \
    })

#define co_try$(EXPR)                        \
    ({                                       \
        auto __expr = (EXPR);                \
        if (not static_cast<bool>(__expr)) { \
            __tryFail();                     \
            co_return __expr.none();         \
        }                                    \
        __expr.take();                       \
    })

#define co_try_await$(EXPR)                  \
    ({                                       \
        auto __expr = co_await (EXPR);       \
        if (not static_cast<bool>(__expr)) { \
            __tryFail();                     \
            co_return __expr.none();         \
        }                                    \
        __expr.take();                       \
    })

namespace Karm {

template <typename T>
concept Takeable = requires(T t) {
    { t.take() };
};

template <typename T>
using TakeableType = decltype(Meta::declval<T>().take());

template <typename T>
concept Unwrapable = requires(T t) {
    { t.unwrap() };
};

template <typename T>
using UnwrapableType = Meta::RemoveRef<decltype(Meta::declval<T>().unwrap())>;

template <typename T>
concept Tryable = requires(T t) {
    { not static_cast<bool>(t) };
    { t.none() };
} and Unwrapable<T> and Takeable<T>;

auto tryOr(Tryable auto opt, Meta::RemoveRef<decltype(opt.unwrap())> defaultValue) -> Meta::RemoveRef<decltype(opt.unwrap())> {
    if (not opt) {
        return defaultValue;
    }

    return opt.unwrap();
}

auto tryOrElse(Tryable auto opt, auto defaultValue) -> Meta::RemoveRef<decltype(opt.unwrap())> {
    if (not opt) {
        return defaultValue();
    }

    return opt.unwrap();
}

} // namespace Karm
