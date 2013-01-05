/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_ANY_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_ANY_CHAR_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/char/literal_char.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Encoding>
    struct any_char : char_parser<any_char<Encoding>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;
        typedef char_type attribute_type;

        template <typename Char, typename Context>
        bool test(Char ch_, Context&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch_));
        }

        template <typename Char>
        literal_char<Encoding>
        operator()(Char ch) const
        {
            return literal_char<Encoding>(ch);
        }
    };
}}}

#endif
