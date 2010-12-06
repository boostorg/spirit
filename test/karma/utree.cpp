// Copyright (c) 2001-2010 Hartmut Kaiser
// Copyright (c) 2001-2010 Joel de Guzman
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/support_utree.hpp>

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
        utree ut1;
        ut1.push_back('a');
        ut1.push_back('b');
        ut.push_back(ut1);
        BOOST_TEST(test("1.23ab", double_ << *char_, ut));
        BOOST_TEST(test("1.23ab", r1 << *char_, ut)); 
        BOOST_TEST(test("1.23ab", r2 << *char_, ut)); 

        ut.clear();
        ut.push_back(ut1);
        ut.push_back(1.23);
        BOOST_TEST(test("ab1.23", *~digit << double_, ut));
        BOOST_TEST(test("ab1.23", *~digit << r1, ut));
        BOOST_TEST(test("ab1.23", *~digit << r2, ut));
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
        utree ut;
        ut.push_back('a');
        ut.push_back('b');
        BOOST_TEST(test("a,b", char_ % ',', ut));

        ut.clear();
        ut.push_back(123);
        ut.push_back(456);
        BOOST_TEST(test("123,456", int_ % ',', ut));

        ut.clear();

        rule<output_iterator, utree()> r4 = double_ % ',';
        ut.push_back(1.23);
        ut.push_back(4.56);
        BOOST_TEST(test("1.23,4.56", double_ % ',', ut));
        BOOST_TEST(test("1.23,4.56", r4, ut));

//         rule<output_iterator, std::vector<char>()> r1 = char_ % ',';
//         ut.clear();
//         ut.push_back('a');
//         ut.push_back('b');
//         BOOST_TEST(test("a,b", r1, ut));

//         rule<char const*, std::vector<int>()> r2 = int_ % ',';
//         ut.clear();
//         BOOST_TEST(test("123,456", r2, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( 123 456 )"));
// 
//         rule<char const*, std::vector<double>()> r3 = double_ % ',';
//         ut.clear();
//         BOOST_TEST(test("1.23,4.56", r3, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( 1.23 4.56 )"));
    }

//     // alternatives
//     {
//         using boost::spirit::qi::real_parser;
//         using boost::spirit::qi::strict_real_policies;
// 
//         typedef real_parser<double, strict_real_policies<double> >
//             strict_double_type;
//         strict_double_type const strict_double = strict_double_type();
// 
//         utree ut;
//         BOOST_TEST(test("10", strict_double | int_, ut) &&
//             ut.which() == utree_type::int_type && check(ut, "10"));
//         ut.clear();
//         BOOST_TEST(test("10.2", strict_double | int_, ut) &&
//             ut.which() == utree_type::double_type && check(ut, "10.2"));
// 
//         rule<char const*, boost::variant<int, double>()> r1 = strict_double | int_;
// 
//         ut.clear();
//         BOOST_TEST(test("10", r1, ut) &&
//             ut.which() == utree_type::int_type && check(ut, "10"));       // FIXME?: "( 10 )"
//         ut.clear();
//         BOOST_TEST(test("10.2", r1, ut) &&
//             ut.which() == utree_type::double_type && check(ut, "10.2"));  // FIXME?: "( 10.2 )"
// 
//         rule<char const*, utree()> r2 = strict_double | int_;
// 
//         ut.clear();
//         BOOST_TEST(test("10", r2, ut) &&
//             ut.which() == utree_type::int_type && check(ut, "10"));       // FIXME?: "( 10 )"
//         ut.clear();
//         BOOST_TEST(test("10.2", r2, ut) &&
//             ut.which() == utree_type::double_type && check(ut, "10.2"));  // FIXME?: "( 10.2 )"
//     }
// 
//     // optionals
//     {
//         utree ut;
//         BOOST_TEST(test("x", -char_, ut) &&
//             ut.which() == utree_type::string_type && check(ut, "\"x\""));
//         ut.clear();
//         BOOST_TEST(test("", -char_, ut) &&
//             ut.which() == utree_type::nil_type && check(ut, "<nil>"));
//     }
// 
//     // as_string
//     {
//         using boost::spirit::qi::digit;
// 
//         utree ut;
//         BOOST_TEST(test("xy", as_string[char_ >> char_], ut) &&
//             ut.which() == utree_type::string_type && check(ut, "\"xy\""));
//         ut.clear();
// 
//         BOOST_TEST(test("ab1.2", as_string[*~digit] >> double_, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"ab\" 1.2 )"));
//         ut.clear();
// 
//         BOOST_TEST(test("xy", as_string[*char_], ut) &&
//             ut.which() == utree_type::string_type && check(ut, "\"xy\""));
//         ut.clear();
// 
//         BOOST_TEST(test("x,y", as_string[char_ >> ',' >> char_], ut) &&
//             ut.which() == utree_type::string_type && check(ut, "\"xy\""));
//         ut.clear();
// 
//         BOOST_TEST(test("x,y", char_ >> ',' >> char_, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));
//         ut.clear();
// 
//         BOOST_TEST(test("a,b1.2", as_string[~digit % ','] >> double_, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"ab\" 1.2 )"));
//         ut.clear();
// 
//         BOOST_TEST(test("a,b1.2", ~digit % ',' >> double_, ut) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"a\" \"b\" 1.2 )"));   // FIXME?: "( ( \"a\" \"b\" ) 1.2 )"
//         ut.clear();
//     }
// 
//     {
//         using boost::spirit::qi::digit;
//         using boost::spirit::qi::space;
// 
//         utree ut;
//         BOOST_TEST(test("xy c", lexeme[char_ >> char_] >> char_, ut, space) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"xy\" \"c\" )"));
//         ut.clear();
// 
//         BOOST_TEST(test("ab 1.2", lexeme[*~digit] >> double_, ut, space) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"ab\" 1.2 )"));
//         ut.clear();
// 
//         BOOST_TEST(test("a xy", char_ >> lexeme[*char_], ut, space) &&
//             ut.which() == utree_type::list_type && check(ut, "( \"a\" \"xy\" )"));
//         ut.clear();
//     }

    return boost::report_errors();
}
