#ifndef __OBFY_META_HPP__
#define __OBFY_META_HPP__

#include <cstddef>

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

} // namespace detail
} // namespace obfy

#endif // __OBFY_META_HPP__
