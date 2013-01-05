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
#include <boost/spirit/home/support/char_encoding/standard.hpp>
#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>

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

        template <typename Char, typename Context>
        bool test(Char ch_, Context&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch_))
                && ch == char_type(ch_);
        }

        char_type ch;
    };

    template <typename Encoding>
    struct any_char : char_parser<any_char<Encoding>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;
        typedef literal_char<Encoding> literal_char_type;

        template <typename Char, typename Context>
        bool test(Char ch_, Context&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch_));
        }

        template <typename Char>
        literal_char_type
        operator()(Char ch) const
        {
            return literal_char_type(ch);
        }
    };

    namespace standard
    {
        typedef any_char<char_encoding::standard> char_type;
        char_type const char_ = char_type();
    }

    namespace standard_wide
    {
        typedef any_char<char_encoding::standard_wide> char_type;
        char_type const char_ = char_type();
    }

    namespace ascii
    {
        typedef any_char<char_encoding::ascii> char_type;
        char_type const char_ = char_type();
    }

    namespace extension
    {
        template <>
        struct as_parser<char>
        {
            typedef standard::char_type::literal_char_type type;
            static type call(char ch)
            {
                return type(ch);
            }
        };

        template <>
        struct as_parser<wchar_t>
        {
            typedef standard_wide::char_type::literal_char_type type;
            static type call(wchar_t ch)
            {
                return type(ch);
            }
        };
    }
}}}

#endif
