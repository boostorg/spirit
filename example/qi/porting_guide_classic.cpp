/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[porting_guide_classic_includes
#include <boost/spirit/include/classic.hpp>
#include <iostreams>
#include <string>
//]

//[porting_guide_classic_namespace
using namespace boost::spirit::classic;
//]

int main()
{
    {
        //[porting_guide_classic_parse
        std::string input("1,1");
        parse_info<std::string::iterator> pi = parse(input.begin(), input.end(), int_p);

        if (pi.hit) 
            std::cout << "successful match!\n";

        if (pi.full) 
            std::cout << "full match!\n";
        else
            std::cout << "stopped at: " << std::string(pi.stop, input.end()) << "\n";

        std::cout << "matched length: " << pi.length << "\n";
        //]
    }

    {
        //[porting_guide_classic_phrase_parse
        std::string input(" 1, 1");
        parse_info<std::string::iterator> pi = parse(input.begin(), input.end(), int_p, space_p);

        if (pi.hit) 
            std::cout << "successful match!\n";

        if (pi.full) 
            std::cout << "full match!\n";
        else
            std::cout << "stopped at: " << std::string(pi.stop, input.end()) << "\n";

        std::cout << "matched length: " << pi.length << "\n";
        //]
    }
    return 0;
}

