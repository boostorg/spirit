/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
//~ #include <boost/phoenix/core.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using boost::spirit::x3::eps;
    using boost::spirit::x3::unused_type;

    {
        BOOST_TEST((test("", eps)));
        BOOST_TEST((test("xxx", eps, false)));
        //~ BOOST_TEST((!test("", !eps))); // not predicate $$$ Implement me! $$$
    }

    {   // test non-lazy semantic predicate

        BOOST_TEST((test("", eps(true))));
        BOOST_TEST((!test("", eps(false))));
        //~ BOOST_TEST((test("", !eps(false)))); // not predicate $$$ Implement me! $$$
    }

    {   // test lazy semantic predicate

        auto ft = [](unused_type, unused_type, unused_type, unused_type)
        { return true; };
        auto ff = [](unused_type, unused_type, unused_type, unused_type)
        { return false; };

        BOOST_TEST((test("", eps(ft))));
        BOOST_TEST((!test("", eps(ff))));
        //~ BOOST_TEST((test("", !eps(val(false))))); // not predicate $$$ Implement me! $$$
    }

    //~ {   // test lazy semantic predicate

        //~ using boost::phoenix::val;

        //~ BOOST_TEST((test("", eps(val(true)))));
        //~ BOOST_TEST((!test("", eps(val(false)))));
        //~ BOOST_TEST((test("", !eps(val(false))))); // not predicate
    //~ }

    return boost::report_errors();
}
