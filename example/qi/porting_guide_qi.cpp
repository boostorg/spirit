/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[porting_guide_qi_includes
#include <boost/spirit/include/qi.hpp>
#include <iostreams>
#include <string>
#include <algorithm>
//]

//[porting_guide_qi_namespace
using namespace boost::spirit;
//]

int main()
{
    {
        //[porting_guide_qi_parse
        std::string input("1,1");
        std::string::iterator it = input.begin();
        bool result = qi::parse(it, input.end(), int_);

        if (result) 
            std::cout << "successful match!\n";

        if (it == input.end()) 
            std::cout << "full match!\n";
        else
            std::cout << "stopped at: " << std::string(it, input.end()) << "\n";

        std::cout << "matched length: " << std::distance(input.begin(), it) << "\n";
        //]
    }

    {
        //[porting_guide_qi_phrase_parse
        std::string input(" 1, 1");
        std::string::iterator it = input.begin();
        bool result = qi::phrase_parse(it, input.end(), int_, ascii::space);

        if (result) 
            std::cout << "successful match!\n";

        if (it == input.end()) 
            std::cout << "full match!\n";
        else
            std::cout << "stopped at: " << std::string(it, input.end()) << "\n";

        std::cout << "matched length: " << std::distance(input.begin(), it) << "\n";
        //]
    }
    return 0;
}

