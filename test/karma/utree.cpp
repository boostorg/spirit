// Copyright (c) 2001-2010 Hartmut Kaiser
// Copyright (c) 2001-2010 Joel de Guzman
// Copyright (c)      2010 Bryce Lelbach
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
    using boost::spirit::utf8_string_range_type;
    using boost::spirit::utf8_string_type;
    using boost::spirit::utf8_symbol_type;

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

        ut = utf8_symbol_type("xyz");
        BOOST_TEST(test("xyz", string, ut));
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

        // ( 1.23 "a" "b" )
        ut.push_back(1.23);
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("1.23ab", double_ << *char_, ut));
        BOOST_TEST(test("1.23ab", r1 << *char_, ut)); 
        BOOST_TEST(test("1.23ab", r2 << *char_, ut)); 

        // ( ( 1.23 ) "a" "b" )
        ut.clear();
        utree ut1;
        ut1.push_back(1.23);
        ut.push_back(ut1);
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("1.23ab", r1 << *char_, ut)); 
        BOOST_TEST(test("1.23ab", r2 << *char_, ut)); 

        // ( "a" "b" 1.23 )
        ut.clear();
        ut.push_back('a');
        ut.push_back('b');
        ut.push_back(1.23);
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << double_, ut));
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << r1, ut));
        BOOST_TEST(test("ab1.23", repeat(2)[~digit] << r2, ut));

        // ( "a" "b" ( 1.23 ) )
        ut.clear();
        ut.push_back('a');
        ut.push_back('b');
        ut.push_back(ut1);
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
        // ( "a" "b" )
        rule<output_iterator, utree()> r1 = char_ % ',';
        utree ut;
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("a,b", char_ % ',', ut));
        BOOST_TEST(test("a,b", r1, ut));

        // ( ( "a" "b" ) )
        utree ut1;
        ut1.push_back(ut);
        BOOST_TEST(test("a,b", r1, ut1));

//         rule<output_iterator, std::vector<char>()> r2 = char_ % ',';
//         BOOST_TEST(test("a,b", r2, ut));
//         BOOST_TEST(test("a,b", r2, ut1));

        // ( ( "a" "b" ) ( "a" "b" ) )
        ut1.push_back(ut);
        BOOST_TEST(test("a,b a,b", r1 << ' ' << r1, ut1));

        // ( 123 456 )
        rule<output_iterator, utree()> r3 = int_ % ',';
        ut.clear();
        ut.push_back(123);
        ut.push_back(456);
        BOOST_TEST(test("123,456", int_ % ',', ut));
        BOOST_TEST(test("123,456", r3, ut));

        // ( ( 123 456 ) ) 
        ut1.clear();
        ut1.push_back(ut);
        BOOST_TEST(test("123,456", r3, ut1));

//         rule<output_iterator, std::vector<int>()> r4 = int_ % ',';
//         BOOST_TEST(test("123,456", r4, ut));
//         BOOST_TEST(test("123,456", r4, ut1));

        // ( ( 123 456 ) ( 123 456 ) ) 
        ut1.push_back(ut);
        BOOST_TEST(test("123,456 123,456", r3 << ' ' << r3, ut1));

        // ( 1.23 4.56 ) 
        rule<output_iterator, utree()> r5 = double_ % ',';
        ut.clear();
        ut.push_back(1.23);
        ut.push_back(4.56);
        BOOST_TEST(test("1.23,4.56", double_ % ',', ut));
        BOOST_TEST(test("1.23,4.56", r5, ut));

        // ( ( 1.23 4.56 ) )
        ut1.clear();
        ut1.push_back(ut);
        BOOST_TEST(test("1.23,4.56", r5, ut1));

//         rule<output_iterator, std::vector<double>()> r6 = double_ % ',';
//         BOOST_TEST(test("1.23,4.56", r6, ut));
//         BOOST_TEST(test("1.23,4.56", r6, ut1));

        // ( ( 1.23 4.56 ) ( 1.23 4.56 ) )
        ut1.push_back(ut);
        BOOST_TEST(test("1.23,4.56 1.23,4.56", r5 <<' ' << r5, ut1));
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

    // as_string
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
    
    // as
    {
        using boost::spirit::karma::digit;
        using boost::spirit::karma::as;
        
        typedef as<std::string> as_string_type;
        as_string_type const as_string = as_string_type();

        typedef as<utf8_symbol_type> as_symbol_type;
        as_symbol_type const as_symbol = as_symbol_type();

        utree ut("xy");
        BOOST_TEST(test("xy", string, ut));
        BOOST_TEST(test("xy", as_string[*char_], ut));
        BOOST_TEST(test("x,y", as_string[char_ << ',' << char_], ut));

        ut.clear();
        ut.push_back("ab");
        ut.push_back(1.2);
        BOOST_TEST(test("ab1.2", as_string[*~digit] << double_, ut));
        BOOST_TEST(test("a,b1.2", as_string[~digit % ','] << double_, ut));
        
        ut = utf8_symbol_type("xy");
        BOOST_TEST(test("xy", string, ut));
        BOOST_TEST(test("xy", as_symbol[*char_], ut));
        BOOST_TEST(test("x,y", as_symbol[char_ << ',' << char_], ut));

        ut.clear();
        ut.push_back(utf8_symbol_type("ab"));
        ut.push_back(1.2);
        BOOST_TEST(test("ab1.2", as_symbol[*~digit] << double_, ut));
        BOOST_TEST(test("a,b1.2", as_symbol[~digit % ','] << double_, ut));
    }
    
    // typed basic_string rules
    {
        utree ut("buzz");

        rule<output_iterator, utf8_string_type()> r1 = string;
        rule<output_iterator, utf8_symbol_type()> r2 = string;

        BOOST_TEST(test("buzz", r1, ut));

        ut = utf8_symbol_type("bar");
        BOOST_TEST(test("bar", r2, ut));
    }

    // parameterized karma::string
    {
        utree ut("foo");

        rule<output_iterator, utf8_string_type()> r1 = string("foo");
        BOOST_TEST(test("foo", string("foo"), ut));
        BOOST_TEST(test("foo", r1, ut));
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
