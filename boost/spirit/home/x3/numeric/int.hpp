/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_INT_APR_17_2006_0830AM)
#define BOOST_SPIRIT_INT_APR_17_2006_0830AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/support/numeric_utils/extract_int.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct int_parser : parser<int_parser<T, Radix, MinDigits, MaxDigits>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
            "Error Unsupported Radix");

        typedef T attribute_type;

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            typedef extract_int<T, Radix, MinDigits, MaxDigits> extract;
            x3::skip_over(first, last, context);
            return extract::call(first, last, attr);
        }
    };

    typedef int_parser<short> short_type;
    short_type const short_ = short_type();

    typedef int_parser<long> long_type;
    long_type const long_ = long_type();

    typedef int_parser<int> int_type;
    int_type const int_ = int_type();

    typedef int_parser<long long> long_long_type;
    long_long_type const long_long = long_long_type();

}}}

#endif
