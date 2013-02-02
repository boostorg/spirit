/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::x3::ascii;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::expect;
    using spirit_test::test;
    using boost::spirit::x3::expectation_failure;

    {
        try
        {
            BOOST_TEST((test("aa", char_ >> expect[char_])));
            BOOST_TEST((test("aaa", char_ >> expect[char_ >> char_('a')])));
            BOOST_TEST((test("xi", char_('x') >> expect[char_('i')])));
            BOOST_TEST((!test("xi", char_('y') >> expect[char_('o')]))); // should not throw!
            BOOST_TEST((test("xin", char_('x') >> expect[char_('i') >> char_('n')])));
            BOOST_TEST((!test("xi", char_('x') >> expect[char_('o')])));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.what_;
            std::cout << " got: \"" << x.first << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((test("aa", char_ > char_)));
            BOOST_TEST((test("aaa", char_ > char_ > char_('a'))));
            BOOST_TEST((test("xi", char_('x') > char_('i'))));
            BOOST_TEST((!test("xi", char_('y') > char_('o')))); // should not throw!
            BOOST_TEST((test("xin", char_('x') > char_('i') > char_('n'))));
            BOOST_TEST((!test("xi", char_('x') > char_('o'))));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.what_;
            std::cout << " got: \"" << x.first << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((test(" a a", char_ > char_, space)));
            BOOST_TEST((test(" x i", char_('x') > char_('i'), space)));
            BOOST_TEST((!test(" x i", char_('x') > char_('o'), space)));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.what_;
            std::cout << " got: \"" << x.first << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((test("bar", expect[lit("foo")])));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.what_;
            std::cout << " got: \"" << x.first << '"' << std::endl;
        }
    }

    //~ {
        //~ try
        //~ {
            //~ BOOST_TEST((test("aA", no_case[char_('a') > 'a'])));
            //~ BOOST_TEST((test("BEGIN END", no_case[lit("begin") > "end"], space)));
            //~ BOOST_TEST((!test("BEGIN END", no_case[lit("begin") > "nend"], space)));
        //~ }
        //~ catch (expectation_failure<char const*> const& x)
        //~ {
            //~ std::cout << "expected: " << x.what_;
            //~ std::cout << " got: \"" << x.first << '"' << std::endl;
        //~ }
    //~ }

    //~ {
        //~ using boost::spirit::x3::rule;
        //~ using boost::spirit::x3::eps;
        //~ rule<const wchar_t*, void(int)> r;
        //~ r = eps > eps(_r1);
    //~ }

    { // test various what results

        //~ using boost::spirit::compile;
        //~ BOOST_TEST((x3::what(compile<x3::domain>('a')) == "'a'"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>("hello")) == "\"hello\""));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::space)) == "space"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::bool_)) == "boolean"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::true_)) == "true"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::false_)) == "false"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::int_)) == "integer"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::hex)) == "hexadecimal integer"));
        //~ BOOST_TEST((x3::what(compile<x3::domain>(x3::double_)) == "real number"));
    }

    return boost::report_errors();
}

