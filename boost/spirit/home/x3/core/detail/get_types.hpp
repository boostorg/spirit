/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_GET_TYPES_JAN_06_2013_1015AM)
#define SPIRIT_GET_TYPES_JAN_06_2013_1015AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_of.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/joint_view.hpp>

#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right, typename Derived>
    struct binary_parser;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename L, typename R>
    struct get_types
    {
        typedef
            mpl::vector<
                typename traits::attribute_of<L>::type
              , typename traits::attribute_of<R>::type
            >
        type;
    };

    template <typename LL, typename LR, typename D, typename R>
    struct get_types<binary_parser<LL, LR, D>, R>
    {
        typedef typename
            mpl::push_front<
                typename get_types<LL, LR>::type
              , typename traits::attribute_of<R>::type
            >::type
        type;
    };

    template <typename L, typename RL, typename RR, typename D>
    struct get_types<L, binary_parser<RL, RR, D>>
    {
        typedef typename
            mpl::push_back<
                typename get_types<RL, RR>::type
              , typename traits::attribute_of<L>::type
            >::type
        type;
    };

    template <typename LL, typename LR, typename DL
      , typename RL, typename RR, typename DR>
    struct get_types<binary_parser<LL, LR, DL>, binary_parser<RL, RR, DR>>
    {
        typedef
            mpl::joint_view<
                typename get_types<LL, LR>::type
              , typename get_types<RL, RR>::type
            >
        type;
    };
}}}}

#endif
