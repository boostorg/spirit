/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_STATEMENT_DEF_HPP)
#define BOOST_SPIRIT_X3_CALC8_STATEMENT_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "statement.hpp"
#include "expression.hpp"
#include "error_handler.hpp"

namespace client { namespace parser
{
    using x3::raw;
    using x3::lexeme;
    using namespace x3::ascii;
    
    typedef x3::rule<class statement_list, ast::statement_list> statement_list_type;
    typedef x3::rule<class variable_declaration, ast::variable_declaration> variable_declaration_type;
    typedef x3::rule<class assignment, ast::assignment> assignment_type;
    typedef x3::rule<class identifier, std::string> identifier_type;
    
    statement_list_type const statement_list("statement_list");
    variable_declaration_type const variable_declaration("variable_declaration");
    assignment_type const assignment("assignment");
    identifier_type const identifier("identifier");
    
    // Import the expression rule
    auto const expression = client::expression();

    auto const statement_list_def =
        +(variable_declaration | assignment)
        ;

    auto const identifier_def =
        raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
        ;

    auto const variable_declaration_def =
            lexeme["var" >> !(alnum | '_')] // make sure we have whole words
        >   assignment
        ;

    auto const assignment_def =
            identifier
        >   '='
        >   expression
        >   ';'
        ;

    BOOST_SPIRIT_DEFINE(
        statement_list = statement_list_def
      , identifier = identifier_def
      , variable_declaration = variable_declaration_def
      , assignment = assignment_def
    );
}}

namespace client
{
    parser::statement_list_type statement()
    {
        return parser::statement_list;
    }
}

#endif
