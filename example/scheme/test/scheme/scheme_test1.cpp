/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include <input/parse_sexpr_impl.hpp>
#include <scheme/compiler.hpp>
#include <utree/io.hpp>
#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    { // testing the c++ side

        using scheme::if_;
        using scheme::plus;
        using scheme::times;
        using scheme::minus;
        using scheme::lte;
        using scheme::_1;
        using scheme::_2;
        using scheme::lambda;

        std::cout << "result: " << plus(11, 22, 33)         () << std::endl;
        std::cout << "result: " << plus(11, 22, _1)         (33) << std::endl;
        std::cout << "result: " << plus(11, _1, _2)         (22, 33) << std::endl;
        std::cout << "result: " << plus(11, plus(_1, _2))   (22, 33) << std::endl;

        lambda factorial;
        factorial = if_(lte(_1, 0), 1, times(_1, factorial(minus(_1, 1))));

        std::cout << "result: " << factorial(_1)            (10) << std::endl;
    }

    return boost::report_errors();
}


