//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// #define KARMA_TEST_COMPILE_FAIL

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_action.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>
#include <boost/spirit/include/support_unused.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/fusion/include/vector.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::ascii;
    namespace fusion = boost::fusion;

    {
        {
            BOOST_TEST(test("xi", char_('x') << char_('i')));
            BOOST_TEST(!test("xi", char_('x') << char_('o')));
        }

        {
            BOOST_TEST(test_delimited("x i ", char_('x') << 'i', char(' ')));
            BOOST_TEST(!test_delimited("x i ", 
                char_('x') << char_('o'), char(' ')));
        }

        {
            BOOST_TEST(test_delimited("Hello , World ", 
                lit("Hello") << ',' << "World", char(' ')));
        }

        {
            fusion::vector<char, char, std::string> p ('a', 'b', "cdefg");
            BOOST_TEST(test("abcdefg", char_ << char_ << string, p));
            BOOST_TEST(test_delimited("a b cdefg ", 
                char_ << char_ << string, p, char(' ')));
        }

        {
            fusion::vector<char, int, char> p ('a', 12, 'c');
            BOOST_TEST(test("a12c", char_ << int_ << char_, p));
            BOOST_TEST(test_delimited("a 12 c ", 
                char_ << int_ << char_, p, char(' ')));
        }

        {
            // if all elements of a sequence have unused parameters, the whole 
            // sequence has an unused parameter as well
            fusion::vector<char, char> p ('a', 'e');
            BOOST_TEST(test("abcde", 
                char_ << (lit('b') << 'c' << 'd') << char_, p));
            BOOST_TEST(test_delimited("a b c d e ", 
                char_ << (lit('b') << 'c' << 'd') << char_, p, char(' ')));
        }

        {
            // literal generators do not need an attribute
            fusion::vector<char, char> p('a', 'c');
            BOOST_TEST(test("abc", char_ << 'b' << char_, p));
            BOOST_TEST(test_delimited("a b c ", 
                char_ << 'b' << char_, p, char(' ')));
        }

        {
            // literal generators do not need an attribute, not even at the end
            fusion::vector<char, char> p('a', 'c');
            BOOST_TEST(test("acb", char_ << char_ << 'b', p));
            BOOST_TEST(test_delimited("a c b ", 
                char_ << char_ << 'b', p, char(' ')));
        }

        {
            std::list<int> v;
            v.push_back(1);
            v.push_back(2);
            v.push_back(3);
            BOOST_TEST(test("123", int_ << int_ << int_, v));
            BOOST_TEST(test_delimited("1 2 3 ", int_ << int_ << int_, v, ' '));
            BOOST_TEST(test("1,2,3", int_ << ',' << int_ << ',' << int_, v));
            BOOST_TEST(test_delimited("1 , 2 , 3 ", 
                int_ << ',' << int_ << ',' << int_, v, ' '));
        }

        {
            BOOST_TEST(test("aa", lower[char_('A') << 'a']));
            BOOST_TEST(test_delimited("BEGIN END ", 
                upper[lit("begin") << "end"], char(' ')));
            BOOST_TEST(!test_delimited("BEGIN END ", 
                upper[lit("begin") << "nend"], char(' ')));

            BOOST_TEST(test("Aa        ", left_align[char_('A') << 'a']));
            BOOST_TEST(test("    Aa    ", center[char_('A') << 'a']));
            BOOST_TEST(test("        Aa", right_align[char_('A') << 'a']));
        }

        {
            // make sure single element tuples get passed through if the rhs 
            // has a single element tuple as its attribute
            typedef spirit_test::output_iterator<char>::type iterator_type;
            fusion::vector<double, int> fv(2.0, 1);
            karma::rule<iterator_type, fusion::vector<double, int>()> r;
            r %= double_ << ',' << int_;
            BOOST_TEST(test("test:2.0,1", "test:" << r, fv));
        }

        // action tests
        {
            using namespace boost::phoenix;

            BOOST_TEST(test("abcdefg", 
                (char_ << char_ << string)[_1 = 'a', _2 = 'b', _3 = "cdefg"]));
            BOOST_TEST(test_delimited("a b cdefg ", 
                (char_ << char_ << string)[_1 = 'a', _2 = 'b', _3 = "cdefg"], 
                char(' ')));

            BOOST_TEST(test_delimited("a 12 c ", 
                (char_ << lit(12) << char_)[_1 = 'a', _2 = 'c'], char(' ')));

            char c = 'c';
            BOOST_TEST(test("abc", 
                (char_[_1 = 'a'] << 'b' << char_)[_1 = 'x', _2 = ref(c)]));
            BOOST_TEST(test_delimited("a b c ", 
                (char_[_1 = 'a'] << 'b' << char_)[_2 = ref(c)], char(' ')));

            BOOST_TEST(test("aa", lower[char_ << 'A'][_1 = 'A']));
            BOOST_TEST(test("AA", upper[char_ << 'a'][_1 = 'a']));

            BOOST_TEST(test("Aa        ", left_align[char_ << 'a'][_1 = 'A']));
            BOOST_TEST(test("    Aa    ", center[char_ << 'a'][_1 = 'A']));
            BOOST_TEST(test("        Aa", right_align[char_ << 'a'][_1 = 'A']));
        }
    }

    // test special case where sequence has a one element vector attribute 
    // sequence and this element is a rule (attribute has to be passed through 
    // without change)
    {
        typedef spirit_test::output_iterator<char>::type outiter_type;
        namespace karma = boost::spirit::karma;

        karma::rule<outiter_type, std::vector<int>()> r = -(int_ % ',');
        std::vector<int> v;
        BOOST_TEST(test(">", '>' << r, v));

        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        BOOST_TEST(test(">1,2,3,4", '>' << r, v));
    }

    {
        namespace karma = boost::spirit::karma;
        typedef spirit_test::output_iterator<char>::type outiter_type;

        karma::rule<outiter_type, std::string()> e = karma::string;
        karma::rule<outiter_type, std::vector<std::string>()> l = e << *(',' << e);

        std::vector<std::string> v;
        v.push_back("abc1");
        v.push_back("abc2");
        v.push_back("abc3");
        BOOST_TEST(test("abc1,abc2,abc3", l, v));
    }

    return boost::report_errors();
}

