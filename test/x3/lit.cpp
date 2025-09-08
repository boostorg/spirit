/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#undef BOOST_SPIRIT_NO_STANDARD_WIDE

#ifndef BOOST_SPIRIT_UNICODE
# define BOOST_SPIRIT_UNICODE
#endif

#ifndef BOOST_SPIRIT_X3_UNICODE
# define BOOST_SPIRIT_X3_UNICODE
#endif

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/vector.hpp>

#include <string>

#include "test.hpp"

int main()
{
    namespace x3 = boost::spirit::x3;
    using spirit_test::test;
    using spirit_test::test_attr;

    // standard
    {
        (void)x3::lit('f'); // deprecated
        (void)x3::lit("f"); // deprecated
        (void)x3::lit("foo"); // deprecated
        (void)x3::standard::lit('f');
        (void)x3::standard::lit("f");
        (void)x3::standard::lit("foo");

        (void)x3::char_('f'); // deprecated
        (void)x3::char_("f"); // deprecated
        (void)x3::char_("foo"); // deprecated
        (void)x3::standard::char_('f');
        (void)x3::standard::char_("f");
        (void)x3::standard::char_("foo");

        (void)x3::string('f'); // deprecated
        (void)x3::string("f"); // deprecated
        (void)x3::string("foo"); // deprecated
        (void)x3::standard::string('f');
        (void)x3::standard::string("f");
        (void)x3::standard::string("foo");
    }

    // standard_wide
    {
        (void)x3::lit(L'f'); // deprecated
        (void)x3::lit(L"f"); // deprecated
        (void)x3::lit(L"foo"); // deprecated
        (void)x3::standard_wide::lit(L'f');
        (void)x3::standard_wide::lit(L"f");
        (void)x3::standard_wide::lit(L"foo");

        (void)x3::standard_wide::char_(L'f');
        (void)x3::standard_wide::char_(L"f");
        (void)x3::standard_wide::char_(L"foo");

        (void)x3::string(L'f'); // deprecated
        (void)x3::string(L"f"); // deprecated
        (void)x3::string(L"foo"); // deprecated
        (void)x3::standard_wide::string(L'f');
        (void)x3::standard_wide::string(L"f");
        (void)x3::standard_wide::string(L"foo");
    }

    // unicode
    {
        (void)x3::unicode::lit(U'f');
        (void)x3::unicode::lit(U"f");
        (void)x3::unicode::lit(U"foo");

        (void)x3::unicode::char_(U'f');
        (void)x3::unicode::char_(U"f");
        (void)x3::unicode::char_(U"foo");

        (void)x3::unicode::string(U'f');
        (void)x3::unicode::string(U"f");
        (void)x3::unicode::string(U"foo");
    }

    {
        std::string attr;
        auto p = x3::standard::char_ >> x3::standard::lit("\n");
        BOOST_TEST(test_attr("A\n", p, attr));
        BOOST_TEST(attr == "A");
    }

    {
        std::wstring attr;
        auto p = x3::standard_wide::char_ >> x3::standard_wide::lit(L"\n");
        BOOST_TEST(test_attr(L"É\n", p, attr));
        BOOST_TEST(attr == L"É");
    }

    // -------------------------------------------------

    {
        BOOST_TEST((test("kimpo", x3::standard::lit("kimpo"))));

        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test("kimpo", x3::standard::lit(s))));
        BOOST_TEST((test(L"kimpo", x3::standard_wide::lit(ws))));
    }

    {
        std::basic_string<char> s("kimpo");
        BOOST_TEST((test("kimpo", x3::standard::lit(s))));

        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test(L"kimpo", x3::standard_wide::lit(ws))));
    }

    // -------------------------------------------------

    {
        BOOST_TEST((test("kimpo", "kimpo")));
        BOOST_TEST((test("kimpo", x3::standard::string("kimpo"))));

        BOOST_TEST((test("x", x3::standard::string("x"))));
        BOOST_TEST((test(L"x", x3::standard_wide::string(L"x"))));

        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test("kimpo", s)));
        BOOST_TEST((test(L"kimpo", ws)));
        BOOST_TEST((test("kimpo", x3::standard::string(s))));
        BOOST_TEST((test(L"kimpo", x3::standard_wide::string(ws))));
    }

    {
        BOOST_TEST((test(L"kimpo", L"kimpo")));
        BOOST_TEST((test(L"kimpo", x3::standard_wide::string(L"kimpo"))));
        BOOST_TEST((test(L"x", x3::standard_wide::string(L"x"))));
    }

    {
        std::basic_string<char> s("kimpo");
        BOOST_TEST((test("kimpo", x3::standard::string(s))));

        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test(L"kimpo", x3::standard_wide::string(ws))));
    }

    {
        // single-element fusion vector tests
        boost::fusion::vector<std::string> s;
        BOOST_TEST(test_attr("kimpo", x3::standard::string("kimpo"), s));
        BOOST_TEST(boost::fusion::at_c<0>(s) == "kimpo");
    }
    return boost::report_errors();
}
