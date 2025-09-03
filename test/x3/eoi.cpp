/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "test.hpp"

#include <boost/spirit/home/x3.hpp>

#include <iostream>

int main()
{
    using spirit_test::test;
    namespace x3 = boost::spirit::x3;
    using x3::eoi;

    BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(eoi);

    {
        BOOST_TEST((test("", eoi)));
        BOOST_TEST(!(test("x", eoi)));
    }

    {
        BOOST_TEST(x3::what(eoi) == "eoi");
    }

    return boost::report_errors();
}
