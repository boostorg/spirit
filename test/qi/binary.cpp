/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/qi_binary.hpp>
#include <boost/cstdint.hpp>
#include "test.hpp"

///////////////////////////////////////////////////////////////////////////////
int main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    using boost::spirit::qi::byte_;
    using boost::spirit::qi::word;
    using boost::spirit::qi::dword;
    using boost::spirit::qi::big_word;
    using boost::spirit::qi::big_dword;
    using boost::spirit::qi::little_word;
    using boost::spirit::qi::little_dword;
#ifdef BOOST_HAS_LONG_LONG
    using boost::spirit::qi::qword;
    using boost::spirit::qi::big_qword;
    using boost::spirit::qi::little_qword;
#endif

    boost::uint8_t uc;
    boost::uint16_t us;
    boost::uint32_t ui;
#ifdef BOOST_HAS_LONG_LONG
    boost::uint64_t ul;
#endif

    {   // test native endian binaries
#ifdef BOOST_LITTLE_ENDIAN
        BOOST_TEST(test_attr("\x01", byte_, uc) && uc == 0x01);
        BOOST_TEST(test_attr("\x01\x02", word, us) && us == 0x0201);
        BOOST_TEST(test_attr("\x01\x02\x03\x04", dword, ui) && ui == 0x04030201);
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test_attr("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul) &&
            ul == 0x0807060504030201LL);
#endif
#else
        BOOST_TEST(test_attr("\x01", byte_, uc) && uc == 0x01);
        BOOST_TEST(test_attr("\x01\x02", word, us) && us ==  0x0102);
        BOOST_TEST(test_attr("\x01\x02\x03\x04", dword, ui) && ui == 0x01020304);
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test_attr("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul) &&
            ul == 0x0102030405060708LL);
#endif
#endif
    }

    {   // test native endian binaries
#ifdef BOOST_LITTLE_ENDIAN
        BOOST_TEST(test("\x01", byte_(0x01)));
        BOOST_TEST(test("\x01\x02", word(0x0201)));
        BOOST_TEST(test("\x01\x02\x03\x04", dword(0x04030201)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test("\x01\x02\x03\x04\x05\x06\x07\x08",
            qword(0x0807060504030201LL)));
#endif
#else
        BOOST_TEST(test("\x01", byte_(0x01)));
        BOOST_TEST(test("\x01\x02", word(0x0102)));
        BOOST_TEST(test("\x01\x02\x03\x04", dword(0x01020304)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test("\x01\x02\x03\x04\x05\x06\x07\x08",
            qword(0x0102030405060708LL)));
#endif
#endif
    }

    {   // test big endian binaries
        BOOST_TEST(test_attr("\x01\x02", big_word, us) && us == 0x0102);
        BOOST_TEST(test_attr("\x01\x02\x03\x04", big_dword, ui) && ui == 0x01020304);
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test_attr("\x01\x02\x03\x04\x05\x06\x07\x08", big_qword, ul)
            && ul == 0x0102030405060708LL);
#endif
    }

    {
        BOOST_TEST(test("\x01\x02", big_word(0x0102)));
        BOOST_TEST(test("\x01\x02\x03\x04", big_dword(0x01020304)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test("\x01\x02\x03\x04\x05\x06\x07\x08",
            big_qword(0x0102030405060708LL)));
#endif
    }

    {   // test little endian binaries
        BOOST_TEST(test_attr("\x01\x02", little_word, us) && us == 0x0201);
        BOOST_TEST(test_attr("\x01\x02\x03\x04", little_dword, ui) && ui == 0x04030201);
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test_attr("\x01\x02\x03\x04\x05\x06\x07\x08", little_qword, ul)
            && ul == 0x0807060504030201LL);
#endif
    }

    {
        BOOST_TEST(test("\x01\x02", little_word(0x0201)));
        BOOST_TEST(test("\x01\x02\x03\x04", little_dword(0x04030201)));
#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(test("\x01\x02\x03\x04\x05\x06\x07\x08",
            little_qword(0x0807060504030201LL)));
#endif
    }

    return boost::report_errors();
}
