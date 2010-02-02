/*=============================================================================
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UNICODE_1_JANUARY_12_2010_0728PM)
#define BOOST_SPIRIT_UNICODE_1_JANUARY_12_2010_0728PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/cstdint.hpp>
#include <boost/spirit/home/support/char_encoding/unicode/query.hpp>

namespace boost { namespace spirit { namespace char_encoding
{
    ///////////////////////////////////////////////////////////////////////////
    //  Test characters for specified conditions (using iso8859-1)
    ///////////////////////////////////////////////////////////////////////////
    struct unicode
    {
        typedef ::boost::uint32_t char_type;

        static bool
        isascii_(char_type ch)
        {
            return 0 == (ch & ~0x7f);
        }

        static bool
        ischar(char_type ch)
        {
            // unicode code points in the range 0x00 to 0x10FFFF
            return ch <= 0x10FFFF;
        }

        static bool
        isalnum(char_type ch)
        {
            return ucd::is_alphanumeric(ch);
        }

        static bool
        isalpha(char_type ch)
        {
            return ucd::is_alphabetic(ch);
        }

        static bool
        isdigit(char_type ch)
        {
            return ucd::is_decimal_number(ch);
        }

        static bool
        isxdigit(char_type ch)
        {
            return ucd::is_hexadecimal_number(ch);
        }

        static bool
        iscntrl(char_type ch)
        {
            return ucd::is_control(ch);
        }

        static bool
        isgraph(char_type ch)
        {
            return ucd::is_graph(ch);
        }

        static bool
        islower(char_type ch)
        {
            return ucd::is_lowercase(ch);
        }

        static bool
        isprint(char_type ch)
        {
            return ucd::is_print(ch);
        }

        static bool
        ispunct(char_type ch)
        {
            return ucd::is_punctuation(ch);
        }

        static bool
        isspace(char_type ch)
        {
            return ucd::is_white_space(ch);
        }

        static int
        isblank BOOST_PREVENT_MACRO_SUBSTITUTION (char_type ch)
        {
            return ucd::is_blank(ch);
        }

        static bool
        isupper(char_type ch)
        {
            return ucd::is_uppercase(ch);
        }

    ///////////////////////////////////////////////////////////////////////////
    //  Simple character conversions
    ///////////////////////////////////////////////////////////////////////////

        static char_type
        tolower(char_type ch)
        {
            return ucd::to_lowercase(ch);
        }

        static char_type
        toupper(char_type ch)
        {
            return ucd::to_uppercase(ch);
        }

        static ::boost::uint32_t
        toucs4(char_type ch)
        {
            return ch;
        }
    };

}}}

#endif

