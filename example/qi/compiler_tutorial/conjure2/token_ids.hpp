/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_TOKEN_IDS_HPP)
#define BOOST_SPIRIT_CONJURE_TOKEN_IDS_HPP

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_lexertl_position_token.hpp>

namespace client { namespace lexer 
{
    namespace lex = boost::spirit::lex;

    enum tokenids
    {
        ID_OP_OPERATOR = 0x10000,
        ID_OP_BINARY = 0x20000,
        ID_OP_UNARY = 0x40000,

        // the token ids (added values below) have to correspond to the 
        // sequence numbers used in the ast::optoken enumeration
        ID_OP_LOGICAL_OR = (ID_OP_OPERATOR | ID_OP_BINARY) + 12,
        ID_OP_LOGICAL_AND = (ID_OP_OPERATOR | ID_OP_BINARY) + 13,
        ID_OP_EQUAL = (ID_OP_OPERATOR | ID_OP_BINARY) + 17,
        ID_OP_NOT_EQUAL = (ID_OP_OPERATOR | ID_OP_BINARY) + 18,
        ID_OP_LESS = (ID_OP_OPERATOR | ID_OP_BINARY) + 19,
        ID_OP_LESS_EQUAL = (ID_OP_OPERATOR | ID_OP_BINARY) + 20,
        ID_OP_GREATER = (ID_OP_OPERATOR | ID_OP_BINARY) + 21,
        ID_OP_GREATER_EQUAL = (ID_OP_OPERATOR | ID_OP_BINARY) + 22,
        ID_OP_PLUS = (ID_OP_OPERATOR | ID_OP_UNARY | ID_OP_BINARY) + 25,
        ID_OP_MINUS = (ID_OP_OPERATOR | ID_OP_UNARY | ID_OP_BINARY) + 26,
        ID_OP_TIMES = (ID_OP_OPERATOR | ID_OP_BINARY) + 27,
        ID_OP_DIVIDE = (ID_OP_OPERATOR | ID_OP_BINARY) + 28,
        ID_OP_NOT = (ID_OP_OPERATOR | ID_OP_UNARY) + 35,

        ID_IDENTIFIER = ID_OP_OPERATOR + 100,
        ID_COMMENT,
        ID_WHITESPACE,
        ID_VOID_KWD,
        ID_INT_KWD,
        ID_IF_KWD,
        ID_ELSE_KWD,
        ID_WHILE_KWD,
        ID_RETURN_KWD,
        ID_UINT,
        ID_BOOL
    };
}}

#endif


