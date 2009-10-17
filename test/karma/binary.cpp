//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_binary.hpp>
#include <boost/spirit/include/karma_generate.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost::spirit;
    using namespace boost::phoenix;

    {   // test native endian binaries
#ifdef BOOST_LITTLE_ENDIAN
        BOOST_TEST(binary_test("\x01", 1, byte_, 0x01));
        BOOST_TEST(binary_test("\x01\x02", 2, word, 0x0201));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, 0x04030201));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword,
            0x0807060504030201LL));
#endif

        BOOST_TEST(binary_test_delimited("\x01\x00\x00\x00", 4, byte_, 0x01, pad(4)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x00\x00", 4, word, 0x0201, pad(4)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04", 4, dword, 0x04030201, pad(4)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 10, 
            qword, 0x0807060504030201LL, pad(10)));
#endif

#else // BOOST_LITTLE_ENDIAN

        BOOST_TEST(binary_test("\x01", 1, byte_, 0x01));
        BOOST_TEST(binary_test("\x01\x02", 2, word, 0x0102));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, 0x01020304));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword,
            0x0102030405060708LL));
#endif

        BOOST_TEST(binary_test_delimited("\x01\x00\x00\x00", 4, byte_, 0x01, pad(4)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x00\x00", 4, word, 0x0102, pad(4)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04", 4, dword, 0x01020304, pad(4)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 10, 
            qword, 0x0102030405060708LL, pad(10)));
#endif
#endif
    }

    {   // test native endian binaries
#ifdef BOOST_LITTLE_ENDIAN
        BOOST_TEST(binary_test("\x01", 1, byte_(0x01)));
        BOOST_TEST(binary_test("\x01\x02", 2, word(0x0201)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword(0x04030201)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8,
            qword(0x0807060504030201LL)));
#endif
#else
        BOOST_TEST(binary_test("\x01", 1, byte_(0x01)));
        BOOST_TEST(binary_test("\x01\x02", 2, word(0x0102)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword(0x01020304)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8,
            qword(0x0102030405060708LL)));
#endif
#endif
    }

    {   // test big endian binaries
        BOOST_TEST(binary_test("\x01\x02", 2, big_word, 0x0102));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, big_dword, 0x01020304));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, big_qword,
            0x0102030405060708LL));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 
            10, big_qword, 0x0102030405060708LL, pad(10)));
#endif
    }

    {
        BOOST_TEST(binary_test("\x01\x02", 2, big_word(0x0102)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, big_dword(0x01020304)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8,
            big_qword(0x0102030405060708LL)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 
            10, big_qword(0x0102030405060708LL), pad(10)));
#endif
    }

    {   // test little endian binaries
        BOOST_TEST(binary_test("\x01\x02", 2, little_word, 0x0201));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, little_dword, 0x04030201));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, little_qword,
            0x0807060504030201LL));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 
            10, little_qword, 0x0807060504030201LL, pad(10)));
#endif
    }

    {
        BOOST_TEST(binary_test("\x01\x02", 2, little_word(0x0201)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, little_dword(0x04030201)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8,
            little_qword(0x0807060504030201LL)));
        BOOST_TEST(binary_test_delimited("\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00", 
            10, little_qword(0x0807060504030201LL), pad(10)));
#endif
    }

    {   // test native endian binaries
        boost::optional<boost::uint8_t> v8;
        boost::optional<boost::uint16_t> v16;
        boost::optional<boost::uint32_t> v32;

#ifdef BOOST_LITTLE_ENDIAN

        BOOST_TEST(!binary_test("", 1, byte_, v8));
        BOOST_TEST(!binary_test("", 2, word, v16));
        BOOST_TEST(!binary_test("", 4, dword, v32));
#ifdef BOOST_HAS_LONG_LONG
        boost::optional<boost::uint64_t> v64;
        BOOST_TEST(!binary_test("", 8, qword, v64));
#endif

#else // BOOST_LITTLE_ENDIAN

        BOOST_TEST(!binary_test("", 1, byte_, v8));
        BOOST_TEST(!binary_test("", 2, word, v16));
        BOOST_TEST(!binary_test("", 4, dword, v32));
#ifdef BOOST_HAS_LONG_LONG
        boost::optional<boost::uint64_t> v64;
        BOOST_TEST(!binary_test("", 8, qword, v64));
#endif

#endif
    }

    {   // test optional attributes

#ifdef BOOST_LITTLE_ENDIAN
        boost::optional<boost::uint8_t> v8 (0x01);
        BOOST_TEST(binary_test("\x01", 1, byte_, v8));
        boost::optional<boost::uint16_t> v16 (0x0201);
        BOOST_TEST(binary_test("\x01\x02", 2, word, v16));
        boost::optional<boost::uint32_t> v32 (0x04030201);
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, v32));
#ifdef BOOST_HAS_LONG_LONG
        boost::optional<boost::uint64_t> v64 (0x0807060504030201LL);
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, v64));
#endif

#else // BOOST_LITTLE_ENDIAN

        boost::optional<boost::uint8_t> v8 (0x01);
        BOOST_TEST(binary_test("\x01", 1, byte_, v8));
        boost::optional<boost::uint16_t> v16 (0x0102);
        BOOST_TEST(binary_test("\x01\x02", 2, word, v16));
        boost::optional<boost::uint32_t> v32 (0x01020304);
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, v32));
#ifdef BOOST_HAS_LONG_LONG
        boost::optional<boost::uint64_t> v64 (0x0102030405060708LL);
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, v64));
#endif

#endif
    }

    return boost::report_errors();
}
