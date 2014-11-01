/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ANY_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_X3_ANY_CHAR_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/extent.hpp>
#include <boost/spirit/home/x3/char/literal_char.hpp>
#include <boost/spirit/home/x3/char/char_set.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Encoding>
    struct any_char : char_parser<any_char<Encoding>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;
        typedef char_type attribute_type;
        static bool const has_attribute = true;

        template <typename Char, typename Context>
        bool test(Char ch_, Context const&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch_));
        }

        template <typename Char>
        literal_char<Encoding>
        operator()(Char ch) const
        {
            return literal_char<Encoding>(ch);
        }

        template <typename Char>
        literal_char<Encoding>
        operator()(const Char (&ch)[2]) const
        {
            return literal_char<Encoding>(ch[0]);
        }

        template <typename Char, std::size_t N>
        char_set<Encoding>
        operator()(const Char (&ch)[N]) const
        {
            return char_set<Encoding>(ch);
        }


        template <typename Char>
        char_range<Encoding>
        operator()(Char from, Char to) const
        {
            return char_range<Encoding>(from,to);
        }

        template <typename Char>
        char_range<Encoding>
        operator()(Char (&from)[2], Char (&to)[2]) const
        {
            return char_range<Encoding>(from[0],to[0]);
        }


        template <typename Char>
        char_set<Encoding>
        operator()(std::basic_string<Char> const& s) const
        {
            return char_set<Encoding>(s);
        }

    };
}}}

#endif
