/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC9_EXPRESSION_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC9_EXPRESSION_DEF_HPP

#include "expression.hpp"

#include "ast.hpp"
#include "ast_adapted.hpp"
#include "common.hpp"
#include "error_handler.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

namespace client { namespace parser
{
    using x3::uint_;
    using x3::char_;
    using x3::bool_;
    using x3::raw;
    using x3::lexeme;
    using namespace x3::ascii;

    ////////////////////////////////////////////////////////////////////////////
    // Tokens
    ////////////////////////////////////////////////////////////////////////////

    x3::symbols<ast::optoken> const equality_op {
        { "==", ast::op_equal },
        { "!=", ast::op_not_equal },
    };
    x3::symbols<ast::optoken> const relational_op {
        { "<", ast::op_less },
        { "<=", ast::op_less_equal },
        { ">", ast::op_greater },
        { ">=", ast::op_greater_equal },
    };
    x3::symbols<ast::optoken> const logical_op {
        { "&&", ast::op_and },
        { "||", ast::op_or },
    };
    x3::symbols<ast::optoken> const additive_op {
        { "+", ast::op_plus },
        { "-", ast::op_minus },
    };
    x3::symbols<ast::optoken> const multiplicative_op {
        { "*", ast::op_times },
        { "/", ast::op_divide },
    };
    x3::symbols<ast::optoken> const unary_op {
        { "+", ast::op_positive },
        { "-", ast::op_negative },
        { "!", ast::op_not },
    };
    x3::symbols<> const keywords {
        "var",
        "true",
        "false",
        "if",
        "else",
        "while",
    };

    ////////////////////////////////////////////////////////////////////////////
    // Main expression grammar
    ////////////////////////////////////////////////////////////////////////////

    x3::rule<struct equality_expr_r, ast::expression> const equality_expr = "equality_expr";
    x3::rule<struct relational_expr_r, ast::expression> const relational_expr = "relational_expr";
    x3::rule<struct logical_expr_r, ast::expression> const logical_expr = "logical_expr";
    x3::rule<struct additive_expr_r, ast::expression> const additive_expr = "additive_expr";
    x3::rule<struct multiplicative_expr_r, ast::expression> const multiplicative_expr = "multiplicative_expr";
    x3::rule<struct unary_expr_r, ast::operand> const unary_expr = "unary_expr";
    x3::rule<struct primary_expr_r, ast::operand> const primary_expr = "primary_expr";

    auto const logical_expr_def =
            equality_expr
        >> *(logical_op > equality_expr)
        ;

    auto const equality_expr_def =
            relational_expr
        >> *(equality_op > relational_expr)
        ;

    auto const relational_expr_def =
            additive_expr
        >> *(relational_op > additive_expr)
        ;

    auto const additive_expr_def =
            multiplicative_expr
        >> *(additive_op > multiplicative_expr)
        ;

    auto const multiplicative_expr_def =
            unary_expr
        >> *(multiplicative_op > unary_expr)
        ;

    auto const unary_expr_def =
            primary_expr
        |   (unary_op > primary_expr)
        ;

    auto const primary_expr_def =
            uint_
        |   bool_
        |   (!keywords >> identifier)
        |   '(' > expression > ')'
        ;

    auto const expression_def = logical_expr;

    BOOST_SPIRIT_DEFINE(
        expression
      , logical_expr
      , equality_expr
      , relational_expr
      , additive_expr
      , multiplicative_expr
      , unary_expr
      , primary_expr
    );

    struct unary_expr_r : x3::annotate_on_success {};
    struct primary_expr_r : x3::annotate_on_success {};
}}

#endif
