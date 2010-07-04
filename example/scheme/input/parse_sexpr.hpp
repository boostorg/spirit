//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_PARSE_SEXPR)
#define BOOST_SPIRIT_PARSE_SEXPR

#include "../utree.hpp"
#include "../input/sexpr.hpp"

namespace scheme { namespace input
{
    template <typename InputStream>
    bool parse_sexpr(InputStream& is, utree& result);

    template <typename InputStream>
    bool parse_sexpr_list(InputStream& is, utree& result);
}}

#endif


