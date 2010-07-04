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
int main(int argc, char **argv)
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

    char const* filename = NULL;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    // Ignore the BOM marking the beginning of a UTF-8 file in Windows
    char c = in.peek();
    if (c == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "Error: Unexpected characters from input file: "
                << filename << std::endl;
            return 1;
        }
    }

    scheme::utree program;
    BOOST_TEST(scheme::input::parse_sexpr_list(in, program, filename));

        scheme::actor_list::iterator i = flist.begin();

        std::cout << "the 1st is the define dbl:" << std::endl;
        std::cout << "(dbl 555): " << (*i++)(555) << std::endl;
        std::cout << "the 2nd is the define len:" << std::endl;
        std::cout << "len: " << (*i++)() << std::endl;
        std::cout << "the 3rd is a function call:" << std::endl;
        std::cout << "(dbl len): " << (*i++)() << std::endl;
        std::cout << "the 4th is the define factorial:" << std::endl;
        std::cout << "(factorial 5): " << (*i++)(5) << std::endl;
        std::cout << "the 5th is a function call:" << std::endl;
        std::cout << "(factorial 10): " << (*i++)() << std::endl;
        std::cout << "the 6th is the define fib:" << std::endl;
        std::cout << "(fib 5): " << (*i++)(5) << std::endl;
        std::cout << "the 7th is a function call:" << std::endl;
        std::cout << "(fib 10): " << (*i++)() << std::endl;
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}


