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
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
//
//  *** BEWARE PLATFORM DEPENDENT!!! ***
//  *** The following assumes 32 bit integers and 64 bit long longs.
//  *** Modify these constant strings when appropriate.
//
///////////////////////////////////////////////////////////////////////////////

    char const* max_unsigned = "4294967295";
    char const* unsigned_overflow = "4294967296";
    char const* max_int = "2147483647";
    char const* int_overflow = "2147483648";
    char const* min_int = "-2147483648";
    char const* int_underflow = "-2147483649";
    char const* max_binary = "11111111111111111111111111111111";
    char const* binary_overflow = "100000000000000000000000000000000";
    char const* max_octal = "37777777777";
    char const* octal_overflow = "100000000000";
    char const* max_hex = "FFFFFFFF";
    char const* hex_overflow = "100000000";

///////////////////////////////////////////////////////////////////////////////
// A custom int type
struct custom_int
{
    int n;
    custom_int() : n(0) {}
    explicit custom_int(int n_) : n(n_) {}
    custom_int& operator=(int n_) { n = n_; return *this; }
    friend custom_int operator*(custom_int a, custom_int b)
        { return custom_int(a.n * b.n); }
    friend custom_int operator+(custom_int a, custom_int b)
        { return custom_int(a.n + b.n); }
};

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    ///////////////////////////////////////////////////////////////////////////
    //  unsigned tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        unsigned u;

        BOOST_TEST(test("123456", uint_));
        BOOST_TEST(test_attr("123456", uint_, u));
        BOOST_TEST(u == 123456);

        BOOST_TEST(test(max_unsigned, uint_));
        BOOST_TEST(test_attr(max_unsigned, uint_, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!test(unsigned_overflow, uint_));
        BOOST_TEST(!test_attr(unsigned_overflow, uint_, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  binary tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::bin;
        unsigned u;

        BOOST_TEST(test("11111110", bin));
        BOOST_TEST(test_attr("11111110", bin, u));
        BOOST_TEST(u == 0xFE);

        BOOST_TEST(test(max_binary, bin));
        BOOST_TEST(test_attr(max_binary, bin, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!test(binary_overflow, bin));
        BOOST_TEST(!test_attr(binary_overflow, bin, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  octal tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::oct;
        unsigned u;

        BOOST_TEST(test("12545674515", oct));
        BOOST_TEST(test_attr("12545674515", oct, u));
        BOOST_TEST(u == 012545674515);

        BOOST_TEST(test(max_octal, oct));
        BOOST_TEST(test_attr(max_octal, oct, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!test(octal_overflow, oct));
        BOOST_TEST(!test_attr(octal_overflow, oct, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  hex tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::hex;
        unsigned u;

        BOOST_TEST(test("95BC8DF", hex));
        BOOST_TEST(test_attr("95BC8DF", hex, u));
        BOOST_TEST(u == 0x95BC8DF);

        BOOST_TEST(test("abcdef12", hex));
        BOOST_TEST(test_attr("abcdef12", hex, u));
        BOOST_TEST(u == 0xabcdef12);

        BOOST_TEST(test(max_hex, hex));
        BOOST_TEST(test_attr(max_hex, hex, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!test(hex_overflow, hex));
        BOOST_TEST(!test_attr(hex_overflow, hex, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  limited fieldwidth
    ///////////////////////////////////////////////////////////////////////////
    {
        unsigned u;
        using boost::spirit::qi::uint_parser;

        uint_parser<unsigned, 10, 1, 3> uint3;
        BOOST_TEST(test("123456", uint3, false));
        BOOST_TEST(test_attr("123456", uint3, u, false));
        BOOST_TEST(u == 123);

        uint_parser<unsigned, 10, 2, 4> uint4;
        BOOST_TEST(test("123456", uint4, false));
        BOOST_TEST(test_attr("123456", uint4, u, false));
        BOOST_TEST(u == 1234);

        char const * first = "0000000";
        char const * last  = first + std::strlen(first);
        uint_parser<unsigned, 10, 4, 4> uint_exact4;
        BOOST_TEST(boost::spirit::qi::parse(first, last, uint_exact4, u)
            && first != last && (last-first == 3) && u == 0);

        first = "0001400";
        last  = first + std::strlen(first);
        BOOST_TEST(boost::spirit::qi::parse(first, last, uint_exact4, u)
            && first != last && (last-first == 3) && u == 1);

        BOOST_TEST(!test("1", uint4));
        BOOST_TEST(!test_attr("1", uint4, u));
        BOOST_TEST(test_attr("014567", uint4, u, false) && u == 145);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  uint_parser<unused_type> tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_parser;
        using boost::spirit::qi::unused_type;
        uint_parser<unused_type> any_int;

        BOOST_TEST(test("123456", any_int));
        BOOST_TEST(test("1234567890123456789", any_int));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  action tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::phoenix::ref;
        using boost::spirit::qi::_1;
        using boost::spirit::qi::uint_;
        using boost::spirit::ascii::space;
        int n;

        BOOST_TEST(test("123", uint_[ref(n) = _1]));
        BOOST_TEST(n == 123);
        BOOST_TEST(test("   456", uint_[ref(n) = _1], space));
        BOOST_TEST(n == 456);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Check overflow is parse error
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::qi::uint_parser<boost::uint8_t> uint_;
        boost::uint8_t u;

        BOOST_TEST(!test_attr("999", uint_, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  custom uint tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        custom_int u;

        BOOST_TEST(test_attr("123456", uint_, u));

        uint_parser<custom_int, 10, 1, 2> uint2;
        BOOST_TEST(test_attr("12", uint2, u));
    }

    return boost::report_errors();
}
