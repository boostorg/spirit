//
//  Copyright (c) 2004 Joao Abecasis
//
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/spirit/core.hpp>
#include <boost/test/included/unit_test_framework.hpp>

using namespace boost::unit_test_framework;
using namespace boost::spirit;

void shortest_alternative_parser_test()
{
    typedef
        shortest_alternative<
            shortest_alternative<
                shortest_alternative<
                    strlit<>,
                    strlit<> >,
                strlit<> >,
            strlit<> >
    parser_t;

    parser_t short_rule =
        shortest_d[
            str_p("a")
            | str_p("aa")
            | str_p("aaa")
            | str_p("aaaa")
        ];

    BOOST_CHECK(parse("a", short_rule).full);
    BOOST_CHECK(parse("aa", short_rule).length == 1);
    BOOST_CHECK(parse("aaa", short_rule).length == 1);
    BOOST_CHECK(parse("aaaa", short_rule).length == 1);

    short_rule =
        shortest_d[
            str_p("d")
            | str_p("cd")
            | str_p("bcd")
            | str_p("abcd")
        ];

    BOOST_CHECK(parse("d", short_rule).full);
    BOOST_CHECK(parse("cd", short_rule).full);
    BOOST_CHECK(parse("bcd", short_rule).full);
    BOOST_CHECK(parse("abcd", short_rule).full);

}

test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/[])
{
    test_suite* test = BOOST_TEST_SUITE("shortest_alternative parser test");

    test->add(BOOST_TEST_CASE(&shortest_alternative_parser_test));

    return test;
}
