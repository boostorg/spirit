/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "int.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;

    ///////////////////////////////////////////////////////////////////////////
    //  signed integer literal tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::int_;

        int i = 123456;

        BOOST_TEST( test("123456", int_(123456)));
        BOOST_TEST(!test("123456", int_(0)));
        
        BOOST_TEST( test("123456", int_(i)));
        BOOST_TEST(!test("123456", int_(-i)));

        BOOST_TEST( test("+425", int_(425)));
        BOOST_TEST(!test("+425", int_(17)));

        BOOST_TEST( test("-2000", int_(-2000)));
        BOOST_TEST(!test("-2000", int_(2000)));

        BOOST_TEST( test(max_int, int_(INT_MAX)));
        BOOST_TEST(!test(max_int, int_(INT_MIN)));

        BOOST_TEST( test(min_int, int_(INT_MIN)));
        BOOST_TEST(!test(min_int, int_(INT_MAX)));

        BOOST_TEST(!test("-", int_(8451)));
        BOOST_TEST(!test("+", int_(8451)));

        // with leading zeros
        BOOST_TEST(test("000000000098765", int_(98765)));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  long long literal tests
    ///////////////////////////////////////////////////////////////////////////
#ifdef BOOST_HAS_LONG_LONG
    {
        using boost::spirit::long_long;
        boost::long_long_type ll = 1234567890123456789LL;

        BOOST_TEST( test("1234567890123456789", long_long(1234567890123456789LL)));
        BOOST_TEST(!test("1234567890123456789", long_long(-19LL)));
        
        BOOST_TEST( test("1234567890123456789", long_long(ll)));
        BOOST_TEST(!test("1234567890123456789", long_long(-ll)));

        BOOST_TEST( test("-100000000000000", long_long(-100000000000000LL)));
        BOOST_TEST(!test("-100000000000000", long_long(3243515525263LL)));

        BOOST_TEST( test(max_long_long, long_long(LONG_LONG_MAX)));
        BOOST_TEST(!test(max_long_long, long_long(LONG_LONG_MIN)));

        BOOST_TEST( test(min_long_long, long_long(LONG_LONG_MIN)));
        BOOST_TEST(!test(min_long_long, long_long(LONG_LONG_MAX)));
    }
#endif

    ///////////////////////////////////////////////////////////////////////////
    //  short_ and long_ literal tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::short_;
        using boost::spirit::long_;
        short s = 12345;
        long l = 1234567890L;

        BOOST_TEST( test("12345",  short_(12345)));
        BOOST_TEST(!test("12345",  short_(-12345)));
        BOOST_TEST( test("12345",  short_(s)));
        BOOST_TEST(!test("12345",  short_(-s)));

        BOOST_TEST( test("-12345", short_(-12345)));
        BOOST_TEST(!test("-12345", short_(12345)));
        BOOST_TEST( test("-12345", short_(-s)));
        BOOST_TEST(!test("-12345", short_(s)));

        BOOST_TEST( test("1234567890",  long_(1234567890)));
        BOOST_TEST(!test("1234567890",  long_(-1234567890)));
        BOOST_TEST( test("1234567890",  long_(l)));
        BOOST_TEST(!test("1234567890",  long_(-l)));

        BOOST_TEST( test("-1234567890", long_(-1234567890)));
        BOOST_TEST(!test("-1234567890", long_(1234567890)));
        BOOST_TEST( test("-1234567890", long_(-l)));
        BOOST_TEST(!test("-1234567890", long_(l)));
    }

    return boost::report_errors();
}
