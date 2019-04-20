/*=============================================================================
    Copyright (c) 2002-2018 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_MINIMAL_EMPLOYEE_HPP)
#define BOOST_SPIRIT_X3_MINIMAL_EMPLOYEE_HPP

#include "ast.hpp"

#include <boost/spirit/home/x3.hpp>

namespace client
{
    ///////////////////////////////////////////////////////////////////////////////
    //  Our employee parser declaration
    ///////////////////////////////////////////////////////////////////////////////
    namespace parser
    {
        namespace x3 = boost::spirit::x3;
        x3::rule<class employee_r, ast::employee> const employee = "employee";
        BOOST_SPIRIT_DECLARE(decltype(employee));
    }

    using parser::employee;
}

#endif
