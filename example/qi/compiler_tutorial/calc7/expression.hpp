/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_EXPRESSION_HPP)
#define BOOST_SPIRIT_CALC7_EXPRESSION_HPP

///////////////////////////////////////////////////////////////////////////////
// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
//#define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include "ast.hpp"
#include <map>

namespace client
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////////
    //  The expression grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct expression : qi::grammar<Iterator, ast::program(), ascii::space_type>
    {
        expression();

        qi::rule<Iterator, ast::program(), ascii::space_type> expr;
        qi::rule<Iterator, ast::program(), ascii::space_type> additive_expr;
        qi::rule<Iterator, ast::program(), ascii::space_type> multiplicative_expr;
        qi::rule<Iterator, ast::operand(), ascii::space_type> unary_expr;
        qi::rule<Iterator, ast::operand(), ascii::space_type> primary_expr;

        std::map<int, Iterator> iters;
        int current_id;
    };
}

#endif


