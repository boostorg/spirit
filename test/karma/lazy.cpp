//  Copyright (c) 2001-2008 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_auxiliary.hpp>
#include <boost/spirit/include/karma_action.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using namespace boost::spirit;
    using namespace boost::spirit::karma;
    using namespace boost::phoenix;
    using namespace boost::spirit::arg_names;

    {
        BOOST_TEST(test("123", lazy(val(int_)), 123));
    }

    {
        int result = 123;
        BOOST_TEST(test("123", lazy(val(int_))[_1 = ref(result)]));
    }

    {
        typedef spirit_test::output_iterator<char>::type outiter_type;
        rule<outiter_type, void(std::string)> r;

        r = char_('<') << lazy(_r1) << '>' <<  "</" << lazy(_r1) << '>';

        std::string tag("tag"), foo("foo");
        BOOST_TEST(test("<tag></tag>", r (ref(tag))));
        BOOST_TEST(!test("<foo></bar>", r (ref(foo))));
    }

    return boost::report_errors();
}
