/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_END_HPP)
#define FUSION_SEQUENCE_END_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

namespace boost { namespace fusion
{
    template <typename Tag>
    struct end_traits
    {
        template <typename Sequence>
        struct impl {};
    };

    template <typename Sequence>
    struct result_of_end
    {
        typedef typename
            end_traits<FUSION_GET_TAG(Sequence)>::
                template impl<Sequence>::type
        type;
    };

    template <typename Sequence>
    inline typename result_of_end<Sequence const>::type
    end(sequence_base<Sequence> const& seq)
    {
        return end_traits<FUSION_GET_TAG(Sequence)>::
            template impl<Sequence const>::apply(
                static_cast<Sequence const&>(seq));
    }

    template <typename Sequence>
    inline typename result_of_end<Sequence>::type
    end(sequence_base<Sequence>& seq)
    {
        return end_traits<FUSION_GET_TAG(Sequence)>::
            template impl<Sequence>::apply(
                static_cast<Sequence&>(seq));
    }

}}

#endif
