//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_PARSE_SEXPR_IMPL)
#define BOOST_SPIRIT_PARSE_SEXPR_IMPL

#include <iostream>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include "../input/sexpr.hpp"
#include "../input/parse_sexpr.hpp"

namespace scheme { namespace input
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename InputStream>
    bool parse_sexpr(InputStream& is, utree& result)
    {
        // no white space skipping in the stream!
        is.unsetf(std::ios::skipws);

        typedef boost::spirit::basic_istream_iterator<char> iterator_type;
        iterator_type first(is);
        iterator_type last;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::white_space<iterator_type> ws;

        return phrase_parse(first, last, p, ws, result);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename InputStream>
    bool parse_sexpr_list(InputStream& is, utree& result)
    {
        // no white space skipping in the stream!
        is.unsetf(std::ios::skipws);

        typedef boost::spirit::basic_istream_iterator<char> iterator_type;
        iterator_type first(is);
        iterator_type last;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::white_space<iterator_type> ws;

        return phrase_parse(first, last, +p, ws, result);
    }
}}

#endif


