/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[reference_karma_includes
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <iostream>
#include <string>
//]

//[reference_karma_test
template <typename G>
void test_generator(char const* expected, G const& g)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g) && str == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_test_attr
template <typename G, typename T>
void test_generator_attr(char const* expected, G const& g, T const& attr)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g, attr) && str == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

int
main()
{   
    {
        //[reference_karma_using_declarations_sequence
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_sequence
        test_generator_attr("1.0,2.0", double_ << ',' << double_, std::make_pair(1.0, 2.0));
        //]
    }

    {
        //[reference_karma_using_declarations_alternative
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::string;
        //]

        //[reference_karma_alternative
        boost::variant<std::string, double> v(1.0);
        test_generator_attr("1.0", string | double_, v);
        //]
    }
    return 0;
}
