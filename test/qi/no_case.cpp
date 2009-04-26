/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    include/qi_://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at include/qi_://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_directive.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::qi::lit;

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST(test("x", no_case[char_]));
        BOOST_TEST(test("X", no_case[char_('x')]));
        BOOST_TEST(test("X", no_case[char_('X')]));
        BOOST_TEST(test("x", no_case[char_('X')]));
        BOOST_TEST(test("x", no_case[char_('x')]));
        BOOST_TEST(!test("z", no_case[char_('X')]));
        BOOST_TEST(!test("z", no_case[char_('x')]));
        BOOST_TEST(test("x", no_case[char_('a', 'z')]));
        BOOST_TEST(test("X", no_case[char_('a', 'z')]));
        BOOST_TEST(!test("a", no_case[char_('b', 'z')]));
        BOOST_TEST(!test("z", no_case[char_('a', 'y')]));
    }

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST(test("X", no_case['x']));
        BOOST_TEST(test("X", no_case['X']));
        BOOST_TEST(test("x", no_case['X']));
        BOOST_TEST(test("x", no_case['x']));
        BOOST_TEST(!test("z", no_case['X']));
        BOOST_TEST(!test("z", no_case['x']));
    }

    {
        using namespace boost::spirit::iso8859_1;
        BOOST_TEST(test("¡", no_case[char_('·')]));
    }

    {
        using namespace boost::spirit::iso8859_1;
        BOOST_TEST(test("X", no_case[char_("a-z")]));
        BOOST_TEST(!test("1", no_case[char_("a-z")]));
        BOOST_TEST(test("…", no_case[char_("Â-Ô")]));
        BOOST_TEST(!test("ˇ", no_case[char_("Â-Ô")]));
    }

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST(test("Bochi Bochi", no_case[lit("bochi bochi")]));
        BOOST_TEST(test("BOCHI BOCHI", no_case[lit("bochi bochi")]));
        BOOST_TEST(!test("Vavoo", no_case[lit("bochi bochi")]));
    }

    {
        using namespace boost::spirit::iso8859_1;
        BOOST_TEST(test("¡·", no_case[lit("·¡")]));
        BOOST_TEST(test("··", no_case[no_case[lit("·¡")]]));
    }

    {
        // should work!
        using namespace boost::spirit::ascii;
        BOOST_TEST(test("x", no_case[no_case[char_]]));
        BOOST_TEST(test("x", no_case[no_case[char_('x')]]));
        BOOST_TEST(test("yabadabadoo", no_case[no_case[lit("Yabadabadoo")]]));
    }

    {
        using namespace boost::spirit::ascii;
        BOOST_TEST(test("X", no_case[alnum]));
        BOOST_TEST(test("6", no_case[alnum]));
        BOOST_TEST(!test(":", no_case[alnum]));

        BOOST_TEST(test("X", no_case[lower]));
        BOOST_TEST(test("x", no_case[lower]));
        BOOST_TEST(test("X", no_case[upper]));
        BOOST_TEST(test("x", no_case[upper]));
        BOOST_TEST(!test(":", no_case[lower]));
        BOOST_TEST(!test(":", no_case[upper]));
    }

    {
        using namespace boost::spirit::iso8859_1;
        BOOST_TEST(test("X", no_case[alnum]));
        BOOST_TEST(test("6", no_case[alnum]));
        BOOST_TEST(!test(":", no_case[alnum]));

        BOOST_TEST(test("X", no_case[lower]));
        BOOST_TEST(test("x", no_case[lower]));
        BOOST_TEST(test("X", no_case[upper]));
        BOOST_TEST(test("x", no_case[upper]));
        BOOST_TEST(!test(":", no_case[lower]));
        BOOST_TEST(!test(":", no_case[upper]));
    }

    {
        using namespace boost::spirit::standard;
        BOOST_TEST(test("X", no_case[alnum]));
        BOOST_TEST(test("6", no_case[alnum]));
        BOOST_TEST(!test(":", no_case[alnum]));

        BOOST_TEST(test("X", no_case[lower]));
        BOOST_TEST(test("x", no_case[lower]));
        BOOST_TEST(test("X", no_case[upper]));
        BOOST_TEST(test("x", no_case[upper]));
        BOOST_TEST(!test(":", no_case[lower]));
        BOOST_TEST(!test(":", no_case[upper]));
    }

    {
        // chsets
        namespace standard = boost::spirit::standard;
        namespace standard_wide = boost::spirit::standard_wide;

        BOOST_TEST(test("x", standard::no_case[standard::char_("a-z")]));
        BOOST_TEST(test("X", standard::no_case[standard::char_("a-z")]));
        BOOST_TEST(test(L"X", standard_wide::no_case[standard_wide::char_(L"a-z")]));
        BOOST_TEST(test(L"X", standard_wide::no_case[standard_wide::char_(L"X")]));
    }

    {
        using namespace boost::spirit::standard;
        std::string s("bochi bochi");
        BOOST_TEST(test("Bochi Bochi", no_case[lit(s.c_str())]));
        BOOST_TEST(test("Bochi Bochi", no_case[lit(s)]));
        BOOST_TEST(test("Bochi Bochi", no_case[s.c_str()]));
        BOOST_TEST(test("Bochi Bochi", no_case[s]));
    }

    return boost::report_errors();
}
