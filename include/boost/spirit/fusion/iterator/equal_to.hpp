/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_EQUAL_TO_HPP)
#define FUSION_ITERATOR_EQUAL_TO_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_const.hpp>

namespace boost { namespace fusion
{
    template <typename Tag>
    struct equal_to_traits
    {
        template <typename I1, typename I2>
        struct impl
        {
            typedef typename
                is_same<
                    typename add_const<I1>::type
                  , typename add_const<I2>::type
                >::type
            type;
            BOOST_STATIC_CONSTANT(bool, value = FUSION_GET_VALUE(type));
        };
    };

    template <typename I1, typename I2>
    struct equal_to
    {
        typedef typename
            equal_to_traits<FUSION_GET_TAG(I1)>::
                template impl<I1, I2>::type
        type;
        BOOST_STATIC_CONSTANT(bool, value = FUSION_GET_VALUE(type));
    };
}}

#endif

