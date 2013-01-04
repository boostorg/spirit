/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_PARSE_APRIL_16_2006_0442PM)
#define BOOST_SPIRIT_PARSE_APRIL_16_2006_0442PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/concept_check.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename P>
    inline bool
    parse(
        Iterator& first
      , Iterator last
      , parser<P> const& p)
    {
        // Make sure the iterator is at least a forward_iterator. If you got a
        // compilation error here, then you are using an input_iterator while
        // calling this function, you need to supply at least a
        // forward_iterator instead.
        BOOST_CONCEPT_ASSERT((ForwardIterator<Iterator>));

        return p.derived().parse(first, last, unused, unused);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename P, typename S>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , parser<P> const& p
      , parser<S> const& s)
    {
        // Make sure the iterator is at least a forward_iterator. If you got a
        // compilation error here, then you are using an input_iterator while
        // calling this function, you need to supply at least a
        // forward_iterator instead.
        BOOST_CONCEPT_ASSERT((ForwardIterator<Iterator>));

        context<skipper_tag, S const> skipper(s.derived());
        return p.derived().parse(first, last, skipper, unused);
    }
}}}

#endif

