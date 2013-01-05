/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_LITERAL_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_LITERAL_CHAR_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/char/char_parser.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Encoding, typename Attribute = typename Encoding::char_type>
    struct literal_char : char_parser<literal_char<Encoding>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;
        typedef Attribute attribute_type;

        template <typename Char>
        literal_char(Char ch)
          : ch(static_cast<char_type>(ch)) {}

        template <typename Char, typename Context>
        bool test(Char ch_, Context&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch_))
                && ch == char_type(ch_);
        }

        char_type ch;
    };
}}}

#endif
