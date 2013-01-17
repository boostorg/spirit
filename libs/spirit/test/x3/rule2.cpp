/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// this file deliberately contains non-ascii characters
// boostinspect:noascii

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>

#include <string>
#include <cstring>
#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    using namespace boost::spirit::x3::ascii;
    //~ using boost::spirit::x3::locals;
    using boost::spirit::x3::rule;
    using boost::spirit::x3::int_;
    //~ using boost::spirit::x3::uint_;
    //~ using boost::spirit::x3::fail;
    //~ using boost::spirit::x3::on_error;
    //~ using boost::spirit::x3::debug;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::unused_type;

    //~ { // test unassigned rule

        //~ rule<char const*> a;
        //~ BOOST_TEST(!test("x", a));
    //~ }

    //~ { // alias tests

        //~ rule<char const*> a, b, c, d, start;

        //~ a = 'a';
        //~ b = 'b';
        //~ c = 'c';
        //~ d = start.alias(); // d will always track start

        //~ start = *(a | b | c);
        //~ BOOST_TEST(test("abcabcacb", d));

        //~ start = (a | b) >> (start | b);
        //~ BOOST_TEST(test("aaaabababaaabbb", d));
    //~ }

    //~ { // copy tests

        //~ rule<char const*> a, b, c, start;

        //~ a = 'a';
        //~ b = 'b';
        //~ c = 'c';

        //~ // The FF is the dynamic equivalent of start = *(a | b | c);
        //~ start = a;
        //~ start = start.copy() | b;
        //~ start = start.copy() | c;
        //~ start = *(start.copy());

        //~ BOOST_TEST(test("abcabcacb", start));

        //~ // The FF is the dynamic equivalent of start = (a | b) >> (start | b);
        //~ start = b;
        //~ start = a | start.copy();
        //~ start = start.copy() >> (start | b);

        //~ BOOST_TEST(test("aaaabababaaabbb", start));
        //~ BOOST_TEST(test("aaaabababaaabba", start, false));
    //~ }

    { // context tests

        char ch;
        auto a = rule<class a, char>() = alpha;

        //~ BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        //~ BOOST_TEST(ch == 'x');

        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');
    }

    { // auto rules tests

        char ch = '\0';
        auto a = rule<class a, char>() = alpha;

        //~ BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        //~ BOOST_TEST(ch == 'x');
        //~ ch = '\0';
        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');

        //~ a = alpha;    // test deduced auto rule behavior
        //~ ch = '\0';
        //~ BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        //~ BOOST_TEST(ch == 'x');
        //~ ch = '\0';
        //~ BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        //~ BOOST_TEST(ch == 'z');
    }

/*

    { // auto rules tests: allow stl containers as attributes to
      // sequences (in cases where attributes of the elements
      // are convertible to the value_type of the container or if
      // the element itself is an stl container with value_type
      // that is convertible to the value_type of the attribute).

        std::string s;
        rule<char const*, std::string()> r;
        r %= char_ >> *(',' >> char_);

        BOOST_TEST(test("a,b,c,d,e,f", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r = char_ >> *(',' >> char_);    // test deduced auto rule behavior
        s.clear();
        BOOST_TEST(test("a,b,c,d,e,f", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r %= char_ >> char_ >> char_ >> char_ >> char_ >> char_;
        s.clear();
        BOOST_TEST(test("abcdef", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r = char_ >> char_ >> char_ >> char_ >> char_ >> char_;
        s.clear();
        BOOST_TEST(test("abcdef", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");
    }
*/
    return boost::report_errors();
}

