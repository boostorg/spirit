/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_UNICODE
#define BOOST_SPIRIT_X3_UNICODE

#include "test.hpp"

#include <boost/spirit/home/x3.hpp>

#include <type_traits>
#include <iostream>

int main()
{
    using spirit_test::test;
    using spirit_test::test_failure;
    using spirit_test::test_attr;

    using boost::spirit::x3::unused_type;

    {
        using namespace boost::spirit::x3::standard;
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(upper);
        BOOST_TEST(test("1", alnum));
        BOOST_TEST(!test(" ", alnum));
        BOOST_TEST(!test("1", alpha));
        BOOST_TEST(test("x", alpha));
        BOOST_TEST(test(" ", blank));
        BOOST_TEST(!test("x", blank));
        BOOST_TEST(test("1", digit));
        BOOST_TEST(!test("x", digit));
        BOOST_TEST(test("a", lower));
        BOOST_TEST(!test("A", lower));
        BOOST_TEST(test("!", punct));
        BOOST_TEST(!test("x", punct));
        BOOST_TEST(test(" ", space));
        BOOST_TEST(test("\n", space));
        BOOST_TEST(test("\r", space));
        BOOST_TEST(test("\t", space));
        BOOST_TEST(test("A", upper));
        BOOST_TEST(!test("a", upper));
        BOOST_TEST(test("A", xdigit));
        BOOST_TEST(test("0", xdigit));
        BOOST_TEST(test("f", xdigit));
        BOOST_TEST(!test("g", xdigit));
        BOOST_TEST(!test("\xF1", print));
    }

    {
        using namespace boost::spirit::x3::standard_wide;
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(upper);
        BOOST_TEST(test(L"1", alnum));
        BOOST_TEST(!test(L" ", alnum));
        BOOST_TEST(!test(L"1", alpha));
        BOOST_TEST(test(L"x", alpha));
        BOOST_TEST(test(L" ", blank));
        BOOST_TEST(!test(L"x", blank));
        BOOST_TEST(test(L"1", digit));
        BOOST_TEST(!test(L"x", digit));
        BOOST_TEST(test(L"a", lower));
        BOOST_TEST(!test(L"A", lower));
        BOOST_TEST(test(L"!", punct));
        BOOST_TEST(!test(L"x", punct));
        BOOST_TEST(test(L" ", space));
        BOOST_TEST(test(L"\n", space));
        BOOST_TEST(test(L"\r", space));
        BOOST_TEST(test(L"\t", space));
        BOOST_TEST(test(L"A", upper));
        BOOST_TEST(!test(L"a", upper));
        BOOST_TEST(test(L"A", xdigit));
        BOOST_TEST(test(L"0", xdigit));
        BOOST_TEST(test(L"f", xdigit));
        BOOST_TEST(!test(L"g", xdigit));
    }

    {
        using namespace boost::spirit::x3::unicode;
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(upper);
        BOOST_TEST(test(U"1", alnum));
        BOOST_TEST(!test(U" ", alnum));
        BOOST_TEST(!test(U"1", alpha));
        BOOST_TEST(test(U"x", alpha));
        BOOST_TEST(test(U" ", blank));
        BOOST_TEST(!test(U"x", blank));
        BOOST_TEST(test(U"1", digit));
        BOOST_TEST(!test(U"x", digit));
        BOOST_TEST(test(U"a", lower));
        BOOST_TEST(!test(U"A", lower));
        BOOST_TEST(test(U"!", punct));
        BOOST_TEST(!test(U"x", punct));
        BOOST_TEST(test(U" ", space));
        BOOST_TEST(test(U"\n", space));
        BOOST_TEST(test(U"\r", space));
        BOOST_TEST(test(U"\t", space));
        BOOST_TEST(test(U"A", upper));
        BOOST_TEST(!test(U"a", upper));
        BOOST_TEST(test(U"A", xdigit));
        BOOST_TEST(test(U"0", xdigit));
        BOOST_TEST(test(U"f", xdigit));
        BOOST_TEST(!test(U"g", xdigit));

        BOOST_TEST(test(U"A", alphabetic));
        BOOST_TEST(test(U"9", decimal_number));
        BOOST_TEST(test(U"\u2800", braille));
        BOOST_TEST(!test(U" ", braille));
        BOOST_TEST(test(U" ", ~braille));
        // TODO: Add more unicode tests
    }

    {   // test invalid unicode literals
        using namespace boost::spirit::x3::unicode;

        auto const invalid_unicode = char32_t{0x7FFFFFFF};
        auto const input           = std::u32string_view(&invalid_unicode, 1);

        BOOST_TEST(test_failure(input, char_));

        // force unicode category lookup
        // related issue: https://github.com/boostorg/spirit/issues/524
        BOOST_TEST(test_failure(input, alpha));
        BOOST_TEST(test_failure(input, upper));
        BOOST_TEST(test_failure(input, lower));
    }

    {   // test attribute extraction
        using boost::spirit::x3::traits::attribute_of_t;
        using boost::spirit::x3::standard::alpha;
        using boost::spirit::x3::standard::alpha_type;

        static_assert(std::is_same_v<attribute_of_t<alpha_type, unused_type>, char>);

        int attr = 0;
        BOOST_TEST(test_attr("a", alpha, attr));
        BOOST_TEST(attr == 'a');
    }

    {   // test attribute extraction
        using boost::spirit::x3::standard::alpha;
        using boost::spirit::x3::standard::space;
        char attr = 0;
        BOOST_TEST(test_attr("     a", alpha, attr, space));
        BOOST_TEST(attr == 'a');
    }

    {   // test action
        using namespace boost::spirit::x3::standard;
        using boost::spirit::x3::_attr;
        char ch;
        auto f = [&](auto& ctx){ ch = _attr(ctx); };

        BOOST_TEST(test("x", alnum[f]));
        BOOST_TEST(ch == 'x');
        BOOST_TEST(test("   A", alnum[f], space));
        BOOST_TEST(ch == 'A');
    }

    return boost::report_errors();
}
