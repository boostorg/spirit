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
    using x3::eol;

    BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(eol);

    {
        BOOST_TEST((test("\r\n", eol)));
        BOOST_TEST((test("\r", eol)));
        BOOST_TEST((test("\n", eol)));
        BOOST_TEST((!test("\n\r", eol)));
        BOOST_TEST((!test("", eol)));
    }

    {
        BOOST_TEST(x3::what(eol) == "eol");
    }

    return boost::report_errors();
}
