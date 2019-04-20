/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_EXPRESSION_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC7_EXPRESSION_DEF_HPP

#include "expression.hpp"

#include "ast.hpp"
#include "ast_adapted.hpp"
#include "error_handler.hpp"

#include <boost/spirit/home/x3.hpp>

namespace client { namespace calculator_grammar
{
    using x3::uint_;
    using x3::char_;

    x3::rule<struct term_r, ast::expression> const term = "term";
    x3::rule<struct factor_r, ast::operand> const factor = "factor";

    auto const expression_def =
        term
        >> *(   (char_('+') > term)
            |   (char_('-') > term)
            )
        ;

    auto const term_def =
        factor
        >> *(   (char_('*') > factor)
            |   (char_('/') > factor)
            )
        ;

    auto const factor_def =
            uint_
        |   '(' > expression > ')'
        |   (char_('-') > factor)
        |   (char_('+') > factor)
        ;

    BOOST_SPIRIT_DEFINE(
        expression
      , term
      , factor
    );

    struct expression_r : error_handler {};
}}

#endif
