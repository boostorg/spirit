/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_CHAR_CLASS_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_CHAR_CLASS_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/x3/char/detail/cast_char.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    struct char_tag {};
    struct alnum_tag {};
    struct alpha_tag {};
    struct blank_tag {};
    struct cntrl_tag {};
    struct digit_tag {};
    struct graph_tag {};
    struct print_tag {};
    struct punct_tag {};
    struct space_tag {};
    struct xdigit_tag {};
    struct lower_tag {};
    struct upper_tag {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Encoding, typename Tag>
    struct char_class
      : char_parser<char_class<Encoding, Tag>>
    {
        typedef Encoding encoding;
        typedef Tag tag;
        typedef typename Encoding::char_type char_type;
        typedef char_type attribute_type;

#define BOOST_SPIRIT_CLASSIFY(name)                                             \
        template <typename Char>                                                \
        static bool                                                             \
        is(name##_tag, Char ch)                                                 \
        {                                                                       \
            return Encoding::is##name                                           \
                BOOST_PREVENT_MACRO_SUBSTITUTION                                \
                    (detail::cast_char<char_type>(ch));                         \
        }                                                                       \
        /***/

        BOOST_SPIRIT_CLASSIFY(char)
        BOOST_SPIRIT_CLASSIFY(alnum)
        BOOST_SPIRIT_CLASSIFY(alpha)
        BOOST_SPIRIT_CLASSIFY(digit)
        BOOST_SPIRIT_CLASSIFY(xdigit)
        BOOST_SPIRIT_CLASSIFY(cntrl)
        BOOST_SPIRIT_CLASSIFY(graph)
        BOOST_SPIRIT_CLASSIFY(lower)
        BOOST_SPIRIT_CLASSIFY(print)
        BOOST_SPIRIT_CLASSIFY(punct)
        BOOST_SPIRIT_CLASSIFY(space)
        BOOST_SPIRIT_CLASSIFY(blank)
        BOOST_SPIRIT_CLASSIFY(upper)

#undef BOOST_SPIRIT_CLASSIFY

        template <typename Char, typename Context>
        bool test(Char ch, Context&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch))
                && is(tag(), ch);
        }
    };

    namespace ascii
    {
        typedef char_class<char_encoding::ascii, space_tag> space_type;
        space_type const space = space_type();
    }

}}}

#endif
