#ifndef __OBFY_STR_HPP__
#define __OBFY_STR_HPP__

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

namespace obfy {
namespace detail {

    template<std::size_t... I>
    struct index_sequence { };

    template<std::size_t N, std::size_t... I>
    struct make_index_sequence_impl : make_index_sequence_impl<N-1, N-1, I...> { };

    template<std::size_t... I>
    struct make_index_sequence_impl<0, I...> { using type = index_sequence<I...>; };

    template<std::size_t N>
    using make_index_sequence = typename make_index_sequence_impl<N>::type;

    template<typename Char, unsigned char K1, unsigned char K2, unsigned char K3, typename Seq>
    struct obf_string_impl;

    template<typename Char, unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    struct obf_string_impl<Char, K1, K2, K3, index_sequence<I...>> {
        alignas(Char) unsigned char data[sizeof...(I) + sizeof(Char)];
        mutable std::once_flag once_;
        template<std::size_t N>
        constexpr obf_string_impl(const Char (&s)[N])
            : data{ encode(reinterpret_cast<const unsigned char*>(s)[I], I)... } {}
        const Char* decrypt() {
            std::call_once(once_, [&]{
                for (std::size_t i = 0; i < sizeof...(I); ++i)
                    data[i] = decode(data[i], i);
            });
            return reinterpret_cast<const Char*>(data);
        }
        struct tmp_string {
            std::basic_string<Char> str;
            ~tmp_string() { for (std::size_t i = 0; i < str.size(); ++i) str[i] = Char(); }
            operator const std::basic_string<Char>&() const { return str; }
            operator const Char*() const { return str.c_str(); }
        };
        tmp_string decrypt_once() const {
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

#ifndef OBFY_TU_SALT
#  define OBFY_TU_SALT 0ull
#endif

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
#define OBFY_STR_ONCE(s) ([](){ OBFY_DEF_STR(s) _obfy_str{ s }; return _obfy_str.decrypt_once(); }())
#define OBFY_WSTR_ONCE(s) ([](){ OBFY_DEF_WSTR(s) _obfy_wstr{ s }; return _obfy_wstr.decrypt_once(); }())

#endif // __OBFY_STR_HPP__
