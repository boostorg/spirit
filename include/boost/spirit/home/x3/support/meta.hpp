/*=============================================================================
    Copyright (c) 2020 Nikita Kniazev

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_X3_SUPPORT_META
#define BOOST_SPIRIT_X3_SUPPORT_META

#include <boost/spirit/home/x3/support/unused.hpp>
#include <type_traits>

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename... T>
    struct type_sequence
    {
        using type = type_sequence;

        static const int size = sizeof...(T);

        template <typename... U>
        using append = type_sequence<T..., U...>;

        template <typename... U>
        using prepend = type_sequence<U..., T...>;

        template <typename U>
        using extend = typename U::template prepend<T...>;

        template <template <typename...> class U>
        using transfer_to = U<T...>;

        template <typename U>
        static constexpr bool contains = (std::is_same_v<U, T> || ...);
    };

    template <bool>
    struct conditional_impl
    {
        template <typename T, typename>
        using type = T;
    };

    template <>
    struct conditional_impl<false>
    {
        template <typename, typename F>
        using type = F;
    };

    template <bool B, typename T, typename F>
    using conditional_t = typename conditional_impl<B>::template type<T, F>;
}}}}

#endif
