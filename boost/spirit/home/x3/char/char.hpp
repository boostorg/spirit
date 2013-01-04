/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_CHAR_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Encoding>
    struct literal_char : char_parser<literal_char<Encoding>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;

        template <typename Char>
        literal_char(Char ch)
          : ch(static_cast<char_type>(ch)) {}

        template <typename CharParam, typename Context>
        bool test(CharParam ch_, Context&) const
        {
            return encoding::ischar(ch_) && ch == char_type(ch_);
        }

        char_type ch;
    };

    namespace ascii
    {
        template <typename Char>
        inline literal_char<char_encoding::ascii>
        char_(Char ch)
        {
            return literal_char<char_encoding::ascii>(ch);
        }
    }
}}}

#endif
