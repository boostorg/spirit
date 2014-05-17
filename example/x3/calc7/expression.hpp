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
    namespace calculator_grammar
    {
        typedef x3::identity<class expression> expression_id;
        typedef x3::identity<class term> term_id;
        typedef x3::identity<class factor> factor_id;
        
        typedef x3::rule<class expression, ast::expression> const expression_type;
        typedef x3::rule<class term, ast::expression> const term_type;
        typedef x3::rule<class factor, ast::operand> const factor_type;
        
        BOOST_SPIRIT_DECLARE(expression_type);
    }
}

#endif
