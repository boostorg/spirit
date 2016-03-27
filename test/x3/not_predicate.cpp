/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/phoenix/core.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::x3::alpha;
    using boost::spirit::x3::int_;
    using boost::spirit::x3::lexeme;

    {
        BOOST_TEST((!test("1234", !int_)));
        BOOST_TEST((test("abcd", !int_, false)));
        BOOST_TEST((!test("abcd", !!int_, false)));
    }

    // Test attributes
    {
        std::string attr1, attr2;
        BOOST_TEST(test_attr("abc", !int_ >> lexeme[alpha >> *alpha], attr1));
        BOOST_TEST(test_attr("abc", lexeme[alpha >> *alpha]  -  int_, attr2));
        BOOST_TEST_EQ(attr1, attr2);
    }

    return boost::report_errors();
}
