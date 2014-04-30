/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_SUPPORT_UTILITY_INTEGER_SEQUENCE_HPP_INCLUDED
#define BOOST_SPIRIT_X3_SUPPORT_UTILITY_INTEGER_SEQUENCE_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <cstddef>


namespace boost { namespace spirit { namespace x3
{
    template<class T, T... Ns>
    struct integer_sequence
    {};
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template<class T>
    struct integer_sequence_builder
    {
        template<class S1, class S2>
        struct concat;

        template<T... N1, T... N2>
        struct concat<integer_sequence<T, N1...>, integer_sequence<T, N2...>>
        {
            typedef integer_sequence<T, N1..., N2...> type;
        };

        template<T N, class S>
        struct offset;

        template<T N, T... Ns>
        struct offset<N, integer_sequence<T, Ns...>>
        {
            typedef integer_sequence<T, (N + Ns)...> type;
        };

        template<T N, class = void>
        struct make
        {
            static T const m = N / 2;
            typedef typename make<m>::type part1;
            typedef typename make<N - m>::type part2;
            typedef typename
                concat<part1, typename offset<m, part2>::type>::type
            type;
        };

        template<class Dummy>
        struct make<0, Dummy>
        {
            typedef integer_sequence<T> type;
        };
        
        template<class Dummy>
        struct make<1, Dummy>
        {
            typedef integer_sequence<T, 0> type;
        };
    };
}}}}

namespace boost { namespace spirit { namespace x3
{
    template<std::size_t... Ns>
    using index_sequence = integer_sequence<std::size_t, Ns...>;

    template<class T, T N>
    using make_integer_sequence =
        typename detail::integer_sequence_builder<T>::
            template make<N>::type;

    template<std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;

    template<class... T>
    using index_sequence_for = make_index_sequence<sizeof...(T)>;
}}}


#endif

