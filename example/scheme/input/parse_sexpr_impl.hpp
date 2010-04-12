//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_PARSE_SEXPR_IMPL)
#define BOOST_SPIRIT_PARSE_SEXPR_IMPL

#include <iostream>
#include <string>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include "../input/sexpr.hpp"
#include "../input/parse_sexpr.hpp"

namespace scheme { namespace input
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Char>
    bool parse_sexpr(std::basic_istream<Char>& is, utree& result)
    {
        // no white space skipping in the stream!
        is.unsetf(std::ios::skipws);

        typedef boost::spirit::basic_istream_iterator<Char> iterator_type;
        iterator_type first(is);
        iterator_type last;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::sexpr_white_space<iterator_type> ws;

        using boost::spirit::qi::phrase_parse;
        return phrase_parse(first, last, p, ws, result);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Char>
    bool parse_sexpr_list(std::basic_istream<Char>& is, utree& result)
    {
        // no white space skipping in the stream!
        is.unsetf(std::ios::skipws);

        typedef boost::spirit::basic_istream_iterator<Char> iterator_type;
        iterator_type first(is);
        iterator_type last;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::sexpr_white_space<iterator_type> ws;

        using boost::spirit::qi::phrase_parse;
        return phrase_parse(first, last, +p, ws, result);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Char>
    bool parse_sexpr(std::basic_string<Char>& str, utree& result)
    {
        typedef typename std::basic_string<Char>::const_iterator iterator_type;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::sexpr_white_space<iterator_type> ws;

        using boost::spirit::qi::phrase_parse;
        return phrase_parse(str.begin(), str.end(), p, ws, result);
    }

    template <typename Char>
    bool parse_sexpr_list(std::basic_string<Char>& str, utree& result)
    {
        typedef typename std::basic_string<Char>::const_iterator iterator_type;

        scheme::input::sexpr<iterator_type> p;
        scheme::input::sexpr_white_space<iterator_type> ws;

        using boost::spirit::qi::phrase_parse;
        return phrase_parse(str.begin(), str.end(), +p, ws, result);
    }
}}

#endif


