/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/confix.hpp>

using namespace boost;
using namespace unit_test_framework;
using namespace spirit;

//-----------------------------------------------------------------------------

typedef
    scanner<char const*, scanner_policies<skipper_iteration_policy<> > >
        scanner_t;

typedef
    rule<scanner_t>
        rule_t;

//-----------------------------------------------------------------------------

void comment_nest_p_test()
{
    rule_t r = comment_nest_p('{', '}');

    {
        parse_info<> info = parse("{a{b}c{d}e}", r, space_p);
        BOOST_CHECK(info.full);
    }

    {
        parse_info<> info = parse("{a{b}c{d}e}x", r, space_p);
        BOOST_CHECK(info.hit);
        BOOST_CHECK(info.length == 11);
    }

    {
        char const* str = "x{a{b}c{d}e}";
        parse_info<> info = parse(str, r, space_p);
        BOOST_CHECK(!info.hit);
        BOOST_CHECK(info.stop == str);
    }

    {
        char const* str = "{a{b}c{d}e";
        parse_info<> info = parse(str, r, space_p);
        BOOST_CHECK(!info.hit);
        BOOST_CHECK(info.stop == (str + 10));
    }
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int argc, char* argv[])
{
    test_suite* test = BOOST_TEST_SUITE("confix tests");

    test->add(BOOST_TEST_CASE(&comment_nest_p_test));

    return test;
}

//-----------------------------------------------------------------------------
