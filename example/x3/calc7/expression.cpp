/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "expression_def.hpp"

namespace client { namespace parser
{
    template <typename Iterator, typename Skipper>
    auto expression_parser(Skipper const& skipper)
    -> x3::any_parser<Iterator, ast::expression, decltype(x3::make_context<x3::skipper_tag>(skipper))>
    {
        return { expression };
    }

    auto x = expression_parser<char const*>(x3::space);
}}