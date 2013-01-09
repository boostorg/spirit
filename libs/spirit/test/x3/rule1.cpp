/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// this file deliberately contains non-ascii characters
// boostinspect:noascii

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3/nonterminal/rule.hpp>
#include <boost/spirit/home/x3/char.hpp>
#include <boost/spirit/home/x3/string.hpp>
#include <boost/spirit/home/x3/numeric.hpp>
#include <boost/spirit/home/x3/operator.hpp>

//~ #include <boost/fusion/include/std_pair.hpp>

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

    //~ namespace phx = boost::phoenix;

    { // basic tests

        auto a = lit('a');
        auto b = lit('b');
        auto c = lit('c');
        rule<class r> r;

        {
            auto start =
                r = *(a | b | c);

            BOOST_TEST(test("abcabcacb", start));
        }

        {
            auto start =
                r = (a | b) >> (r | b);

            BOOST_TEST(test("aaaabababaaabbb", start));
            BOOST_TEST(test("aaaabababaaabba", start, false));

            // ignore the skipper!
            BOOST_TEST(test("aaaabababaaabba", start, space, false));
        }
    }
    //~ { // basic tests with direct initialization

        //~ rule<char const*> a ('a');
        //~ rule<char const*> b ('b');
        //~ rule<char const*> c ('c');
        //~ rule<char const*> start = (a | b) >> (start | b);

        //~ BOOST_TEST(test("aaaabababaaabbb", start));
        //~ BOOST_TEST(test("aaaabababaaabba", start, false));

        //~ // ignore the skipper!
        //~ BOOST_TEST(test("aaaabababaaabba", start, space, false));
    //~ }

    { // basic tests w/ skipper

        auto a = lit('a');
        auto b = lit('b');
        auto c = lit('c');
        rule<class r> r;

        {
            auto start =
                r = *(a | b | c);

            BOOST_TEST(test(" a b c a b c a c b ", start, space));
        }

        {
            auto start =
                r = (a | b) >> (r | b);

            BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, space));
            BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, space, false));
        }
    }

/*

    { // basic tests w/ skipper but no final post-skip

        rule<char const*, space_type> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';

        a.name("a");
        b.name("b");
        c.name("c");
        start.name("start");

        debug(a);
        debug(b);
        debug(c);
        debug(start);

        start = *(a | b) >> c;

        using boost::spirit::x3::phrase_parse;
        using boost::spirit::x3::skip_flag;
        {
            char const *s1 = " a b a a b b a c ... "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::dont_postskip)
              && s1 == e1 - 5);
        }

        start = (a | b) >> (start | c);
        {
            char const *s1 = " a a a a b a b a b a a a b b b c "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::postskip)
              && s1 == e1);
        }
        {
            char const *s1 = " a a a a b a b a b a a a b b b c "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::dont_postskip)
              && s1 == e1 - 1);
        }
    }
*/
    return boost::report_errors();
}

