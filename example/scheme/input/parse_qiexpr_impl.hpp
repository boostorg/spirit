//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_PARSE_QIEXPR_IMPL)
#define BOOST_SPIRIT_PARSE_QIEXPR_IMPL

#include <iostream>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include "../input/qiexpr.hpp"
#include "../input/parse_qiexpr.hpp"

namespace scheme { namespace input
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename String>
    bool parse_qiexpr(String const& str, utree& result)
    {
        typedef typename String::const_iterator iterator_type;

        scheme::input::qiexpr<iterator_type> p;
        scheme::input::qiexpr_white_space<iterator_type> ws;

        iterator_type begin = str.begin();
        iterator_type end = str.end();
        return phrase_parse(begin, end, p, ws, result) && begin == end;
    }
}}

#endif


