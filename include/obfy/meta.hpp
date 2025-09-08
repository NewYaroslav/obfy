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
