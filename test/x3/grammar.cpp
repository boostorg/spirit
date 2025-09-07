/*=============================================================================
    Copyright (c) 2019 Joel de Guzman
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "grammar.hpp"

auto const grammar_def = x3::int_;

BOOST_SPIRIT_X3_DEFINE(grammar)
BOOST_SPIRIT_X3_INSTANTIATE(grammar_type, char const*, x3::unused_type)
