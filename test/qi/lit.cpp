/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::_1;

    {
        BOOST_TEST((test("kimpo", "kimpo")));
        BOOST_TEST((test("kimpo", lit("kimpo"))));
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)
        BOOST_TEST((test("x", lit("x"))));
        BOOST_TEST((test(L"x", lit(L"x"))));
#endif
        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test("kimpo", s)));
        BOOST_TEST((test(L"kimpo", ws)));
        BOOST_TEST((test("kimpo", lit(s))));
        BOOST_TEST((test(L"kimpo", lit(ws))));
    }

    {
        BOOST_TEST((test(L"kimpo", L"kimpo")));
        BOOST_TEST((test(L"kimpo", lit(L"kimpo"))));
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)
        BOOST_TEST((test(L"x", lit(L"x"))));
#endif
        BOOST_TEST((test(L"x", lit(L'x'))));
        BOOST_TEST((test(L"x", lit(L'x'))));
    }

    {
        std::basic_string<char> s("kimpo");
        BOOST_TEST((test("kimpo", lit(s))));

        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST((test(L"kimpo", lit(ws))));
    }

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST((test("    kimpo", lit("kimpo"), space)));
        BOOST_TEST((test(L"    kimpo", lit(L"kimpo"), space)));
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)
        BOOST_TEST((test("    x", lit("x"), space)));
#endif
        BOOST_TEST((test("    x", lit('x'), space)));
        BOOST_TEST((test(L"    x", lit(L'x'), space)));
    }

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST((test("    kimpo", lit("kimpo"), space)));
        BOOST_TEST((test(L"    kimpo", lit(L"kimpo"), space)));
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)
        BOOST_TEST((test("    x", lit("x"), space)));
#endif
        BOOST_TEST((test("    x", lit('x'), space)));
        BOOST_TEST((test(L"    x", lit(L'x'), space)));
    }

    {
        using namespace boost::spirit::ascii;
        std::string s;
        BOOST_TEST((test_attr("kimpo", string("kimpo"), s)));
        BOOST_TEST(s == "kimpo");
        s.clear();
        BOOST_TEST((test_attr(L"kimpo", string(L"kimpo"), s)));
        BOOST_TEST(s == "kimpo");
        s.clear();
        BOOST_TEST((test_attr("x", string("x"), s)));
        BOOST_TEST(s == "x");
    }

    {   // lazy string

        using namespace boost::spirit::ascii;
        namespace phx = boost::phoenix;

        BOOST_TEST((test("x", string(phx::val("x")))));

        std::string str; // make sure lazy lits have an attribute
        BOOST_TEST(test("x", string(phx::val("x"))[phx::ref(str) = _1]));
        BOOST_TEST(str == "x");
    }

    return boost::report_errors();
}
