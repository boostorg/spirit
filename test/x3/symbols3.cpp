/*=============================================================================
    Copyright (c) 2013 Carl Barron
    Copyright (c) 2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_X3_DEBUG
#define BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL 0

#include "test.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/char_encoding/unicode.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/mpl/int.hpp>

#include <optional>
#include <iostream>
#include <numeric>

struct roman
{
    std::optional<int> a;
    std::optional<int> b;
    std::optional<int> c;
};

BOOST_FUSION_ADAPT_STRUCT(roman,
    a, b, c
)

int eval(roman const & c)
{
    return c.a.value_or(0) + c.b.value_or(0) + c.c.value_or(0);
}

int main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::x3::shared_symbols;

    { // construction from initializer-list
        shared_symbols<int> const ones =
        {
            {"I", 1}, {"II", 2}, {"III", 3}, {"IV", 4},
            {"V", 5}, {"VI", 6}, {"VII", 7}, {"VIII", 8},
            {"IX", 9}
        };
        shared_symbols<int> const tens =
        {
            {"X", 10}, {"XX", 20}, {"XXX", 30}, {"XL", 40},
            {"L", 50}, {"LX", 60}, {"LXX", 70}, {"LXXX", 80},
            {"XC", 90}
        };
        shared_symbols<int> const hundreds
        {
            {"C", 100}, {"CC", 200}, {"CCC", 300}, {"CD", 400},
            {"D", 500}, {"DC", 600}, {"DCC", 700}, {"DCCC", 800},
            {"CM", 900}
        };

        auto number = -hundreds >> -tens >> -ones;

        roman r;
        BOOST_TEST((test_attr("CDXLII", number, r)));
        BOOST_TEST(eval(r) == 442);
    }

    { // construction from initializer-list without attribute
        shared_symbols<> foo = {"a1", "a2", "a3"};

        BOOST_TEST((test("a3", foo)));
    }

    { // assignment from initializer-list
        shared_symbols<> foo;
        foo = {"a1", "a2", "a3"};

        BOOST_TEST((test("a3", foo)));
    }

    { // unicode | construction from initializer-list
        using namespace boost::spirit;
        x3::shared_symbols_parser<char_encoding::unicode, int> foo = {{U"a1", 1}, {U"a2", 2}, {U"a3", 3}};

        int r;
        BOOST_TEST((test_attr(U"a3", foo, r)));
        BOOST_TEST(r == 3);
    }

    return boost::report_errors();
}
