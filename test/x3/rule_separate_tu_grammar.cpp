/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "rule_separate_tu_grammar.hpp"

#include <boost/spirit/home/x3.hpp>

namespace unused_attr {

auto const skipper_def = x3::standard::lit('*');
BOOST_SPIRIT_DEFINE(skipper)
BOOST_SPIRIT_INSTANTIATE(skipper_type, char const*, x3::unused_type)

auto const skipper2_def = x3::standard::lit('#');
BOOST_SPIRIT_DEFINE(skipper2)
BOOST_SPIRIT_INSTANTIATE(skipper2_type, char const*, x3::unused_type)

auto const grammar_def = *x3::standard::lit('=');
BOOST_SPIRIT_DEFINE(grammar)
BOOST_SPIRIT_INSTANTIATE(grammar_type, char const*, x3::unused_type)

using skipper_context_type = typename x3::phrase_parse_context<skipper_type, char const*>::type;
BOOST_SPIRIT_INSTANTIATE(grammar_type, char const*, skipper_context_type)

using skipper2_context_type = typename x3::phrase_parse_context<skipper2_type, char const*>::type;
BOOST_SPIRIT_INSTANTIATE(grammar_type, char const*, skipper2_context_type)

}

namespace used_attr {

auto const skipper_def = x3::standard::space;
BOOST_SPIRIT_DEFINE(skipper)
BOOST_SPIRIT_INSTANTIATE(skipper_type, char const*, x3::unused_type)

auto const grammar_def = x3::int_;
BOOST_SPIRIT_DEFINE(grammar)
BOOST_SPIRIT_INSTANTIATE(grammar_type, char const*, x3::unused_type)

using skipper_context_type = typename x3::phrase_parse_context<skipper_type, char const*>::type;
BOOST_SPIRIT_INSTANTIATE(grammar_type, char const*, skipper_context_type)

}
