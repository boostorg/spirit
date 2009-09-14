//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_action.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::ascii;
    namespace fusion = boost::fusion;

    {
        std::string s1("aaaa");
        BOOST_TEST(test("aaaa", +char_, s1));
        BOOST_TEST(test_delimited("a a a a ", +char_, s1, ' '));

        std::string s2("a");
        BOOST_TEST(test("a", +char_, s2));
        BOOST_TEST(test_delimited("a ", +char_, s2, ' '));

        std::string s3("");
        BOOST_TEST(!test("", +char_, s2));
        BOOST_TEST(!test_delimited("", +char_, s3, ' '));
    }

    {
        using namespace boost::assign;

        std::vector<char> v;
        v += 'a', 'b', 'c';

        BOOST_TEST(test("abc", +char_, v));
        BOOST_TEST(test_delimited("a b c ", +char_, v, ' '));
    }

    {
        using namespace boost::assign;

        std::vector<int> v;
        v += 10, 20, 30;

        BOOST_TEST(test("102030", +int_, v));
        BOOST_TEST(test_delimited("10, 20, 30, ", +int_, v, lit(", ")));

        BOOST_TEST(test("10,20,30,", +(int_ << ','), v));
        BOOST_TEST(test_delimited("10 , 20 , 30 , ", +(int_ << ','), v, lit(" ")));
 
        fusion::vector<char, char> cc ('a', 'c');
        BOOST_TEST(test("ac", char_ << !+(lit(' ') << ',') << char_, cc));
        BOOST_TEST(test_delimited("a c ", 
            char_ << !+(lit(' ') << ',') << char_, cc, " "));
    }

    { // actions
        using namespace boost::assign;
        namespace phx = boost::phoenix;

        std::vector<char> v;
        v += 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h';

        BOOST_TEST(test("abcdefgh", (+char_)[_1 = phx::ref(v)]));
        BOOST_TEST(test_delimited("a b c d e f g h ", 
            (+char_ )[_1 = phx::ref(v)], space));
    }

    return boost::report_errors();
}

