/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct tuple_tag;

    template <int N, typename Tuple>
    struct tuple_iterator;

    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    namespace detail
    {
        template <typename Tuple>
        struct begin_traits_impl
        {
            typedef tuple_iterator<0, Tuple> type;

            static type
            apply(Tuple& t);
        };

        template <typename Tuple>
        inline typename begin_traits_impl<Tuple>::type
        begin_traits_impl<Tuple>::apply(Tuple& t)
        {
            return type(t);
        }

        template <typename Tuple>
        struct end_traits_impl
        {
            typedef typename Tuple::size size;
            typedef tuple_iterator<FUSION_GET_VALUE(size), Tuple> type;

            static type
            apply(Tuple& t);
        };

        template <typename Tuple>
        inline typename end_traits_impl<Tuple>::type
        end_traits_impl<Tuple>::apply(Tuple& t)
        {
            return type(t);
        }
    }

    template <>
    struct begin_traits<tuple_tag>
    {
        template <typename Tuple>
        struct impl : detail::begin_traits_impl<Tuple> {};
    };

    template <>
    struct end_traits<tuple_tag>
    {
        template <typename Tuple>
        struct impl : detail::end_traits_impl<Tuple> {};
    };

}}

#endif
