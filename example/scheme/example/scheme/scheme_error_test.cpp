/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include <input/sexpr.hpp>
#include <input/parse_sexpr_impl.hpp>
#include <scheme/compiler.hpp>
#include <utree/io.hpp>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename = filename = argv[1];
    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "scheme_error.scm not found" << std::endl;
        return -1;
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
            std::cerr << "unexpected characters in file" << std::endl;
            return -1;
        }
    }

    scheme::interpreter factorial(in, filename);
    return 0;
}


