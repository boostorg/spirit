/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "rule_separate_tu_grammar.hpp"

#include <boost/core/lightweight_test.hpp>

#include "test.hpp"

int main()
{
    using spirit_test::test;
    using spirit_test::test_attr;

    {
        BOOST_TEST(test("*", unused_attr::skipper));
        BOOST_TEST(test("#", unused_attr::skipper2));
        BOOST_TEST(test("==", unused_attr::grammar));
        BOOST_TEST(test("*=*=", unused_attr::grammar, unused_attr::skipper));
        BOOST_TEST(test("#=#=", unused_attr::grammar, unused_attr::skipper2));
    }

    {
        int i;
        BOOST_TEST(test_attr("123", used_attr::grammar, i));
        BOOST_TEST_EQ(i, 123);
        BOOST_TEST(test_attr(" 42", used_attr::grammar, i, used_attr::skipper));
        BOOST_TEST_EQ(i, 42);
    }

    return boost::report_errors();
}
