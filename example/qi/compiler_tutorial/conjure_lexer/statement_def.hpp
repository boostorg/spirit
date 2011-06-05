/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "token_ids.hpp"
#include "statement.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"

namespace client { namespace parser
{
    template <typename Iterator, typename Lexer>
    statement<Iterator, Lexer>::statement(
            error_handler<typename Lexer::base_iterator_type, Iterator>& error_handler
          , Lexer const& l)
      : statement::base_type(statement_list), expr(error_handler, l)
    {
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        qi::_val_type _val;
        qi::raw_token_type raw_token;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using boost::phoenix::function;

        typedef client::error_handler<typename Lexer::base_iterator_type, Iterator>
            error_handler_type;
        typedef function<error_handler_type> error_handler_function;
        typedef function<client::annotation<Iterator> > annotation_function;

        statement_list =
            +statement_
            ;

        statement_ =
                variable_declaration
            |   assignment
            |   compound_statement
            |   if_statement
            |   while_statement
            |   return_statement
            ;

        variable_declaration =
                raw_token(lexer::ID_INT_KWD) 
            >   expr.identifier
            >  -('=' > expr)
            >   ';'
            ;

        assignment =
                expr.identifier
            >   '='
            >   expr
            >   ';'
            ;

        if_statement =
                raw_token(lexer::ID_IF_KWD)
            >   '('
            >   expr
            >   ')'
            >   statement_
            >
               -(
                    raw_token(lexer::ID_ELSE_KWD)
                >   statement_
                )
            ;

        while_statement =
                raw_token(lexer::ID_WHILE_KWD)
            >   '('
            >   expr
            >   ')'
            >   statement_
            ;

        compound_statement =
            '{' >> -statement_list >> '}'
            ;

        return_statement =
                raw_token(lexer::ID_RETURN_KWD)
            >  -expr
            >   ';'
            ;

        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (statement_list)
            (variable_declaration)
            (assignment)
        );

        // Error handling: on error in statement_list, call error_handler.
        on_error<fail>(statement_list,
            error_handler_function(error_handler)(
                "Error! Expecting ", _4, _3));

        // Annotation: on success in assignment, call annotation.
        on_success(assignment,
            annotation_function(error_handler.iters)(_val, _1));
        on_success(return_statement,
            annotation_function(error_handler.iters)(_val, _1));
    }
}}


