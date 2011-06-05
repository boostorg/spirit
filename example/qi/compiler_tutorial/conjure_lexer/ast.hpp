/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_AST_HPP)
#define BOOST_SPIRIT_CONJURE_AST_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

#include "token_ids.hpp"

namespace client { namespace ast
{
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct tagged
    {
        int id; // Used to annotate the AST with the iterator position.
                // This id is used as a key to a map<int, Iterator>
                // (not really part of the AST.)
    };

    struct nil {};
    struct unary;
    struct function_call;
    struct expression;

    struct identifier : tagged
    {
        identifier(std::string const& name = "") : name(name) {}
        std::string name;
    };

    typedef boost::variant<
            nil
          , bool
          , unsigned int
          , identifier
          , boost::recursive_wrapper<unary>
          , boost::recursive_wrapper<function_call>
          , boost::recursive_wrapper<expression>
        >
    operand;

    #define OP(id)              (id + op_operator)
    #define OP_EX(id, mask)     (id + (mask | op_operator))

    enum optoken
    {
        // pseudo tags
        op_operator = lexer::ID_OP_OPERATOR,
        op_binary = lexer::ID_OP_BINARY,
        op_unary = lexer::ID_OP_UNARY,
        op_mask = (op_operator | op_unary | op_binary),

        // precedence 1
        op_comma = OP(0),

        // precedence 2
        op_assign = OP(1),
        op_plus_assign = OP(2),
        op_minus_assign = OP(3),
        op_times_assign = OP(4),
        op_divide_assign = OP(5),
        op_mod_assign = OP(6),
        op_bit_and_assign = OP(7),
        op_bit_xor_assign = OP(8),
        op_bitor_assign = OP(9),
        op_shift_left_assign = OP(10),
        op_shift_right_assign = OP(11),

        // precedence 3
        op_logical_or = OP_EX(12, op_binary),

        // precedence 4
        op_logical_and = OP_EX(13, op_binary),

        // precedence 5
        op_bit_or = OP_EX(14, op_binary),

        // precedence 6
        op_bit_xor = OP_EX(15, op_binary),

        // precedence 7
        op_bit_and = OP_EX(16, op_binary),

        // precedence 8
        op_equal = OP_EX(17, op_binary),
        op_not_equal = OP_EX(18, op_binary),

        // precedence 9
        op_less = OP_EX(19, op_binary),
        op_less_equal = OP_EX(20, op_binary),
        op_greater = OP_EX(21, op_binary),
        op_greater_equal = OP_EX(22, op_binary),

        // precedence 10
        op_shift_left = OP_EX(23, op_binary),
        op_shift_right = OP_EX(24, op_binary),

        // precedence 11
        op_plus = OP_EX(25, op_binary|op_unary),
        op_minus = OP_EX(26, op_binary|op_unary),

        // precedence 12
        op_times = OP_EX(27, op_binary),
        op_divide = OP_EX(28, op_binary),
        op_mod = OP_EX(29, op_binary),

        // precedence 13
        op_positive = OP_EX(30, op_unary),
        op_negative = OP_EX(31, op_unary),
        op_pre_incr = OP_EX(32, op_unary),
        op_pre_decr = OP_EX(33, op_unary),
        op_compl = OP_EX(34, op_unary),
        op_not = OP_EX(35, op_unary),

        // precedence 14
        op_post_incr = OP(36),
        op_post_decr = OP(37)
    };

    struct unary
    {
        optoken operator_;
        operand operand_;
    };

    struct operation
    {
        optoken operator_;
        operand operand_;
    };

    struct function_call
    {
        identifier function_name;
        std::list<expression> args;
    };

    struct expression
    {
        operand first;
        std::list<operation> rest;
    };

    struct assignment
    {
        identifier lhs;
        expression rhs;
    };

    struct variable_declaration
    {
        identifier lhs;
        boost::optional<expression> rhs;
    };

    struct if_statement;
    struct while_statement;
    struct statement_list;
    struct return_statement;

    typedef boost::variant<
            variable_declaration
          , assignment
          , boost::recursive_wrapper<if_statement>
          , boost::recursive_wrapper<while_statement>
          , boost::recursive_wrapper<return_statement>
          , boost::recursive_wrapper<statement_list>
        >
    statement;

    struct statement_list : std::list<statement> {};

    struct if_statement
    {
        expression condition;
        statement then;
        boost::optional<statement> else_;
    };

    struct while_statement
    {
        expression condition;
        statement body;
    };

    struct return_statement : tagged
    {
        boost::optional<expression> expr;
    };

    struct function
    {
        std::string return_type;
        identifier function_name;
        std::list<identifier> args;
        statement_list body;
    };

    typedef std::list<function> function_list;

    // print functions for debugging
    inline std::ostream& operator<<(std::ostream& out, nil)
    {
        out << "nil"; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, identifier const& id)
    {
        out << id.name; return out;
    }
}}

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::unary,
    (client::ast::optoken, operator_)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::operation,
    (client::ast::optoken, operator_)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::function_call,
    (client::ast::identifier, function_name)
    (std::list<client::ast::expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::expression,
    (client::ast::operand, first)
    (std::list<client::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::variable_declaration,
    (client::ast::identifier, lhs)
    (boost::optional<client::ast::expression>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::assignment,
    (client::ast::identifier, lhs)
    (client::ast::expression, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::if_statement,
    (client::ast::expression, condition)
    (client::ast::statement, then)
    (boost::optional<client::ast::statement>, else_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::while_statement,
    (client::ast::expression, condition)
    (client::ast::statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::return_statement,
    (boost::optional<client::ast::expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::function,
    (std::string, return_type)
    (client::ast::identifier, function_name)
    (std::list<client::ast::identifier>, args)
    (client::ast::statement_list, body)
)

#endif
