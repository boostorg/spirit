/*=============================================================================
    Copyright (c) 2022 Denis Mikhailov
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <string>
#include <vector>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

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
using error_handler_type = boost::spirit::x3::error_handler<iterator_type>;

std::string input = "Hello world!!";

void test() {
  {
    error_handler_type error_handler{input.begin(), input.end(), std::cerr};
    employee ast;
    error_handler.tag(ast, input.begin() + 6, input.begin() + 11);
    // const auto r = error_handler.position_of(ast);
    // BOOST_TEST(r.empty());
    BOOST_TEST(error_handler.get_position_cache().get_positions().empty());
  }
}

void test_with_position_tagged() {
  {
    error_handler_type error_handler{input.begin(), input.end(), std::cerr};
    employee_with_position_tagged ast;
    error_handler.tag(ast, input.begin() + 6, input.begin() + 11);
    const auto r = error_handler.position_of(ast);
    const auto str = std::string{r.begin(), r.end()};
    BOOST_TEST(!r.empty());
    BOOST_TEST(str == "world");
  }
  {
    std::stringstream stream;
    error_handler_type error_handler{input.begin(), input.end(), stream};
    employee_with_position_tagged ast;
    error_handler.tag(ast, input.begin() + 6, input.begin() + 11);
    error_handler(ast, "You were wrong");
    BOOST_TEST_EQ(stream.str(), "In line 1:\nYou were wrong\nHello world!!\n      ~~~~~ <<-- Here\n");
  }
}

int
main()
{
  test();
  test_with_position_tagged();
  return boost::report_errors();
}

