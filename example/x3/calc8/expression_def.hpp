/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_EXPRESSION_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC8_EXPRESSION_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "expression.hpp"
#include "common.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"

namespace client { namespace parser
{
    using x3::uint_;
    using x3::char_;
    using x3::raw;
    using x3::lexeme;
    using namespace x3::ascii;
    
    typedef x3::rule<class additive_expr, ast::expression> additive_expr_type;
    typedef x3::rule<class multiplicative_expr, ast::expression> multiplicative_expr_type;
    typedef x3::rule<class unary_expr, ast::operand> unary_expr_type;
    typedef x3::rule<class primary_expr, ast::operand> primary_expr_type;

    additive_expr_type const additive_expr = "additive_expr";
    multiplicative_expr_type const multiplicative_expr = "multiplicative_expr";
    unary_expr_type unary_expr = "unary_expr";
    primary_expr_type primary_expr = "primary_expr";
    
    auto const expression = additive_expr;

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

    BOOST_SPIRIT_DEFINE(
        additive_expr = additive_expr_def
      , multiplicative_expr = multiplicative_expr_def
      , unary_expr = unary_expr_def
      , primary_expr = primary_expr_def
    );
}}

namespace client
{
    parser::expression_type expression()
    {
        return parser::expression;
    }
}

#endif
