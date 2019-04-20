/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_STATEMENT_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC8_STATEMENT_DEF_HPP

#include "statement.hpp"

#include "ast.hpp"
#include "ast_adapted.hpp"
#include "expression.hpp"
#include "common.hpp"
#include "error_handler.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

namespace client { namespace parser
{
    using x3::raw;
    using x3::lexeme;
    using namespace x3::ascii;

    x3::rule<struct statement_list_r, ast::statement_list> const statement_list = "statement_list";
    x3::rule<struct variable_declaration_r, ast::variable_declaration> const variable_declaration = "variable_declaration";
    x3::rule<struct assignment_r, ast::assignment> const assignment = "assignment";
    x3::rule<struct variable_r, ast::variable> const variable = "variable";

    // Import the expression rule
    namespace { using client::expression; }

    auto const statement_list_def =
        +(variable_declaration | assignment)
        ;

    auto const variable_declaration_def =
            lexeme["var" >> !(alnum | '_')] // make sure we have whole words
        >   assignment
        ;

    auto const assignment_def =
            variable
        >   '='
        >   expression
        >   ';'
        ;

    auto const variable_def = identifier;
    auto const statement_def = statement_list;

    BOOST_SPIRIT_DEFINE(
        statement
      , statement_list
      , variable_declaration
      , assignment
      , variable
    );

    struct statement_r : error_handler_base, x3::annotate_on_success {};
    struct assignment_r : x3::annotate_on_success {};
    struct variable_r : x3::annotate_on_success {};
}}

#endif
