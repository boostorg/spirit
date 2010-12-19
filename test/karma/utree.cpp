// Copyright (c) 2001-2010 Hartmut Kaiser
// Copyright (c) 2001-2010 Joel de Guzman
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/mpl/print.hpp>

#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/karma.hpp>

#include <sstream>

#include "test.hpp"

int main()
{
    using spirit_test::test;
    using spirit_test::test_delimited;
    using boost::spirit::utree;
    using boost::spirit::utree_type;
    using boost::spirit::utf8_string_range;

    using boost::spirit::karma::char_;
    using boost::spirit::karma::bool_;
    using boost::spirit::karma::int_;
    using boost::spirit::karma::double_;
    using boost::spirit::karma::string;
    using boost::spirit::karma::space;
    using boost::spirit::karma::rule;

    typedef spirit_test::output_iterator<char>::type output_iterator;

    // primitive data types
    {
        utree ut('x');
        BOOST_TEST(test("x", char_, ut));

        ut = false;
        BOOST_TEST(test("false", bool_, ut));

        ut = 123;
        BOOST_TEST(test("123", int_, ut));

        ut = 123.45;
        BOOST_TEST(test("123.45", double_, ut));

        ut = "abc";
        BOOST_TEST(test("abc", string, ut));
    }

    // sequences
    {
        using boost::spirit::karma::digit;
        using boost::spirit::karma::repeat;

        utree ut;
        ut.push_back('x');
        ut.push_back('y');
        BOOST_TEST(test("xy", char_ << char_, ut));

        ut.clear();
        ut.push_back(123);
        ut.push_back(456);
        BOOST_TEST(test_delimited("123 456 ", int_ << int_, ut, space));

        ut.clear();
        ut.push_back(1.23);
        ut.push_back(4.56);
        BOOST_TEST(test_delimited("1.23 4.56 ", double_ << double_, ut, space));

        ut.clear();
        ut.push_back(1.23);
        ut.push_back("ab");
        BOOST_TEST(test("1.23ab", double_ << string, ut));

        ut.clear();

        rule<output_iterator, double()> r1 = double_;
        rule<output_iterator, utree()> r2 = double_;

        ut.push_back(1.23);
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("1.23ab", double_ << *char_, ut));
        BOOST_TEST(test("1.23ab", r1 << *char_, ut)); 
        BOOST_TEST(test("1.23ab", r2 << *char_, ut)); 

        ut.clear();
        ut.push_back('a');
        ut.push_back('b');
        ut.push_back(1.23);
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << double_, ut));
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << r1, ut));
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << r2, ut));
    }

    // kleene star
    {
        utree ut;
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("ab", *char_, ut));

        ut.clear();
        ut.push_back(123);
        ut.push_back(456);
        BOOST_TEST(test_delimited("123 456 ", *int_, ut, space));

        ut.clear();
        ut.push_back(1.23);
        ut.push_back(4.56);
        BOOST_TEST(test_delimited("1.23 4.56 ", *double_, ut, space));
    }

    // lists
    {
        rule<output_iterator, utree()> r1 = char_ % ',';
        utree ut;
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("a,b", char_ % ',', ut));
        BOOST_TEST(test("a,b", r1, ut));

        rule<output_iterator, utree()> r2 = int_ % ',';
        ut.clear();
        ut.push_back(123);
        ut.push_back(456);
        BOOST_TEST(test("123,456", int_ % ',', ut));
        BOOST_TEST(test("123,456", r2, ut));

        ut.clear();

        rule<output_iterator, utree()> r3 = double_ % ',';
        ut.push_back(1.23);
        ut.push_back(4.56);
        BOOST_TEST(test("1.23,4.56", double_ % ',', ut));
        BOOST_TEST(test("1.23,4.56", r3, ut));
    }

    // alternatives
    {
        rule<output_iterator, utree()> r1 = int_ | double_;
        utree ut(10);
        BOOST_TEST(test("10", int_ | double_, ut));
        BOOST_TEST(test("10", r1, ut));

        ut = 10.2;
        BOOST_TEST(test("10.2", int_ | double_, ut));
        BOOST_TEST(test("10.2", r1, ut));
    }

    // optionals
    {
        utree ut('x');
        BOOST_TEST(test("x", -char_, ut));

        ut.clear();
        BOOST_TEST(test("", -char_, ut));
    }

    {
        using boost::spirit::karma::digit;
        using boost::spirit::karma::as_string;

        utree ut("xy");
        BOOST_TEST(test("xy", string, ut));
        BOOST_TEST(test("xy", as_string[*char_], ut));
        BOOST_TEST(test("x,y", as_string[char_ << ',' << char_], ut));

        ut.clear();
        ut.push_back("ab");
        ut.push_back(1.2);
        BOOST_TEST(test("ab1.2", as_string[*~digit] << double_, ut));
        BOOST_TEST(test("a,b1.2", as_string[~digit % ','] << double_, ut));
    }

    {
        using boost::spirit::karma::verbatim;
        using boost::spirit::karma::repeat;
        using boost::spirit::karma::space;
        using boost::spirit::karma::digit;

        utree ut;
        ut.push_back('x');
        ut.push_back('y');
        ut.push_back('c');
        BOOST_TEST(test_delimited("xy c ", verbatim[repeat(2)[char_]] << char_, ut, space));
        BOOST_TEST(test_delimited("x yc ", char_ << verbatim[*char_], ut, space));

        ut.clear();
        ut.push_back('a');
        ut.push_back('b');
        ut.push_back(1.2);
        BOOST_TEST(test_delimited("ab 1.2 ", verbatim[repeat(2)[~digit]] << double_, ut, space));
    }

    return boost::report_errors();
}
