/*=============================================================================
    Copyright (c) 2019 Joel de Guzman
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

x3::rule<struct grammar_r, int> const grammar;
using grammar_type = decltype(grammar);
BOOST_SPIRIT_X3_DECLARE(grammar_type)
