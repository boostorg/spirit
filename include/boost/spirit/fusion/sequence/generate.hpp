/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_GENERATE_HPP)
#define FUSION_SEQUENCE_GENERATE_HPP

#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/sequence/detail/generate.hpp>

namespace boost { namespace fusion
{
    template <typename Sequence>
    struct result_of_generate
    {
        typedef typename
            detail::result_of_generate<
                typename result_of_begin<Sequence>::type
              , typename result_of_end<Sequence>::type
            >::type
        type;
    };

    template <typename Sequence>
    inline typename result_of_generate<Sequence const>::type
    generate(Sequence const& seq)
    {
        return detail::generate(fusion::begin(seq), fusion::end(seq));
    }
}}

#endif


