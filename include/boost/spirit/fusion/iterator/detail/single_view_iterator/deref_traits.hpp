/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_DETAIL_SINGLE_VIEW_ITERATOR_DEREF_TRAITS_HPP)
#define FUSION_ITERATOR_DETAIL_SINGLE_VIEW_ITERATOR_DEREF_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/detail/access.hpp>
#include <boost/mpl/identity.hpp>

namespace boost { namespace fusion
{
    struct single_view_iterator_tag;

    template <typename Tag>
    struct deref_traits;

    template <>
    struct deref_traits<single_view_iterator_tag>
    {
        template <typename Iterator>
        struct impl
        {
            typedef typename detail::cref_result<
                mpl::identity<FUSION_GET_VALUE_TYPE(Iterator)> >::type
            type;

            static type
            apply(Iterator const& i)
            {
                return i.val;
            }
        };
    };
}}

#endif


