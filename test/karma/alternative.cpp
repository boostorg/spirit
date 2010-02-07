//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// #define KARMA_TEST_COMPILE_FAIL

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_auxiliary.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost;
    using namespace boost::spirit;
    using namespace boost::spirit::ascii;

    {
        BOOST_TEST(test("x", char_('x') | char_('i')));
        BOOST_TEST(test("xi", char_('x') << char_('i') | char_('i')));
        BOOST_TEST(test("i", char_('i') | char_('x') << char_('i')));

        BOOST_TEST(test("x", buffer[char_('x')] | char_('i')));

        variant<int, char> v (10);
        BOOST_TEST(test("10", char_ | int_, v));
        BOOST_TEST(test("10", int_ | char_, v));
        BOOST_TEST(test("a", lit('a') | char_ | int_, v));
        BOOST_TEST(test("a", char_ | lit('a') | int_, v));
        BOOST_TEST(test("10", int_ | lit('a') | char_, v));

        v = 'c';
        BOOST_TEST(test("c", char_ | int_, v));
        BOOST_TEST(test("a", lit('a') | char_ | int_, v));
        BOOST_TEST(test("c", char_ | lit('a') | int_, v));
        BOOST_TEST(test("a", int_ | lit('a') | char_, v));
        BOOST_TEST(test("c", int_ | char_ | lit('a'), v));
    }

    // testing for alignment/truncation problems on little endian systems
    // (big endian systems will fail one of the other tests below)
    {
        // test optional attribute
        optional<variant<int, char> > v;
        BOOST_TEST(!test("", char_ | int_, v));
        BOOST_TEST(!test("", int_ | char_, v));
        BOOST_TEST(test("a", lit('a') | char_ | int_, v));
        BOOST_TEST(test("a", char_ | lit('a') | int_, v));
        BOOST_TEST(test("a", int_ | lit('a') | char_, v));

        v = 10;
        BOOST_TEST(test("10", char_ | int_, v));
        BOOST_TEST(test("10", int_ | char_, v));
        BOOST_TEST(test("a", lit('a') | char_ | int_, v));
        BOOST_TEST(test("a", char_ | lit('a') | int_, v));
        BOOST_TEST(test("10", int_ | lit('a') | char_, v));

        v = 'c';
        BOOST_TEST(test("c", char_ | int_, v));
        BOOST_TEST(test("a", lit('a') | char_ | int_, v));
        BOOST_TEST(test("c", char_ | lit('a') | int_, v));
        BOOST_TEST(test("a", int_ | lit('a') | char_, v));
        BOOST_TEST(test("c", int_ | char_ | lit('a'), v));
    }

    {
        // more tests for optional attribute
        optional<int> o;
        BOOST_TEST(test("a", lit('a') | int_, o));
        BOOST_TEST(test("a", int_ | lit('a'), o));

        o = 10;
        BOOST_TEST(test("a", lit('a') | int_, o));
        BOOST_TEST(test("10", int_ | lit('a'), o));
    }

    {
        int i = 10;
        BOOST_TEST(test("a", lit('a') | int_, i));
        BOOST_TEST(test("10", int_ | lit('a'), i));
    }

    {
        BOOST_TEST(test("abc", string | int_, std::string("abc")));
        BOOST_TEST(test("1234", string | int_, 1234));
        BOOST_TEST(test("abc", int_ | string, std::string("abc")));
        BOOST_TEST(test("1234", int_ | string, 1234));
    }

    {
        // testing for alignment/truncation problems on little endian systems
        // (big endian systems will fail one of the other tests below)
        std::basic_string<wchar_t> generated;
        std::back_insert_iterator<std::basic_string<wchar_t> > outit(generated);
        boost::variant<int, char> v(10);
        bool result = karma::generate_delimited(outit
          , karma::int_ | karma::char_, karma::char_(' '), v);
        BOOST_TEST(result && generated == L"10 ");
    }

    {
        // test if alternatives with all components having unused 
        // attribute generate first alternative 
        fusion::vector<char, char> v('a', 'b');
        BOOST_TEST(test("axb", char_ << (lit('x') | lit('i')) << char_, v));
        BOOST_TEST(test("axib", 
            char_ << (lit('x') << lit('i') | lit('i')) << char_, v));
    }

    {
        BOOST_TEST(test_delimited("x ", char_('x') | char_('i'), char_(' ')));
        BOOST_TEST(test_delimited("x i ", 
            char_('x') << char_('i') | char_('i'), char_(' ')));
        BOOST_TEST(test_delimited("i ", 
            char_('i') | char_('x') << char_('i'), char_(' ')));

        variant<int, char> v (10);
        BOOST_TEST(test_delimited("10 ", char_ | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("10 ", int_ | char_, v, char_(' ')));
        BOOST_TEST(test_delimited("a ", lit('a') | char_ | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("a ", char_ | lit('a') | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("10 ", int_ | lit('a') | char_, v, char_(' ')));

        v = 'c';
        BOOST_TEST(test_delimited("c ", char_ | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("a ", lit('a') | char_ | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("c ", char_ | lit('a') | int_, v, char_(' ')));
        BOOST_TEST(test_delimited("a ", int_ | lit('a') | char_, v, char_(' ')));
        BOOST_TEST(test_delimited("c ", int_ | char_ | lit('a'), v, char_(' ')));
    }

// this leads to infinite loops
//     {
//         variant<int, std::string> v(10);
//         BOOST_TEST(test("10", int_ | +char_, v));
// 
//         v = "abc";
//         BOOST_TEST(test("abc", int_ | +char_, v));
//     }

    {
        // if nothing matches, the first explicit alternative will be chosen
        variant<double, char const*> v (10.0);
        BOOST_TEST(test("11", char_ | lit(11), v));
        BOOST_TEST(test("11", lit(11) | char_ , v));
        BOOST_TEST(test("10.0", double_ | lit(11), v));
        BOOST_TEST(test("11", lit(11) | double_, v));
        BOOST_TEST(!test("", char_ | int_, v));

        v = "c";
        BOOST_TEST(test("11", char_ | lit(11), v));
        BOOST_TEST(test("11", double_ | lit(11), v));
        BOOST_TEST(!test("", char_ | int_, v));
    }

    {
        // if nothing matches, the first explicit alternative will be chosen
        variant<double, char const*> v (10.0);
        BOOST_TEST(test_delimited("11 ", char_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", lit(11) | char_ , v, char_(' ')));
        BOOST_TEST(test_delimited("10.0 ", double_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", lit(11) | double_, v, char_(' ')));
        BOOST_TEST(!test_delimited("", char_ | int_, v, char_(' ')));

        v = "c";
        BOOST_TEST(test_delimited("11 ", char_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", double_ | lit(11), v, char_(' ')));
        BOOST_TEST(!test_delimited("", char_ | int_, v, char_(' ')));
    }

    {
        // if nothing matches, the first explicit alternative will be chosen,
        // optionals need to be accepted
        optional<variant<double, char const*> > v (10.0);
        BOOST_TEST(test_delimited("11 ", char_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", lit(11) | char_ , v, char_(' ')));
        BOOST_TEST(test_delimited("10.0 ", double_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", lit(11) | double_, v, char_(' ')));
        BOOST_TEST(!test_delimited("", char_ | int_, v, char_(' ')));

        v = "c";
        BOOST_TEST(test_delimited("11 ", char_ | lit(11), v, char_(' ')));
        BOOST_TEST(test_delimited("11 ", double_ | lit(11), v, char_(' ')));
        BOOST_TEST(!test_delimited("", char_ | int_, v, char_(' ')));
    }

    {
        std::vector<int> v;
        BOOST_TEST(test("[]", '[' << (int_ % ", ") << ']' | "[]", v));
        BOOST_TEST(test("[]", '[' << -(int_ % ", ") << ']', v));
        BOOST_TEST(test("[]", '[' << ((int_ % ", ") | eps) << ']', v));

        v.push_back(5);
        v.push_back(5);
        v.push_back(5);
        BOOST_TEST(test("[5, 5, 5]", '[' << (int_ % ", ") << ']' | "[]", v));
    }

    {
        boost::optional<int> v;
        BOOST_TEST(test("error", int_ | "error" << omit[-int_], v));
        BOOST_TEST(test("error", int_ | "error" << omit[int_], v));
        v = 1;
        BOOST_TEST(test("1", int_ | "error" << omit[-int_], v));
        BOOST_TEST(test("1", int_ | "error" << omit[int_], v));
    }

    {
        typedef spirit_test::output_iterator<char>::type outiter_type;
        namespace karma = boost::spirit::karma;

        karma::rule<outiter_type, int()> r = int_;
        std::vector<int> v;
        BOOST_TEST(test("", '>' << r % ',' | karma::eps, v));

        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        BOOST_TEST(test(">1,2,3,4", '>' << r % ',' | karma::eps, v));
    }

    {
        typedef spirit_test::output_iterator<char>::type outiter_type;
        namespace karma = boost::spirit::karma;

        karma::rule<outiter_type, boost::optional<int>()> r = int_;
        boost::optional<int> o;
        BOOST_TEST(test("error", r | "error", o));

        o = 10;
        BOOST_TEST(test("10", r | "error", o));
    }

    return boost::report_errors();
}

