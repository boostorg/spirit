/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <climits>
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "test.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  *** BEWARE PLATFORM DEPENDENT!!! ***
//  *** The following assumes 32 bit or 64 bit integers and 64 bit long longs.
//  *** Modify these constant strings when appropriate.
//
///////////////////////////////////////////////////////////////////////////////
#ifdef BOOST_HAS_LONG_LONG
// Some compilers have long long, but don't define the
// LONG_LONG_MIN and LONG_LONG_MAX macros in limits.h.  This
// assumes that long long is 64 bits.

BOOST_STATIC_ASSERT(sizeof(boost::long_long_type) == 8);

#if !defined(LONG_LONG_MIN) && !defined(LONG_LONG_MAX)
# define LONG_LONG_MAX 0x7fffffffffffffffLL
# define LONG_LONG_MIN (-LONG_LONG_MAX - 1)
#endif

#endif // BOOST_HAS_LONG_LONG

#if INT_MAX != LLONG_MAX
    BOOST_STATIC_ASSERT(sizeof(int) == 4);
    char const* max_int = "2147483647";
    char const* int_overflow = "2147483648";
    char const* min_int = "-2147483648";
    char const* int_underflow = "-2147483649";
#else
    BOOST_STATIC_ASSERT(sizeof(int) == 8);
    char const* max_int = "9223372036854775807";
    char const* int_overflow = "9223372036854775808";
    char const* min_int = "-9223372036854775808";
    char const* int_underflow = "-9223372036854775809";
#endif

#ifdef BOOST_HAS_LONG_LONG
    char const* max_long_long = "9223372036854775807";
    char const* long_long_overflow = "9223372036854775808";
    char const* min_long_long = "-9223372036854775808";
    char const* long_long_underflow = "-9223372036854775809";
#endif

///////////////////////////////////////////////////////////////////////////////
// A custom int type
struct custom_int
{
    int n;
    custom_int() : n(0) {}
    explicit custom_int(int n_) : n(n_) {}
    custom_int& operator=(int n_) { n = n_; return *this; }
    friend custom_int operator*(custom_int a, custom_int b) { return custom_int(a.n * b.n); }
    friend custom_int operator+(custom_int a, custom_int b) { return custom_int(a.n + b.n); }
    friend custom_int operator-(custom_int a, custom_int b) { return custom_int(a.n - b.n); }
};

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;

    ///////////////////////////////////////////////////////////////////////////
    //  signed integer tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::int_;
        int i;

        BOOST_TEST(test("123456", int_));
        BOOST_TEST(test_attr("123456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(test("+123456", int_));
        BOOST_TEST(test_attr("+123456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(test("-123456", int_));
        BOOST_TEST(test_attr("-123456", int_, i));
        BOOST_TEST(i == -123456);

        BOOST_TEST(test(max_int, int_));
        BOOST_TEST(test_attr(max_int, int_, i));
        BOOST_TEST(i == INT_MAX);

        BOOST_TEST(test(min_int, int_));
        BOOST_TEST(test_attr(min_int, int_, i));
        BOOST_TEST(i == INT_MIN);

        BOOST_TEST(!test(int_overflow, int_));
        BOOST_TEST(!test_attr(int_overflow, int_, i));
        BOOST_TEST(!test(int_underflow, int_));
        BOOST_TEST(!test_attr(int_underflow, int_, i));

        BOOST_TEST(!test("-", int_));
        BOOST_TEST(!test_attr("-", int_, i));

        BOOST_TEST(!test("+", int_));
        BOOST_TEST(!test_attr("+", int_, i));

        // Bug report from Steve Nutt
        BOOST_TEST(test_attr("5368709120", int_, i, false));
        BOOST_TEST(i == 536870912);

        // with leading zeros
        BOOST_TEST(test("0000000000123456", int_));
        BOOST_TEST(test_attr("0000000000123456", int_, i));
        BOOST_TEST(i == 123456);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  long long tests
    ///////////////////////////////////////////////////////////////////////////
#ifdef BOOST_HAS_LONG_LONG
    {
        using boost::spirit::long_long;
        boost::long_long_type ll;

        BOOST_TEST(test("1234567890123456789", long_long));
        BOOST_TEST(test_attr("1234567890123456789", long_long, ll));
        BOOST_TEST(ll == 1234567890123456789LL);

        BOOST_TEST(test("-1234567890123456789", long_long));
        BOOST_TEST(test_attr("-1234567890123456789", long_long, ll));
        BOOST_TEST(ll == -1234567890123456789LL);

        BOOST_TEST(test(max_long_long, long_long));
        BOOST_TEST(test_attr(max_long_long, long_long, ll));
        BOOST_TEST(ll == LONG_LONG_MAX);

        BOOST_TEST(test(min_long_long, long_long));
        BOOST_TEST(test_attr(min_long_long, long_long, ll));
        BOOST_TEST(ll == LONG_LONG_MIN);

        BOOST_TEST(!test(long_long_overflow, long_long));
        BOOST_TEST(!test_attr(long_long_overflow, long_long, ll));
        BOOST_TEST(!test(long_long_underflow, long_long));
        BOOST_TEST(!test_attr(long_long_underflow, long_long, ll));
    }
#endif

    ///////////////////////////////////////////////////////////////////////////
    //  short_ and long_ tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::short_;
        using boost::spirit::long_;
        int i;

        BOOST_TEST(test("12345", short_));
        BOOST_TEST(test_attr("12345", short_, i));
        BOOST_TEST(i == 12345);

        BOOST_TEST(test("1234567890", long_));
        BOOST_TEST(test_attr("1234567890", long_, i));
        BOOST_TEST(i == 1234567890);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Check overflow is parse error
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::qi::int_parser<boost::int8_t> int8_;
        char c;

        BOOST_TEST(!test_attr("999", int8_, c));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  int_parser<unused_type> tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::int_parser;
        using boost::spirit::unused_type;
        int_parser<unused_type> any_int;

        BOOST_TEST(test("123456", any_int));
        BOOST_TEST(test("-123456", any_int));
        BOOST_TEST(test("-1234567890123456789", any_int));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  action tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::phoenix::ref;
        using boost::spirit::_1;
        using boost::spirit::ascii::space;
        using boost::spirit::int_;
        int n, m;

        BOOST_TEST(test("123", int_[ref(n) = _1]));
        BOOST_TEST(n == 123);
        BOOST_TEST(test_attr("789", int_[ref(n) = _1], m));
        BOOST_TEST(n == 789 && m == 789);
        BOOST_TEST(test("   456", int_[ref(n) = _1], space));
        BOOST_TEST(n == 456);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  custom int tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::int_;
        using boost::spirit::qi::int_parser;
        custom_int i;

        BOOST_TEST(test_attr("-123456", int_, i));
        int_parser<custom_int, 10, 1, 2> int2;
        BOOST_TEST(test_attr("-12", int2, i));
    }

    return boost::report_errors();
}
