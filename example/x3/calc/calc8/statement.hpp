/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_STATEMENT_HPP)
#define BOOST_SPIRIT_X3_CALC8_STATEMENT_HPP

#include "ast.hpp"

#include <boost/spirit/home/x3.hpp>

namespace client
{
    namespace parser
    {
        namespace x3 = boost::spirit::x3;
        x3::rule<struct statement_r, ast::statement_list> const statement = "statement";
        BOOST_SPIRIT_DECLARE(decltype(statement));
    }
    
    using parser::statement;
}

#endif
