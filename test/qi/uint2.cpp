/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "uint.hpp"

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
        unsigned u = 123456;

        BOOST_TEST( test("123456", uint_(123456)));
        BOOST_TEST(!test("123456", uint_(4321)));
        BOOST_TEST( test("123456", uint_(u)));
        BOOST_TEST(!test("123456", uint_(u - 1)));

        BOOST_TEST(test(max_unsigned, uint_(UINT_MAX)));

        BOOST_TEST(!test(unsigned_overflow, uint_(345)));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  binary tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::bin;
        unsigned u = 0xFE;

        BOOST_TEST( test("11111110", bin(0xFE)));
        BOOST_TEST(!test("11111110", bin(0xEF)));
        BOOST_TEST( test("11111110", bin(u)));
        BOOST_TEST(!test("11111110", bin(u - 1)));

        BOOST_TEST(test(max_binary, bin(UINT_MAX)));

        BOOST_TEST(!test(binary_overflow, bin(9)));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  octal literal tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::oct;
        unsigned u = 012545674515;

        BOOST_TEST( test("12545674515", oct(012545674515)));
        BOOST_TEST(!test("12545674515", oct(051554521)));
        BOOST_TEST( test("12545674515", oct(u)));
        BOOST_TEST(!test("12545674515", oct(u + 1)));

        BOOST_TEST(test(max_octal, oct(UINT_MAX)));

        BOOST_TEST(!test(octal_overflow, oct(12)));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  hex literal tests
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::hex;
        unsigned u = 0x95BC8DF;

        BOOST_TEST( test("95BC8DF", hex(0x95BC8DF)));
        BOOST_TEST(!test("95BC8DF", hex(0xFD8C9)));
        BOOST_TEST( test("95BC8DF", hex(u)));
        BOOST_TEST(!test("95BC8DF", hex(u + 1)));

        BOOST_TEST( test("abcdef12", hex(0xabcdef12)));
        BOOST_TEST(!test("abcdef12", hex(0x12abcdef)));

        BOOST_TEST(test(max_hex, hex(UINT_MAX)));

        BOOST_TEST(!test(hex_overflow, hex(0xdd)));
    }

    return boost::report_errors();
}
