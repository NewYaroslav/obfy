#ifndef __OBFY_STR_HPP__
#define __OBFY_STR_HPP__

#include <cstddef>
#include <cstdint>
#include <mutex>

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

    template<unsigned char K1, unsigned char K2, unsigned char K3, typename Seq>
    struct obf_string;

    template<unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    struct obf_string<K1, K2, K3, index_sequence<I...>> {
        unsigned char data[sizeof...(I) + 1];
        mutable std::once_flag once_;
        template<std::size_t N>
        constexpr obf_string(const char (&s)[N])
            : data{ encode(s[I], I)..., 0 } {}
        const char* decrypt() {
            std::call_once(once_, [&]{
                for (std::size_t i = 0; i < sizeof...(I); ++i)
                    data[i] = decode(data[i], i);
            });
            return reinterpret_cast<const char*>(data);
        }
        static constexpr unsigned char encode(char c, std::size_t i) {
            unsigned char uc = static_cast<unsigned char>(c);
            return static_cast<unsigned char>(((uc ^ K1) + K2) ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i)));
        }
        static constexpr unsigned char decode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(((c ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i))) - K2) ^ K1);
        }
    };

} // namespace detail
} // namespace obfy

#define OBFY_DEF_STR(s) \
    ::obfy::detail::obf_string< \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value), \
        ::obfy::detail::make_index_sequence<sizeof(s) - 1>>

#define OBFY_STR(s) ([](){ static OBFY_DEF_STR(s) _obfy_str{ s }; return _obfy_str.decrypt(); }())

#endif // __OBFY_STR_HPP__
