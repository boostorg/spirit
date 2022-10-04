/*=============================================================================
    Copyright (c) 2022 Denis Mikhailov
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <string>
#include <vector>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/core/lightweight_test.hpp>

#include <string>
#include <iostream>

struct employee
{
    int age;
    double salary;
};

struct employee_with_position_tagged : boost::spirit::x3::position_tagged
{
    int age;
    double salary;
};

using iterator_type = std::string::const_iterator;
using position_cache = boost::spirit::x3::position_cache<std::vector<iterator_type>>;

std::string input = "Hello world!!";

int
main()
{
    position_cache positions{input.begin(), input.end()};

    {
      employee ast;
      positions.annotate(ast, input.begin() + 6, input.begin() + 11);
      const auto r = positions.position_of(ast);
      BOOST_TEST(r.empty());
    }

    {
      employee_with_position_tagged ast;
      positions.annotate(ast, input.begin() + 6, input.begin() + 11);
      const auto r = positions.position_of(ast);
      const auto str = std::string{r.begin(), r.end()};
      BOOST_TEST(!r.empty());
      BOOST_TEST(str == "world");
    }

    return boost::report_errors();
}