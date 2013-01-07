/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_LITERAL_STRING_APR_18_2006_1125PM)
#define BOOST_SPIRIT_LITERAL_STRING_APR_18_2006_1125PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/string/detail/string_parse.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>
#include <boost/spirit/home/support/char_encoding/standard.hpp>
#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <string>

namespace boost { namespace spirit { namespace x3
{
    template <typename String, typename Encoding,
        typename Attribute = std::basic_string<typename Encoding::char_type>>
    struct literal_string : parser<literal_string<String, Encoding, Attribute>>
    {
        typedef typename Encoding::char_type char_type;
        typedef Encoding encoding;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<unused_type, attribute_type>::value;

        literal_string(typename add_reference<String>::type str)
          : str(str)
        {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            x3::skip_over(first, last, context);
            return detail::string_parse(str, first, last, attr);
        }

        String str;
    };

    namespace standard
    {
        inline literal_string<char const*, char_encoding::standard>
        string(char const* s)
        {
            return literal_string<char const*, char_encoding::standard>(s);
        }
    }

    using standard::string;

    inline literal_string<char const*, char_encoding::standard, unused_type>
    lit(char const* s)
    {
        return literal_string<char const*, char_encoding::standard, unused_type>(s);
    }
}}}

#endif
