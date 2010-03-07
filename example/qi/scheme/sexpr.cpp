/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "sexpr.hpp"

int main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::qi;
 
    char const* first = "\"this is a\\n hmmm... \\u20AC string\"";
    char const* last  = first + std::strlen(first);
 
    scheme::string<char const*> p;
 
    std::string result;
    if (parse(first, last, p, result))
        std::cout << "success: '" << result << '\'' << std::endl;
    else
        std::cout << "parse error" << std::endl;
 
    return 0;
}


