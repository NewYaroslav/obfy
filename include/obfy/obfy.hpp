/*
 * Copyright (c) 2016, Ferenc Deak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **/
#pragma once
#ifndef __OBFY_HPP__
#define __OBFY_HPP__

#include <string>
#include <typeinfo>

#include <memory>
#include <random>
#include <vector>
#include <type_traits>
#include <utility>
#include <stdint.h>
#include <ctime>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef OBFY_MAX_BOGUS_IMPLEMENTATIONS
#define OBFY_MAX_BOGUS_IMPLEMENTATIONS 3
#endif

namespace obfy
{
// Compile time random number generator from https://github.com/andrivet/ADVobfuscator
// Modified to allow overriding the time-based seed via `OBFY_SEED` for reproducible builds.
/*
 * Written by Sebastien Andrivet
 * Copyright (c) 2010-2015 - Sebastien Andrivet All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

namespace detail {
    constexpr int DigitToInt(char c) { return c - '0'; }

    template <size_t N>
    constexpr int parse_time(const char (&t)[N]) {
        return (DigitToInt(t[0]) * 10 + DigitToInt(t[1])) * 3600 +
               (DigitToInt(t[3]) * 10 + DigitToInt(t[4])) * 60 +
               (DigitToInt(t[6]) * 10 + DigitToInt(t[7]));
    }

    static_assert(parse_time("12:34:56") == 45296, "time parser failed");

    constexpr int time_seed() { return parse_time(__TIME__); }

#if __cplusplus >= 201402L
    constexpr uint64_t mix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ull;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
        return x ^ (x >> 31);
    }
#else
    constexpr uint64_t mix64_step1(uint64_t x) {
        return x + 0x9e3779b97f4a7c15ull;
    }

    constexpr uint64_t mix64_step2(uint64_t x) {
        return (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    }

    constexpr uint64_t mix64_step3(uint64_t x) {
        return (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    }

    constexpr uint64_t mix64_step4(uint64_t x) {
        return x ^ (x >> 31);
    }

    constexpr uint64_t mix64(uint64_t x) {
        return mix64_step4(mix64_step3(mix64_step2(mix64_step1(x))));
    }
#endif

#ifndef OBFY_DISABLE_RUNTIME_TWEAK
    inline uint64_t runtime_tweak() {
        int _obfy_dummy = 0;
#if defined(_WIN32)
        uint64_t pid = static_cast<uint64_t>(GetCurrentProcessId());
#else
        uint64_t pid = static_cast<uint64_t>(getpid());
#endif
        uint64_t addr = reinterpret_cast<uint64_t>(&_obfy_dummy);
        uint64_t t = static_cast<uint64_t>(std::time(nullptr));
        return mix64(pid ^ addr ^ t);
    }
#if defined(_MSC_VER)
    static __declspec(noinline) uint64_t runtime_tweak_seed() {
#else
    static __attribute__((noinline)) uint64_t runtime_tweak_seed() {
#endif
        static uint64_t seed = runtime_tweak();
        return seed;
    }
#else
    inline uint64_t runtime_tweak() { return 0; }
#if defined(_MSC_VER)
    static __declspec(noinline) uint64_t runtime_tweak_seed() { return 0; }
#else
    static __attribute__((noinline)) uint64_t runtime_tweak_seed() { return 0; }
#endif
#endif

    constexpr uint64_t fnv1a64(const char* s, uint64_t h = 1469598103934665603ull) {
        return *s ? fnv1a64(s + 1, (h ^ static_cast<unsigned char>(*s)) * 1099511628211ull) : h;
    }

#ifndef OBFY_FILE_FOR_HASH
#  define OBFY_FILE_FOR_HASH __FILE__
#endif

    constexpr uint64_t OBFY_TU_SALT_CT = mix64(fnv1a64(OBFY_FILE_FOR_HASH));
}

#ifndef OBFY_SEED
#  define OBFY_SEED (::obfy::detail::time_seed())
#endif

#ifndef OBFY_TU_SALT
// Translation unit specific salt for key diversification
#  define OBFY_TU_SALT (::obfy::detail::OBFY_TU_SALT_CT)
#endif

#ifndef OBFY_LOCAL_KEY
// Local key combining global seed, TU salt and location macros
#  define OBFY_LOCAL_KEY() (::obfy::detail::mix64(static_cast<uint64_t>(OBFY_SEED) ^ static_cast<uint64_t>(OBFY_TU_SALT) ^ static_cast<uint64_t>(__LINE__) ^ static_cast<uint64_t>(__COUNTER__)))
#endif

static constexpr unsigned seed = static_cast<unsigned>(OBFY_SEED);

template<class T>
struct obfy_is_intish : std::integral_constant<bool,
    std::is_integral<T>::value || std::is_enum<T>::value> {};

template<int N>
struct MetaRandomGenerator final
{
private:
    static constexpr unsigned a = 16807;
    static constexpr unsigned m = 2147483647;
    static constexpr unsigned s = MetaRandomGenerator<N - 1>::value;
    static constexpr unsigned lo = a * (s & 0xFFFF);
    static constexpr unsigned hi = a * (s >> 16);
    static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);
    static constexpr unsigned hi2 = hi >> 15;
    static constexpr unsigned lo3 = lo2 + hi;

public:
    static constexpr unsigned max = m;
    static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template<>
struct MetaRandomGenerator<0> final
{
    static constexpr unsigned value = seed;
};

template<int N, int M>
struct MetaRandom final
{
    static const int value = MetaRandomGenerator<N + 1>::value % M;
};

#define OBFY_COMP_ASSIGNMENT_OPERATOR(x) \
    refholder<T>& operator x##= (const refholder<T>& ov) { v x##= ov.v; return *this;}  \
    refholder<T>& operator x##= (const refholder<T>&& ov) { v x##= ov.v; return *this;} \
    refholder<T>& operator x##= (const T& ov) { v x##= ov; return *this;}               \
    refholder<T>& operator x##= (const T&& ov) { v x##= ov; return *this;}              \
    refholder<T>& operator x##= (T& ov) { v x##= ov; return *this;}


#define OBFY_COMPARISON_OPERATOR(x) \
    bool operator x (const T& ov) { return (v x ov); }

/* simple reference holder class, mostly for dealing with holding variables */
template <typename T>
class refholder final
{
public:
    /* Construction, destruction */
    refholder() = delete;
    refholder(T& pv) : v(pv) {}
    refholder(const refholder& ov) : v(ov.v) {}
    refholder(T&&) = delete;

    ~refholder() = default;

    /* Assignment */
    refholder<T>& operator = (const T& ov) { v = ov; return *this;}
    refholder<T>& operator = (const refholder<T>& ov ) { v = ov.v; return *this; }

    /* Comparison */
    bool operator==(const T& ov) const {
        static_assert(!std::is_floating_point<T>::value,
                      "refholder does not support floating point types");
        T tmp = v;
        typedef std::integral_constant<bool,
            std::is_integral<T>::value || std::is_enum<T>::value> is_int;
        return eq_impl(tmp, ov, is_int{});
    }
    bool operator!=(const T& ov) const { return !(*this == ov); }
    OBFY_COMPARISON_OPERATOR(>=)
    OBFY_COMPARISON_OPERATOR(<=)
    OBFY_COMPARISON_OPERATOR(>)
    OBFY_COMPARISON_OPERATOR(<)

    /* Conversion to the real type */
    operator T() {return v;}

    /* Pre increment/decrement operators */
    refholder<T>& operator++() { ++ v; return *this; }
    refholder<T>& operator--() { -- v; return *this; }

    /* post increment/decrement */
    refholder<T> operator++(int) { refholder<T> rv(*this); operator ++(); return rv; }
    refholder<T> operator--(int) { refholder<T> rv(*this); operator --(); return rv; }

    /* Compound assignments */
    OBFY_COMP_ASSIGNMENT_OPERATOR(+)
    OBFY_COMP_ASSIGNMENT_OPERATOR(-)
    OBFY_COMP_ASSIGNMENT_OPERATOR(*)
    OBFY_COMP_ASSIGNMENT_OPERATOR(/)
    OBFY_COMP_ASSIGNMENT_OPERATOR(%)
    OBFY_COMP_ASSIGNMENT_OPERATOR(<<)
    OBFY_COMP_ASSIGNMENT_OPERATOR(>>)
    OBFY_COMP_ASSIGNMENT_OPERATOR(&)
    OBFY_COMP_ASSIGNMENT_OPERATOR(|)
    OBFY_COMP_ASSIGNMENT_OPERATOR(^)

private:

    static bool eq_impl(T a, T b, std::true_type) { return (a ^ b) == 0; } // integral/enum
    static bool eq_impl(T a, T b, std::false_type) { return a == b; }      // others

    /* The root of all evil */
    volatile T& v;
};

/* simple reference holder class, for holding reference of constant variables */
template <typename T>
class refholder <const T> final
{
public:
    /* Construction, destruction */
    refholder() = delete;
    refholder(const T& pv) : v(pv) {}
    refholder(T&&) = delete;

    ~refholder() = default;

    /* Conversion to the real type */
    operator const T() {return v;}

private:

    /* The root of all evil */
    const T& v;
};

/* Binary operators for the value wrappers */
#define OBFY_DEFINE_BINARY_OPERATOR(x) \
template <class T> refholder<T> operator x (refholder<T>& ls, const T& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, T& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, T&& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, const T&& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, const refholder<T>&& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, refholder<T>&& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, refholder<T>& rs) {refholder<T> rv = ls; ls x##= rs; return rv; } \
template <class T> refholder<T> operator x (refholder<T>& ls, const refholder<T>& rs) {refholder<T> rv = ls; ls x##= rs; return rv; }

OBFY_DEFINE_BINARY_OPERATOR(+)
OBFY_DEFINE_BINARY_OPERATOR(-)
OBFY_DEFINE_BINARY_OPERATOR(*)
OBFY_DEFINE_BINARY_OPERATOR(/)
OBFY_DEFINE_BINARY_OPERATOR(%)
OBFY_DEFINE_BINARY_OPERATOR(&)
OBFY_DEFINE_BINARY_OPERATOR(|)
OBFY_DEFINE_BINARY_OPERATOR(<<)
OBFY_DEFINE_BINARY_OPERATOR(>>)
OBFY_DEFINE_BINARY_OPERATOR(^)


/* Helping stuff */

struct base_rvholder
{
    virtual ~base_rvholder() = default;

    template<class T>
    operator T () const
    {
        return *reinterpret_cast<const T*>(get());
    }

    template<class T>
    bool operator == (const T& o) const
    {
        return o == operator T ();
    }

    template<class T>
    bool equals(const T& o) const
    {
        return o == *reinterpret_cast<const T*>(get());
    }

    virtual const void* get() const = 0;
};

template<class T>
class rvholder : public base_rvholder
{
public:
    rvholder(T t, T c) :base_rvholder(), v(t), check(c) {}
    ~rvholder() = default;
    virtual const void* get() const override {return reinterpret_cast<const void*>(&v);}
private:
    T v;
    T check;
};


/* what the RETURN/BREAK/CONTINUE will return while running from inside a loop block*/
enum class next_step
{
    ns_break,
    ns_continue,
    ns_return,
    ns_done,
    ns_unused
};

struct next_step_functor_base
{
    virtual ~next_step_functor_base() = default;
    virtual next_step run() = 0;
};

template <class T>
struct next_step_functor final : public next_step_functor_base
{
    next_step_functor(T r) : runner(r) {}
    virtual next_step run() {return runner();}

private:
    T runner;
};

struct bool_functor_base
{
    virtual ~bool_functor_base() = default;
    virtual bool run() = 0;
};

template <class T>
struct bool_functor final : public bool_functor_base
{
    bool_functor(T r) : runner(r) {}
    virtual bool run() {return runner();}

private:
    T runner;
};

struct any_functor_base
{
    virtual ~any_functor_base() = default;
    virtual void run(void*) const = 0;
};

template <class T>
struct any_functor final : public any_functor_base
{
    any_functor(T r) : runner(r) {}

    virtual void run(void* retv) const override
    {
        auto r = runner();
        *reinterpret_cast<decltype(r)*>(retv) = r;
    }


private:
    T runner;

};

/* c++ control structures implementation */

/* supporting implementation for the REPEAT/AS_LONG_AS macros*/

class repeat_wrapper final
{
public:
    repeat_wrapper():body(nullptr), condition(nullptr) {}

    void run()
    {
        do
        {
            try
            {
                body->run();
            }
            catch(next_step& c)
            {
                if(c == next_step::ns_break) break;
                if(c == next_step::ns_continue) continue;
            }
        }
        while( condition->run() );
    }

    template<class T>
    repeat_wrapper& set_body(T lambda) { body.reset(new next_step_functor<T>(lambda)); return *this; }


    template<class T>
    repeat_wrapper& set_condition(T lambda) { condition.reset(new bool_functor<T>(lambda)); return *this; }

private:

    std::unique_ptr<next_step_functor_base> body;
    std::unique_ptr<bool_functor_base> condition;
};

/* supporting implementation for the FOR macro */

class for_wrapper final
{
public:

    template<class INIT, class COND, class INCR>
    explicit for_wrapper(INIT lambda_init, COND lambda_cond, INCR lambda_incr)
    {
        condition.reset(new bool_functor<COND>(lambda_cond));
        initializer.reset(new next_step_functor<INIT>(lambda_init));
        increment.reset(new next_step_functor<INCR>(lambda_incr));
    }

    void run()
    {
        for( initializer->run(); condition->run(); increment->run())
        {
            try
            {
                body->run();
            }
            catch(next_step& c)
            {
                if(c == next_step::ns_break) break;
                if(c == next_step::ns_continue) continue;
            }
        }
    }

    ~for_wrapper() noexcept = default;

    template<class T>
    for_wrapper& set_body(T lambda) { body.reset(new next_step_functor<T>(lambda)); return *this; }
private:
    std::unique_ptr<next_step_functor_base> initializer;
    std::unique_ptr<bool_functor_base> condition;
    std::unique_ptr<next_step_functor_base> increment;
    std::unique_ptr<next_step_functor_base> body;

};

/* supporting implementation for the WHILE macro */

class while_wrapper final
{
public:
    template<class T>
    explicit while_wrapper(T lambda) :body(nullptr), condition(nullptr)
    {condition.reset(new bool_functor<T>(lambda));}

    void run()
    {
        while( condition->run() )
        {
            try
            {
                body->run();
            }
            catch(next_step& c)
            {
                if(c == next_step::ns_break) break;
                if(c == next_step::ns_continue) continue;
            }
        }
    }

    ~while_wrapper() noexcept = default;

    template<class T>
    while_wrapper& set_body(T lambda) { body.reset(new next_step_functor<T>(lambda)); return *this; }

private:
    std::unique_ptr<next_step_functor_base> body;
    std::unique_ptr<bool_functor_base> condition;
};

/* supporting implementation for the IF macro */

class if_wrapper final
{
public:
    template<class T>
    if_wrapper(T lambda) {condition.reset(new bool_functor<T>(lambda));}

    void run()
    {
        if(condition->run()) { if(thens) {
            thens->run();
        }}
        else { if(elses) {
            elses->run();
        }}

    }

    ~if_wrapper() noexcept = default;

    template<class T>
    if_wrapper& set_then(T lambda) { thens.reset(new next_step_functor<T>(lambda)); return *this; }
    template<class T>
    if_wrapper& set_else(T lambda) { elses.reset(new next_step_functor<T>(lambda)); return *this; }

private:
    std::unique_ptr<bool_functor_base> condition;
    std::unique_ptr<next_step_functor_base> thens;
    std::unique_ptr<next_step_functor_base> elses;
};

/* select ... case helper */

class case_instruction
{
public:
    case_instruction() = default;
    virtual ~case_instruction() = default;
    virtual next_step execute(const base_rvholder&) const = 0;
};

template<class CT>
class branch final : public case_instruction
{
public:
    template<class T>
    branch(T lambda) {condition.reset(new any_functor<T>(lambda));}

    bool equals(const base_rvholder& rv, CT lv) const
    {
        return rv.equals(lv);
    }

    virtual next_step execute(const base_rvholder& against) const override
    {
        CT retv;
        condition->run( const_cast<void*>(reinterpret_cast<const void*>(&retv) ) );
        // this looks funny, however we cannot use the operator ==
        // due to a visual C++ 2015 compiler bug, which fails to compile and crashes
        return equals(against,retv) ? next_step::ns_done : next_step::ns_continue;
    }

private:
    std::unique_ptr<any_functor_base> condition;
};

class body final : public case_instruction
{
public:
    template<class T>
    body(T lambda) {instructions.reset(new next_step_functor<T>(lambda));}


    virtual next_step execute(const base_rvholder&) const override
    {
        return instructions->run();
    }
private:
    std::unique_ptr<next_step_functor_base> instructions;
};

template <class CT>
class case_wrapper_base
{
public:
    explicit case_wrapper_base(const CT& v) : check(v), default_step(nullptr) {}


    case_wrapper_base& add_entry(const case_instruction& lambda_holder)
    {
        steps.push_back(&lambda_holder);
        return *this;
    }

    case_wrapper_base& add_default(const case_instruction& lambda_holder)
    {
        default_step = &lambda_holder;
        return *this;
    }

    case_wrapper_base& join()
    {
        return *this;
    }

    void run() const
    {
        auto it = steps.begin();
        while(it != steps.end())
        {
            bool increased = false;
            // see if this is a branch
            if(dynamic_cast<const branch<CT>*>(*it) || dynamic_cast<const branch<const CT>*>(*it))
            {
                // branch. Execute it, see if it returns true or false
                next_step enter = (*it)->execute(rvholder<CT>(check,check));
                if(enter == next_step::ns_continue)
                {
                    // step to the next branch
                    ++it;
                    increased = true;
                }
                else
                {
                    // now fast forward and find the first body,
                    // and from that point on run all the bodies, unless one of them breaks
                    while(! dynamic_cast<const body*>(*it)  && it != steps.end() )
                    {
                        ++it;
                        increased = true;
                    }

                    // found the first body.
                    while(it != steps.end())
                    {
                        if(dynamic_cast<const body*>(*it))
                        {
                            (*it)->execute(rvholder<CT>(check,check));
                        }
                        increased = true;
                        ++it;
                    }
                }
            }
            else if(dynamic_cast<const body*>(*it))
            {
                // skip body
            }
            else
            {
                throw(std::string("invalid type:") + typeid(*it).name());
            }

            // just for safety
            if(!increased)
            {
                ++it;
            }
        }

        if(default_step)
        {
            default_step->execute(rvholder<CT>(check,check));
        }
    }

private:

    std::vector<const case_instruction*> steps;
    const CT check;
    const case_instruction* default_step;
};

template<class CT>
class case_wrapper final : public case_wrapper_base<CT>
{
public:
    explicit case_wrapper(const CT& v) : case_wrapper_base<CT>(v) {}
};


template<class CT>
class case_wrapper <const CT> final : public case_wrapper_base<CT>
{
public:
    explicit case_wrapper(const CT& v) : case_wrapper_base<CT>(v) {}
};

/* syntactic sugar */

class stream_helper {};
template <typename T>
refholder<T> operator << (stream_helper, T& a)
{
    return refholder<T>(a);
}

/* The bogus operations allowed */
class basic_extra
{
public:
    basic_extra() = default;
    ~basic_extra() = default;
    basic_extra(const basic_extra&) = delete;
    basic_extra& operator = (const basic_extra&) = delete;
    basic_extra(const basic_extra&&) = delete;
    basic_extra& operator = (const basic_extra&&) = delete;
};

template <class T>
class extra_xor final : public basic_extra
{
public:
    extra_xor(T& a) : v(a)
    {
        volatile T lv = static_cast<T>(MetaRandom<__COUNTER__, 4096>::value);
        v ^= lv;
    }
    virtual ~extra_xor()
    {
        volatile T lv = static_cast<T>(MetaRandom<__COUNTER__ - 1, 4096>::value);
        v ^= lv;
    }

private:
    volatile T& v;
};

template <class T>
class extra_xor <const T> final : public basic_extra
{
public:
    extra_xor(const T&) {}
};

template <class T>
class extra_addition final : public basic_extra
{
public:
    extra_addition(T& a) : v(a)
    {
        volatile T lv = static_cast<T>(MetaRandom<__COUNTER__, 4096>::value);
        v += lv;
    }
    virtual ~extra_addition()
    {
        volatile T lv = static_cast<T>(MetaRandom<__COUNTER__ - 1, 4096>::value);
        v -= lv;
    }
private:
    volatile T& v;
};

template <class T>
class extra_addition <const T> final: public basic_extra
{
public:
    extra_addition(const T&) {}
};

template <class T>
class extra_substraction final : public basic_extra
{
public:
    extra_substraction(T& a) : v(a) {v -= MetaRandom<__COUNTER__, 4096>::value; }
    virtual ~extra_substraction() { v += MetaRandom<__COUNTER__ - 1, 4096>::value; }

private:
    volatile T& v;
};

template <class T>
class extra_substraction <const T> final : public basic_extra
{
public:
    extra_substraction(const T&) {}
};

template <typename T, int N>
class extra_chooser final
{
    using type=basic_extra; // intentionally private
};

/* Constant obfuscation implementer */

template<typename T, T n> class Num final
{
public:
    static constexpr T value =
        ((n & 0x01) | (Num<T, (n >> 1)>::value << 1));
    Num() : v(0)
    {
        v = value ^ MetaRandom<32, 4096>::value;
    }
    T get() const { T x = v ^ MetaRandom<32, 4096>::value; return x; }
private:
    volatile T v;
};

struct ObfZero { enum {value = 0}; };
struct ObfOne { enum {value = 1}; };
#define OBFY_ZERO(t) template <> struct Num<t,0> final : public ObfZero { t v = value; t get() const { return v;}; };
#define OBFY_ONE(t) template <> struct Num<t,1> final : public ObfOne { t v = value; t get() const { return v;}; };
#define OBFY_TYPE(t) OBFY_ZERO(t) OBFY_ONE(t)

OBFY_TYPE(bool)

OBFY_TYPE(char)
OBFY_TYPE(signed char)
OBFY_TYPE(unsigned char)
OBFY_TYPE(char16_t)
OBFY_TYPE(char32_t)
OBFY_TYPE(wchar_t)

OBFY_TYPE(short int)
OBFY_TYPE(unsigned short int)

OBFY_TYPE(int)
OBFY_TYPE(unsigned int)

OBFY_TYPE(long int)
OBFY_TYPE(unsigned long int)

OBFY_TYPE(long long int)
OBFY_TYPE(unsigned long long int)

#if defined OBFY_DEBUG

#define OBFY_BEGIN_CODE
#define OBFY_END_CODE

#define OBFY_V(x) x
#define OBFY_N(x) x

#define OBFY_RETURN(x) return x;

#define OBFY_BREAK break;
#define OBFY_CONTINUE continue;

#define OBFY_IF(x) if(x) {
#define OBFY_ELSE } else {
#define OBFY_ENDIF }

#define OBFY_FOR(init,cond,inc) for(init;cond;inc) {
#define OBFY_ENDFOR }

#define OBFY_WHILE(x) while(x) {
#define OBFY_ENDWHILE }

#define OBFY_REPEAT   do {
#define OBFY_AS_LONG_AS(x) } while ((x));

#define OBFY_CASE(a) switch (a) {
#define OBFY_ENDCASE }
#define OBFY_WHEN(c) case c:
#define OBFY_DO {
#define OBFY_DONE }
#define OBFY_OR
#define OBFY_DEFAULT default:



#else
#define OBFY_JOIN(a,b) a##b
#ifndef OBFY_DISABLE_RUNTIME_TWEAK
#define OBFY_N(a) ([](){ constexpr uint64_t _obfy_k64 = OBFY_LOCAL_KEY(); const volatile uint64_t _obfy_rt = ::obfy::detail::runtime_tweak_seed(); uint64_t _obfy_mix = _obfy_k64 ^ _obfy_rt; using _obfy_T = decltype(a); static_assert(::obfy::obfy_is_intish<_obfy_T>::value, "OBFY_N expects integral/enum constant"); using _obfy_U = typename std::make_unsigned<_obfy_T>::type; return static_cast<_obfy_T>(obfy::Num<_obfy_U, static_cast<_obfy_U>(_obfy_k64 ^ static_cast<uint64_t>(a))>().get() ^ _obfy_mix ^ _obfy_rt); }())
#else
#define OBFY_N(a) ([](){ constexpr uint64_t _obfy_k64 = OBFY_LOCAL_KEY(); using _obfy_T = decltype(a); static_assert(::obfy::obfy_is_intish<_obfy_T>::value, "OBFY_N expects integral/enum constant"); using _obfy_U = typename std::make_unsigned<_obfy_T>::type; return static_cast<_obfy_T>(obfy::Num<_obfy_U, static_cast<_obfy_U>(_obfy_k64 ^ static_cast<uint64_t>(a))>().get() ^ static_cast<_obfy_U>(_obfy_k64)); }())
#endif
#define OBFY_DEFINE_EXTRA(N,implementer) template <typename T> struct extra_chooser<T,N> { using type = implementer<T>; }
OBFY_DEFINE_EXTRA(0, extra_xor);
OBFY_DEFINE_EXTRA(1, extra_substraction);
OBFY_DEFINE_EXTRA(2, extra_addition);
#define OBFY_V(a) ([&](){obfy::extra_chooser<std::remove_reference<decltype(a)>::type, obfy::MetaRandom<__COUNTER__, \
            OBFY_MAX_BOGUS_IMPLEMENTATIONS>::value >::type OBFY_JOIN(_ec_,__COUNTER__)(a);\
            return obfy::stream_helper();}() << a)

#define OBFY_FOR(init,cond,inc) { std::shared_ptr<obfy::base_rvholder> __rvlocal; obfy::for_wrapper( [&](){(init); return __crv; },\
           [&]()->bool{return (cond); }, \
           [&](){inc;return __crv;}).set_body( [&]() {
#define OBFY_ENDFOR return __crv;}).run(); }

#define OBFY_END return __crv;}).run(); }

#define OBFY_IF(x) {std::shared_ptr<obfy::base_rvholder> __rvlocal; obfy::if_wrapper(( [&]()->bool{ return (x); })).set_then( [&]() {
#define OBFY_ELSE return __crv;}).set_else( [&]() {
#define OBFY_ENDIF OBFY_END

#define OBFY_WHILE(x) {std::shared_ptr<obfy::base_rvholder> __rvlocal; obfy::while_wrapper([&]()->bool{ return (x); }).set_body( [&]() {
#define OBFY_ENDWHILE OBFY_END

#define OBFY_BREAK __crv = obfy::next_step::ns_break; throw __crv;
#define OBFY_CONTINUE __crv = obfy::next_step::ns_continue; throw __crv;

#define OBFY_RETURN(x) __rvlocal.reset(new obfy::rvholder<std::remove_reference<decltype(x)>::type>(x,x));  throw __rvlocal;

#define OBFY_REPEAT { std::shared_ptr<obfy::base_rvholder> __rvlocal; obfy::repeat_wrapper().set_body( [&]() {
#define OBFY_AS_LONG_AS(x) return __crv;}).set_condition([&]()->bool{ return ( (x) ); }).run(); }

#define OBFY_BEGIN_CODE try { obfy::next_step __crv = obfy::next_step::ns_done; std::shared_ptr<obfy::base_rvholder> __rvlocal; (void)__crv;
#define OBFY_END_CODE } catch(std::shared_ptr<obfy::base_rvholder>& r) { return *r; }
#define OBFY_END_CODE_VOID } catch(std::shared_ptr<obfy::base_rvholder>& r) { return; } catch (...) {throw;}
//#define OBFY_END_CODE } catch(std::shared_ptr<obfy::base_rvholder>& r) { return *r; } catch (...) {throw;}

#define OBFY_CASE(a) try { std::shared_ptr<obfy::base_rvholder> __rvlocal;\
                auto __avholder = a; obfy::case_wrapper<std::remove_reference<decltype(a)>::type>(a).
#define OBFY_ENDCASE run(); } catch(obfy::next_step&) {}
#define OBFY_WHEN(c) add_entry(obfy::branch<std::remove_reference<decltype(__avholder)>::type>\
                ( [&,__avholder]() -> std::remove_reference<decltype(__avholder)>::type \
                { std::remove_reference<decltype(__avholder)>::type __c = (c); return __c;} )).
#define OBFY_DO add_entry( obfy::body([&](){
#define OBFY_DONE return obfy::next_step::ns_continue;})).
#define OBFY_OR join().
#define OBFY_DEFAULT add_default(obfy::body([&](){

#endif

} // namespace obfy

#endif // __OBFY_HPP__
