/*=============================================================================
    Copyright (c) 2001-2016 Joel de Guzman
    Copyright (c) 2018      Nikita Kniazev

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <string>
#include <iostream>
#include "test.hpp"
#include "init_checker.hpp"

namespace x3 = boost::spirit::x3;

struct none {};

using variant = x3::variant<
        none
      , bool
      , std::string
      , int
      , double
      , init_checker
    >;

struct ast : variant
{
    using variant::variant;
    using variant::operator=;

    ast(char const* s)
      : variant(std::string{s})
    {}

    ast& operator=(char const* s)
    {
        variant::operator=(std::string{s});
        return *this;
    }
};

int
main()
{
    using namespace std::string_literals;

    { // default constructor
        variant v;
        (void) boost::get<none>(v);
    }
    { // aggregate initialization
        variant v{ "asd"s };
        BOOST_TEST(boost::get<std::string>(v) == "asd");
    }
    { // copy initialization
        variant v = "asd"s;
        BOOST_TEST(boost::get<std::string>(v) == "asd");
    }
    { // aggregate copy initialization
        variant v = { "asd"s };
        BOOST_TEST(boost::get<std::string>(v) == "asd");
    }
    { // check the checker :-)
        init_checker checker;
        BOOST_TEST(!checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 0);
        BOOST_TEST_EQ(checker.how_initialized, how_initialized::default_constructed);
    }
    { // check copy constructor
        init_checker checker;
        variant v1(checker);
        BOOST_TEST_EQ(boost::get<init_checker>(v1).copy_count, 0);
        variant v2(v1);
        BOOST_TEST_EQ(boost::get<init_checker>(v1).copy_count, 1);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).how_initialized, how_initialized::copy_constructed);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).copy_count, 0);
    }
    { // check move constructor
        init_checker checker;
        variant v1(checker);
        BOOST_TEST_EQ(boost::get<init_checker>(v1).moved_out, false);
        variant v2(std::move(v1));
        BOOST_TEST_EQ(boost::get<init_checker>(v1).moved_out, true);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).how_initialized, how_initialized::move_constructed);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).copy_count, 0);
    }
    {
        // check copy initialization
        init_checker checker;
        variant v1, v2 = checker;
        BOOST_TEST_EQ(checker.copy_count, 1);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).how_initialized, how_initialized::copy_constructed);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).copy_count, 0);
        // check copy assignment
        v1 = v2;
        BOOST_TEST_EQ(boost::get<init_checker>(v1).how_initialized, how_initialized::copy_constructed);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).copy_count, 1);
    }
    { 
        // check move initialization
        init_checker checker;
        variant v1, v2 = std::move(checker);
        BOOST_TEST(checker.moved_out);
        BOOST_TEST(!boost::get<init_checker>(v2).moved_out);
        BOOST_TEST_EQ(boost::get<init_checker>(v2).how_initialized, how_initialized::move_constructed);
        // check move assignment
        v1 = std::move(v2);
        BOOST_TEST(boost::get<init_checker>(v2).moved_out);
        BOOST_TEST_EQ(boost::get<init_checker>(v1).how_initialized, how_initialized::move_constructed);
    }
    { // lvalue construct
        init_checker checker;
        variant v(checker);
        BOOST_TEST(!checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 1);
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::copy_constructed);
    }
    { // rvalue construct
        init_checker checker;
        variant v(std::move(checker));
        BOOST_TEST(checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 0);
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::move_constructed);
    }
    { // lvalue assign
        init_checker checker;
        variant v = none{};
        v = checker;
        BOOST_TEST(!checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 1);
#if 0 // boost::variant assign operator assigns through constructing a temporary variant
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::copy_constructed);
#endif
        // additional tests
        v = checker;
        BOOST_TEST(!checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 2);
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::copy_assigned);
        v = std::move(checker);
        BOOST_TEST(checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 2);
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::move_assigned);
    }
    { // rvalue assign
        init_checker checker;
        variant v = none{};
        v = std::move(checker);
        BOOST_TEST(checker.moved_out);
        BOOST_TEST_EQ(checker.copy_count, 0);
        BOOST_TEST_EQ(boost::get<init_checker>(v).how_initialized, how_initialized::move_constructed);
    }
    { // swap
        variant v1 = 123, v2 = "asd"s;
        boost::swap(v1, v2);
        BOOST_TEST(boost::get<std::string>(v1) == "asd");
        BOOST_TEST(boost::get<int>(v2) == 123);
    }

    {
        ast v{123};
        BOOST_TEST(boost::get<int>(v) == 123);

        v = "test";
        BOOST_TEST(boost::get<std::string>(v) == "test");

        v = true;
        BOOST_TEST(boost::get<bool>(v) == true);
    }
    return boost::report_errors();
}
