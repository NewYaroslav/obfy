#ifndef OBFY_STR_HPP
#define OBFY_STR_HPP

#include <cstddef>
#include <cstdint>

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

    template<char K1, char K2, char K3, typename Seq>
    struct obf_string;

    template<char K1, char K2, char K3, std::size_t... I>
    struct obf_string<K1, K2, K3, index_sequence<I...>> {
        unsigned char data[sizeof...(I) + 1];
        constexpr obf_string(const char* str)
            : data{ encode(str[I], I)..., 0 } {}
        const char* decrypt() {
            for (std::size_t i = 0; i < sizeof...(I); ++i) {
                data[i] = decode(data[i], i);
            }
            return reinterpret_cast<const char*>(data);
        }
        static constexpr unsigned char encode(char c, std::size_t i) {
            return static_cast<unsigned char>(
                (( (static_cast<unsigned char>(c) ^ static_cast<unsigned char>(K1))
                 + static_cast<unsigned char>(K2))
                 ^ static_cast<unsigned char>(static_cast<unsigned char>(K3) + static_cast<unsigned char>(i))));
        }
        static constexpr unsigned char decode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(
                (( (c ^ static_cast<unsigned char>(static_cast<unsigned char>(K3) + static_cast<unsigned char>(i)))
                 - static_cast<unsigned char>(K2))
                 ^ static_cast<unsigned char>(K1)));
        }
    };

} // namespace detail
} // namespace obfy

#define OBFY_DEF_STR(s) \
    ::obfy::detail::obf_string< \
        static_cast<char>(::obfy::MetaRandom<__COUNTER__, 0x7F>::value + 1), \
        static_cast<char>(::obfy::MetaRandom<__COUNTER__, 0x7F>::value + 1), \
        static_cast<char>(::obfy::MetaRandom<__COUNTER__, 0x7F>::value + 1), \
        ::obfy::detail::make_index_sequence<sizeof(s) - 1>>(s)

#define OBFY_STR(s) ([](){ static auto _obfy_str = OBFY_DEF_STR(s); return _obfy_str.decrypt(); }())

#endif // OBFY_STR_HPP
