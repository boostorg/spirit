/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[reference_test
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <iostream>
#include <string>

template <typename P>
void test_parser(char const* input, P const& p)
{
    std::string s(input);
    std::string::iterator f(s.begin());
    if (boost::spirit::qi::parse(f, s.end(), p))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

int
main()
{
    //[reference_using_declarations
    using boost::spirit::qi::parse;
    using boost::spirit::qi::lit;
    using boost::spirit::ascii::string;
    //]
    
    {
        //[reference_string1_literals
        test_parser("boost", "boost");          // plain literal
        test_parser("boost", lit("boost"));     // explicit literal
        test_parser("boost", string("boost"));  // ascii::string
        //]
    }
    
    {
        //[reference_string1_std_string
        std::string s("boost");
        test_parser("boost", s);            // direct
        test_parser("boost", lit(s));       // explicit
        test_parser("boost", string(s));    // ascii::string
        //]
    }
    
    {
        //[reference_string1_phoenix
        namespace phx = boost::phoenix;        
        test_parser("boost", phx::val("boost"));            // direct
        test_parser("boost", lit(phx::val("boost")));       // explicit
        test_parser("boost", string(phx::val("boost")));    // ascii::string
        //]
    }

    return 0;
}
