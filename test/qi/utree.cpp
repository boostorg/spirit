// Copyright (c) 2001-2010 Hartmut Kaiser
// Copyright (c) 2001-2010 Joel de Guzman
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_utree.hpp>

#include <sstream>

#include "test.hpp"

inline bool check(boost::spirit::utree const& val, std::string expected)
{
    std::stringstream s;
    s << val;
    return s.str() == expected + " ";
}

int main()
{
    using spirit_test::test_attr;
    using boost::spirit::utree;
    using boost::spirit::utree_type;
    using boost::spirit::utf8_string_range;

    using boost::spirit::qi::char_;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::double_;
    using boost::spirit::qi::space;
    using boost::spirit::qi::rule;

    // primitive data types
    {
        utree ut;
        BOOST_TEST(test_attr("x", char_, ut) && 
            ut.which() == utree_type::string_type && check(ut, "\"x\""));
        ut.clear();
        BOOST_TEST(test_attr("123", int_, ut) && 
            ut.which() == utree_type::int_type && check(ut, "123"));
        ut.clear();
        BOOST_TEST(test_attr("123.45", double_, ut) && 
            ut.which() == utree_type::double_type && check(ut, "123.45"));
    }

    // sequences
    {
        using boost::spirit::qi::digit;

        utree ut;
        BOOST_TEST(test_attr("xy", char_ >> char_, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));   // should be "( \"xy\" )" instead
        ut.clear();
        BOOST_TEST(test_attr("123 456", int_ >> int_, ut, space) && 
            ut.which() == utree_type::list_type && check(ut, "( 123 456 )"));
        ut.clear();
        BOOST_TEST(test_attr("1.23 4.56", double_ >> double_, ut, space) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));

        ut.clear();
        BOOST_TEST(test_attr("1.2ab", double_ >> *char_, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.2 \"a\" \"b\" )"));
        ut.clear();
        BOOST_TEST(test_attr("ab1.2", *~digit >> double_, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"a\" \"b\" 1.2 )"));

        rule<char const*, utree()> r = double_;

        ut.clear();
        BOOST_TEST(test_attr("1.2ab", r >> *char_, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.2 \"a\" \"b\" )"));
        ut.clear();
        BOOST_TEST(test_attr("ab1.2", *~digit >> r, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"a\" \"b\" 1.2 )"));
    }

    // kleene star
    {
        utree ut;
        BOOST_TEST(test_attr("xy", *char_, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));   // should be "( \"xy\" )" instead
        ut.clear();
        BOOST_TEST(test_attr("123 456", *int_, ut, space) && 
            ut.which() == utree_type::list_type && check(ut, "( 123 456 )"));
        ut.clear();
        BOOST_TEST(test_attr("1.23 4.56", *double_, ut, space) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));
    }

    // lists
    {
        utree ut;
        BOOST_TEST(test_attr("x,y", char_ % ',', ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));
        ut.clear();
        BOOST_TEST(test_attr("123,456", int_ % ',', ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 123 456 )"));
        ut.clear();
        BOOST_TEST(test_attr("1.23,4.56", double_ % ',', ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));

        rule<char const*, std::vector<char>()> r1 = char_ % ',';
        ut.clear();
        BOOST_TEST(test_attr("x,y", r1, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));

        rule<char const*, std::vector<int>()> r2 = int_ % ',';
        ut.clear();
        BOOST_TEST(test_attr("123,456", r2, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 123 456 )"));

        rule<char const*, std::vector<double>()> r3 = double_ % ',';
        ut.clear();
        BOOST_TEST(test_attr("1.23,4.56", r3, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));

        rule<char const*, utree()> r4 = double_ % ',';
        ut.clear();
        BOOST_TEST(test_attr("1.23,4.56", r4, ut) && 
            ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));
    }

    // alternatives
    {
        using boost::spirit::qi::real_parser;
        using boost::spirit::qi::strict_real_policies;

        typedef real_parser<double, strict_real_policies<double> > 
            strict_double_type;
        strict_double_type const strict_double = strict_double_type();

        utree ut;
        BOOST_TEST(test_attr("10", strict_double | int_, ut) && 
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", strict_double | int_, ut) && 
            ut.which() == utree_type::double_type && check(ut, "10.2"));

        rule<char const*, boost::variant<int, double>()> r1 = strict_double | int_;

        ut.clear();
        BOOST_TEST(test_attr("10", r1, ut) && 
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", r1, ut) && 
            ut.which() == utree_type::double_type && check(ut, "10.2"));

        rule<char const*, utree()> r2 = strict_double | int_;

        ut.clear();
        BOOST_TEST(test_attr("10", r2, ut) && 
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", r2, ut) && 
            ut.which() == utree_type::double_type && check(ut, "10.2"));
    }

    // optionals
    {
        utree ut;
        BOOST_TEST(test_attr("x", -char_, ut) && 
            ut.which() == utree_type::string_type && check(ut, "\"x\""));
        ut.clear();
        BOOST_TEST(test_attr("", -char_, ut) && 
            ut.which() == utree_type::nil_type && check(ut, "<nil>"));
    }

    return boost::report_errors();
}
