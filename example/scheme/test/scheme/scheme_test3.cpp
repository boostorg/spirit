/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include "../../input/sexpr.hpp"
#include "../../input/parse_sexpr_impl.hpp"
#include "../../scheme_compiler.hpp"
#include "../../utree_io.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    using scheme::interpreter;
    using scheme::utree;

    utree src = "(define (factorial n) (if (<= n 0) 1 (* n (factorial (- n 1)))))";
    scheme::interpreter factorial(src);
    std::cout << factorial(10) << std::endl;

    return 0;
}


