/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_EXPRESSION_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC8_EXPRESSION_DEF_HPP

#include "expression.hpp"

#include "ast.hpp"
#include "ast_adapted.hpp"
#include "common.hpp"
#include "error_handler.hpp"
#include "config.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

namespace client { namespace parser
{
    using x3::uint_;
    using x3::char_;
    using x3::raw;
    using x3::lexeme;
    using namespace x3::ascii;

    x3::rule<struct additive_expr_r, ast::expression> const additive_expr = "additive_expr";
    x3::rule<struct multiplicative_expr_r, ast::expression> const multiplicative_expr = "multiplicative_expr";
    x3::rule<struct unary_expr_r, ast::operand> const unary_expr = "unary_expr";
    x3::rule<struct primary_expr_r, ast::operand> const primary_expr = "primary_expr";

    auto const additive_expr_def =
        multiplicative_expr
        >> *(   (char_('+') > multiplicative_expr)
            |   (char_('-') > multiplicative_expr)
            )
        ;

    auto const multiplicative_expr_def =
        unary_expr
        >> *(   (char_('*') > unary_expr)
            |   (char_('/') > unary_expr)
            )
        ;

    auto const unary_expr_def =
            primary_expr
        |   (char_('-') > primary_expr)
        |   (char_('+') > primary_expr)
        ;

    auto const primary_expr_def =
            uint_
        |   identifier
        |   '(' > expression > ')'
        ;

    auto const expression_def = additive_expr;

    BOOST_SPIRIT_DEFINE(
        expression
      , additive_expr
      , multiplicative_expr
      , unary_expr
      , primary_expr
    );

    struct unary_expr_r : x3::annotate_on_success {};
    struct primary_expr_r : x3::annotate_on_success {};
}}

#endif
