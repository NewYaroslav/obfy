#ifndef __OBFY_BYTES_HPP__
#define __OBFY_BYTES_HPP__

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <array>

#include <obfy/obfy.hpp>

namespace obfy {
namespace detail {

    template<unsigned char K1, unsigned char K2, unsigned char K3, typename Seq>
    struct obf_bytes_impl;

    template<unsigned char K1, unsigned char K2, unsigned char K3, std::size_t... I>
    struct obf_bytes_impl<K1, K2, K3, index_sequence<I...>> {
        unsigned char data[sizeof...(I)];
        mutable std::once_flag once_;
        template<std::size_t N>
        constexpr obf_bytes_impl(const char (&s)[N])
            : data{ encode(reinterpret_cast<const unsigned char*>(s)[I], I)... } {}
        const unsigned char* decrypt() {
            std::call_once(once_, [&]{
                for (std::size_t i = 0; i < sizeof...(I); ++i)
                    data[i] = decode(data[i], i);
            });
            return data;
        }
        struct tmp_block {
            std::array<unsigned char, sizeof...(I)> bytes;
            ~tmp_block() { for (std::size_t i = 0; i < bytes.size(); ++i) bytes[i] = 0; }
            const unsigned char* data() const { return bytes.data(); }
            std::size_t size() const { return bytes.size(); }
        };
        tmp_block decrypt_once() const {
            tmp_block tmp;
            for (std::size_t i = 0; i < sizeof...(I); ++i)
                tmp.bytes[i] = decode(data[i], i);
            return tmp;
        }
        static constexpr unsigned char encode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(((c ^ K1) + K2) ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i)));
        }
        static constexpr unsigned char decode(unsigned char c, std::size_t i) {
            return static_cast<unsigned char>(((c ^ static_cast<unsigned char>(K3 + static_cast<unsigned char>(i))) - K2) ^ K1);
        }
    };

} // namespace detail
} // namespace obfy

#ifndef OBFY_TU_SALT
#  define OBFY_TU_SALT 0ull
#endif

#define OBFY_DEF_BYTES(b) \
    ::obfy::detail::obf_bytes_impl< \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 0) & 0xFF)), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 8) & 0xFF)), \
        static_cast<unsigned char>(::obfy::MetaRandom<__COUNTER__, 256>::value ^ static_cast<unsigned char>((OBFY_TU_SALT >> 16) & 0xFF)), \
        ::obfy::detail::make_index_sequence<sizeof(b) - 1>>

#define OBFY_BYTES(b) ([](){ static OBFY_DEF_BYTES(b) _obfy_bytes{ b }; return _obfy_bytes.decrypt(); }())
#define OBFY_BYTES_ONCE(b) ([](){ OBFY_DEF_BYTES(b) _obfy_bytes{ b }; return _obfy_bytes.decrypt_once(); }())

#endif // __OBFY_BYTES_HPP__
