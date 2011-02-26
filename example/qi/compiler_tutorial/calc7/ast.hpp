/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_AST_HPP)
#define BOOST_SPIRIT_CALC7_AST_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <list>

namespace client { namespace ast
{
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct nil {};
    struct signed_;
    struct program;
    struct tagged;

    typedef boost::variant<
            nil
          , unsigned int
          , boost::recursive_wrapper<signed_>
          , boost::recursive_wrapper<program>
          , boost::recursive_wrapper<tagged>
        >
    operand;

    struct signed_
    {
        char sign;
        operand operand_;
    };

    struct operation
    {
        char operator_;
        operand operand_;
    };

    struct program
    {
        operand first;
        std::list<operation> rest;
    };

    struct tagged
    {
        operand operand_;
        int id; // Used to annotate the iterator position. This
                // id is used as a key to a map<int, Iterator>
                // (not really part of the AST.)
    };
}}

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::signed_,
    (char, sign)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::operation,
    (char, operator_)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::program,
    (client::ast::operand, first)
    (std::list<client::ast::operation>, rest)
)

#endif
