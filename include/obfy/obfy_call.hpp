#ifndef __OBFY_CALL_HPP__
#define __OBFY_CALL_HPP__

#include <obfy/obfy.hpp>

#ifdef OBFY_ENABLE_FSM_CALL
#include <cstdint>
#include <type_traits>
#include <utility>
#endif

#if defined(_MSC_VER)
#  define OBFY_NOINLINE __declspec(noinline)
#else
#  define OBFY_NOINLINE __attribute__((noinline))
#endif

namespace obfy {

#ifdef OBFY_ENABLE_FSM_CALL

#ifndef OBFY_CALL_STEPS
#define OBFY_CALL_STEPS 32u
#endif

namespace detail {
    inline uint32_t lcg(uint32_t x) { return x * 1664525u + 1013904223u; }
}

template<class F>
struct obf_addr {
    uintptr_t v;
    uintptr_t k;
    explicit constexpr obf_addr(F f, uintptr_t salt)
        : v(reinterpret_cast<uintptr_t>(f) ^ salt), k(salt) {}
    inline F get() const { return reinterpret_cast<F>(v ^ k); }
};

template<typename R, typename F, typename... Args>
OBFY_NOINLINE typename std::enable_if<!std::is_void<R>::value, R>::type
obfy_call_impl(F f, Args&&... args) {
    const uintptr_t salt = detail::mix64(OBFY_LOCAL_KEY() ^ detail::runtime_tweak_seed());
    obf_addr<F> oa(f, salt);
    volatile uint32_t s = static_cast<uint32_t>(salt);
    const uint32_t fire = s % OBFY_CALL_STEPS;
    R ret{};
    for (uint32_t i = 0; i < OBFY_CALL_STEPS; ++i) {
        s = detail::lcg(s) ^ static_cast<uint32_t>(i * 2654435761u);
        if (i == fire) {
            F g = oa.get();
            ret = g(std::forward<Args>(args)...);
        } else {
            s ^= (s >> 13);
        }
    }
    return ret;
}

template<typename R, typename F, typename... Args>
OBFY_NOINLINE typename std::enable_if<std::is_void<R>::value, void>::type
obfy_call_impl(F f, Args&&... args) {
    const uintptr_t salt = detail::mix64(OBFY_LOCAL_KEY() ^ detail::runtime_tweak_seed());
    obf_addr<F> oa(f, salt);
    volatile uint32_t s = static_cast<uint32_t>(salt);
    const uint32_t fire = s % OBFY_CALL_STEPS;
    for (uint32_t i = 0; i < OBFY_CALL_STEPS; ++i) {
        s = detail::lcg(s) ^ static_cast<uint32_t>(i * 2654435761u);
        if (i == fire) {
            F g = oa.get();
            g(std::forward<Args>(args)...);
        } else {
            s ^= (s >> 13);
        }
    }
}

#define OBFY_CALL_RET(R, f, ...) (::obfy::obfy_call_impl<R>(f, __VA_ARGS__))
#define OBFY_CALL(f, ...) (::obfy::obfy_call_impl<void>(f, __VA_ARGS__))

#else  // OBFY_ENABLE_FSM_CALL

#define OBFY_CALL_RET(R, f, ...) (f(__VA_ARGS__))
#define OBFY_CALL(f, ...) (f(__VA_ARGS__))

#endif // OBFY_ENABLE_FSM_CALL

} // namespace obfy

#endif // __OBFY_CALL_HPP__
