/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;

    using boost::spirit::qi::_1;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::omit;
    using boost::spirit::ascii::char_;

    {
        BOOST_TEST((test("1234", -int_)));
        BOOST_TEST((test("abcd", -int_, false)));
    }

    {   // test propagation of unused
        using boost::fusion::at_c;
        using boost::fusion::vector;

        vector<char, char> v;
        BOOST_TEST((test_attr("a1234c", char_ >> -omit[int_] >> char_, v)));
        BOOST_TEST((at_c<0>(v) == 'a'));
        BOOST_TEST((at_c<1>(v) == 'c'));

        v = boost::fusion::vector<char, char>();
        BOOST_TEST((test_attr("a1234c", char_ >> omit[-int_] >> char_, v)));
        BOOST_TEST((at_c<0>(v) == 'a'));
        BOOST_TEST((at_c<1>(v) == 'c'));

        char ch;
        BOOST_TEST((test_attr(",c", -(',' >> char_), ch)));
        BOOST_TEST((ch == 'c'));
    }

    {   // test action

        namespace phx = boost::phoenix;

        boost::optional<int> n = 0;
        BOOST_TEST((test("1234", (-int_)[phx::ref(n) = _1])));
        BOOST_TEST(n.get() == 1234);

        n = boost::optional<int>();
        BOOST_TEST((test("abcd", (-int_)[phx::ref(n) = _1], false)));
        BOOST_TEST(!n);
    }

    return boost::report_errors();
}
