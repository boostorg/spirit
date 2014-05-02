/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_EXPRESSION_HPP)
#define BOOST_SPIRIT_X3_CALC7_EXPRESSION_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace client
{
    namespace x3 = boost::spirit::x3;

    ///////////////////////////////////////////////////////////////////////////////
    // rule IDs
    ///////////////////////////////////////////////////////////////////////////////
    typedef x3::identity<class expression> expression_id;
    typedef x3::identity<class term> term_id;
    typedef x3::identity<class factor> factor_id;

    template <typename Iterator, typename Skipper>
    x3::any_parser<Iterator, ast::expression, x3::context<x3::skipper_tag, Skipper const>>
    expression();

}

#endif
