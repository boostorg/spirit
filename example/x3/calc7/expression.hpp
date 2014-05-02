/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_EXPRESSION_HPP)
#define BOOST_SPIRIT_CALC7_X3_EXPRESSION_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace client { namespace parser
{
    namespace x3 = boost::spirit::x3;

    ///////////////////////////////////////////////////////////////////////////////
    // rule IDs
    ///////////////////////////////////////////////////////////////////////////////
    typedef x3::identity<class expression> expression_id;
    typedef x3::identity<class additive_expr> additive_expr_id;
    typedef x3::identity<class multiplicative_expr> multiplicative_expr_id;
    typedef x3::identity<class unary_expr> unary_expr_id;
    typedef x3::identity<class primary_expr> primary_expr_id;
    typedef x3::identity<class identifier> identifier_id;
}}

#endif
