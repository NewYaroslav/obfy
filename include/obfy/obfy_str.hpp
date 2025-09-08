/*
 * Copyright (c) 2025, NewYaroslav
 * Inspired by ADVobfuscator (Sebastien Andrivet)
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
#ifndef __OBFY_STR_HPP__
#define __OBFY_STR_HPP__

#include <cstddef>
#include <mutex>
#include <string>

#include <obfy/obfy.hpp>
#include <obfy/meta.hpp>

namespace obfy {
namespace detail {

    template<typename Char, unsigned char K1, unsigned char K2, unsigned char K3, typename Seq>
    struct obf_string_impl;

    template<typename Char, unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    struct obf_string_impl<Char, K1, K2, K3, index_sequence<I...>> {
        alignas(Char) unsigned char data[sizeof...(I) + sizeof(Char)];
        mutable std::once_flag once_;
        template<typename SChar, std::size_t N>
        obf_string_impl(const SChar (&s)[N])
            : data{ encode(reinterpret_cast<const unsigned char*>(s)[I], I)... } {
            static_assert(sizeof(s[0]) == sizeof(Char), "string literal has wrong character width");
        }
        static_assert((sizeof...(I) % sizeof(Char)) == 0, "byte count must be multiple of Char");
        inline const Char* decrypt() {
            std::call_once(once_, [&]{
                for (std::size_t i = 0; i < sizeof...(I); ++i)
                    data[i] = decode(data[i], i);
            });
            return reinterpret_cast<const Char*>(data);
        }
        struct tmp_string {
            std::basic_string<Char> str;
            inline ~tmp_string() { for (std::size_t i = 0; i < str.size(); ++i) str[i] = Char(); }
            inline operator const std::basic_string<Char>&() const { return str; }
            inline const Char* c_str() const { return str.c_str(); } // valid while tmp_string lives
        };
        inline tmp_string decrypt_once() const {
            tmp_string tmp;
            tmp.str.resize(sizeof...(I) / sizeof(Char));
            unsigned char* raw = reinterpret_cast<unsigned char*>(&tmp.str[0]);
            for (std::size_t i = 0; i < sizeof...(I); ++i)
                raw[i] = decode(data[i], i);
            return tmp;
        }
        static constexpr unsigned char encode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(((c ^ K1) + K2) ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i)));
        }
        static constexpr unsigned char decode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(((c ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i))) - K2) ^ K1);
        }
    };

    template<unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    using obf_string = obf_string_impl<char, K1, K2, K3, index_sequence<I...>>;

    template<unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    using obf_wstring = obf_string_impl<wchar_t, K1, K2, K3, index_sequence<I...>>;

} // namespace detail
} // namespace obfy

#define OBFY_DEF_STR_T(Char, s) \
    ::obfy::detail::obf_string_impl<Char, \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 0) & 0xFF)), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 8) & 0xFF)), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 16) & 0xFF)), \
        ::obfy::detail::make_index_sequence<sizeof(s) - sizeof(Char)>>

#define OBFY_DEF_STR(s) OBFY_DEF_STR_T(char, s)
#define OBFY_DEF_WSTR(s) OBFY_DEF_STR_T(wchar_t, s)

#define OBFY_STR(s) ([](){ static OBFY_DEF_STR(s) _obfy_str{ s }; return _obfy_str.decrypt(); }())
#define OBFY_WSTR(s) ([](){ static OBFY_DEF_WSTR(s) _obfy_wstr{ s }; return _obfy_wstr.decrypt(); }())
// `*_ONCE` macros return a temporary wiping its contents on destruction.
// `c_str()` remains valid only for the full expression.
#define OBFY_STR_ONCE(s) ([](){ OBFY_DEF_STR(s) _obfy_str{ s }; return _obfy_str.decrypt_once(); }())
#define OBFY_WSTR_ONCE(s) ([](){ OBFY_DEF_WSTR(s) _obfy_wstr{ s }; return _obfy_wstr.decrypt_once(); }())

#endif // __OBFY_STR_HPP__
