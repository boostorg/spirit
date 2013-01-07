/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_STRING_PARSE_APR_18_2006_1125PM)
#define BOOST_SPIRIT_STRING_PARSE_APR_18_2006_1125PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/assign_to.hpp>

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Char, typename Iterator, typename Attribute>
    inline bool string_parse(
        Char const* str
      , Iterator& first, Iterator const& last, Attribute& attr)
    {
        Iterator i = first;
        Char ch = *str;

        for (; !!ch; ++i)
        {
            if (i == last || (ch != *i))
                return false;
            ch = *++str;
        }

        spirit::traits::assign_to(first, i, attr);
        first = i;
        return true;
    }

    template <typename String, typename Iterator, typename Attribute>
    inline bool string_parse(
        String const& str
      , Iterator& first, Iterator const& last, Attribute& attr)
    {
        Iterator i = first;
        typename String::const_iterator stri = str.begin();
        typename String::const_iterator str_last = str.end();

        for (; stri != str_last; ++stri, ++i)
            if (i == last || (*stri != *i))
                return false;
        spirit::traits::assign_to(first, i, attr);
        first = i;
        return true;
    }
}}}}

#endif
