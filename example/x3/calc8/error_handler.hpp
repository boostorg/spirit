/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_ERROR_HANDLER_HPP)
#define BOOST_SPIRIT_X3_CALC8_ERROR_HANDLER_HPP

#include <boost/spirit/home/x3.hpp>

namespace client { namespace calculator_grammar
{
    ////////////////////////////////////////////////////////////////////////////
    //  Our error handler
    ////////////////////////////////////////////////////////////////////////////
    using x3::error_handler_result::fail;
    BOOST_SPIRIT_ONERROR(expression, fail)
    {
        std::cout
            << "Error! Expecting: "
            << x.what_
            << " here: \""
            << std::string(x.first, x.last)
            << "\""
            << std::endl
            ;
        return fail;
    }
}}

#endif
