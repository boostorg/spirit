/*=============================================================================
    Copyright (c) 2015 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include "test.hpp"

namespace x3 = boost::spirit::x3;

struct my_tag;

constexpr auto parserWord = x3::omit[(x3::alpha | x3::lit('_')) >> *(x3::alnum | x3::lit('_'))];

constexpr x3::rule<struct SIdRule1> rule1("");
constexpr auto rule1_def = parserWord >>
	-(
		x3::lit('[') >>
		rule1 >>
		x3::lit(']')
	);

BOOST_SPIRIT_DEFINE(rule1)

int main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    {
        constexpr auto r  = x3::int_ % ',';
        BOOST_TEST(test("123,456", r));
    }

    {
        BOOST_TEST(test("a[b_[c2d]]", rule1));
    }

    {

        constexpr auto a = x3::lit('a');
        constexpr auto b = x3::lit('b');
        constexpr auto c = x3::lit('c');
        constexpr x3::rule<class r> r("");

        {
            constexpr auto start =
                r = *(a | b | c);

            BOOST_TEST(test("abcabcacb", start));
            BOOST_TEST(test(" a b c a b c a c b ", start, x3::space));
        }

        {
            constexpr auto start =
                r = (a | b) >> (r | b);

            BOOST_TEST(test("aaaabababaaabbb", start));
            BOOST_TEST(test("aaaabababaaabba", start, false));

            // ignore the skipper!
            BOOST_TEST(test("aaaabababaaabba", start, x3::space, false));

            BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, x3::space));
            BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, x3::space, false));
        }
    }

    return boost::report_errors();
}
