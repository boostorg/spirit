/*=============================================================================
    Copyright (c) 1999-2003 Jaakko Järvi
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_EQUAL_TO_HPP)
#define FUSION_SEQUENCE_EQUAL_TO_HPP

#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_equal_to.hpp>

namespace boost { namespace fusion
{
    template <typename Seq1, typename Seq2>
    inline bool
    operator==(sequence_base<Seq1> const& a, sequence_base<Seq2> const& b)
    {
        return detail::sequence_equal_to<Seq1 const, Seq2 const>::
            apply(
                begin(static_cast<Seq1 const&>(a))
              , begin(static_cast<Seq2 const&>(b))
            );
    }
}}

#endif
