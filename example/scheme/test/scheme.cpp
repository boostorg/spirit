/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include "../input/sexpr.hpp"
#include "../input/parse_sexpr_impl.hpp"
#include "../scheme_interpreter.hpp"
#include "../scheme_compiler.hpp"
#include <iostream>
#include <fstream>

inline std::ostream& println(std::ostream& out, scheme::utree const& val)
{
    out << val << std::endl;
    return out;
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    { // testing the c++ side

        using scheme::plus;
        using scheme::_1;
        using scheme::_2;

        std::cout << "result: " << plus(11, 22, 33)() << std::endl;
        std::cout << "result: " << plus(11, 22, _1)(33) << std::endl;
        std::cout << "result: " << plus(11, _1, _2)(22, 33) << std::endl;
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
    if (scheme::input::parse_sexpr_list(in, program))
    {
        std::cout << "success: " << std::endl;
        scheme::compiler_environment env;
        scheme::build_basic_environment(env);
        scheme::actor_list flist;
        compile_all(program, env, flist);
        BOOST_FOREACH(scheme::actor const& f, flist)
        {
            std::cout << " result: " << f() << std::endl;
        }
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}


