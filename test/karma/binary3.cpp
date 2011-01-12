//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_binary.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_phoenix_attributes.hpp>

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

// we support Phoenix attributes only starting with V2.2
#if SPIRIT_VERSION >= 0x2020
    {   // test Phoenix expression attributes, only supported if 
        // karma_phoenix_attributes.hpp is included
        namespace phoenix = boost::phoenix;

#ifdef BOOST_LITTLE_ENDIAN
        BOOST_TEST(binary_test("\x01", 1, byte_, phoenix::val(0x01)));
        BOOST_TEST(binary_test("\x01\x02", 2, word, phoenix::val(0x0201)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, 
            phoenix::val(0x04030201)));

        boost::uint8_t v8 (0x01);
        BOOST_TEST(binary_test("\x01", 1, byte_, phoenix::ref(v8)));
        BOOST_TEST(binary_test("\x02", 1, byte_, ++phoenix::ref(v8)));

        boost::uint16_t v16 (0x0201);
        BOOST_TEST(binary_test("\x01\x02", 2, word, phoenix::ref(v16)));
        BOOST_TEST(binary_test("\x02\x02", 2, word, ++phoenix::ref(v16)));

        boost::uint32_t v32 (0x04030201);
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, phoenix::ref(v32)));
        BOOST_TEST(binary_test("\x02\x02\x03\x04", 4, dword, ++phoenix::ref(v32)));

#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, 
            phoenix::val(0x0807060504030201LL)));

        boost::uint64_t v64 (0x0807060504030201LL);
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, 
            phoenix::ref(v64)));
        BOOST_TEST(binary_test("\x02\x02\x03\x04\x05\x06\x07\x08", 8, qword, 
            ++phoenix::ref(v64)));
#endif

#else // BOOST_LITTLE_ENDIAN

        BOOST_TEST(binary_test("\x01", 1, byte_, phoenix::val(0x01)));
        BOOST_TEST(binary_test("\x01\x02", 2, word, phoenix::val(0x0102)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, 
            phoenix::val(0x01020304)));

        boost::uint8_t v8 (0x01);
        BOOST_TEST(binary_test("\x01", 1, byte_, phoenix::ref(v8)));
        BOOST_TEST(binary_test("\x02", 1, byte_, ++phoenix::ref(v8)));

        boost::uint16_t v16 (0x0102);
        BOOST_TEST(binary_test("\x01\x02", 2, word, phoenix::ref(v16)));
        BOOST_TEST(binary_test("\x01\x03", 2, word, ++phoenix::ref(v16)));

        boost::uint32_t v32 (0x01020304);
        BOOST_TEST(binary_test("\x01\x02\x03\x04", 4, dword, phoenix::ref(v32)));
        BOOST_TEST(binary_test("\x01\x02\x03\x05", 4, dword, ++phoenix::ref(v32)));

#ifdef BOOST_HAS_LONG_LONG
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, 
            phoenix::val(0x0102030405060708LL)));

        boost::uint64_t v64 (0x0102030405060708LL);
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, 
            phoenix::ref(v64)));
        BOOST_TEST(binary_test("\x01\x02\x03\x04\x05\x06\x07\x09", 8, qword, 
            ++phoenix::ref(v64)));
#endif

#endif
    }
#endif

    return boost::report_errors();
}
