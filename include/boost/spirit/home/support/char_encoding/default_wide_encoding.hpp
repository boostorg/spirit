/*=============================================================================
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_DEFAULT_WIDE_ENCODING_MARCH_05_2010_1041AM)
#define BOOST_SPIRIT_DEFAULT_WIDE_ENCODING_MARCH_05_2010_1041AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>

namespace boost { namespace spirit { namespace char_encoding
{
    ///////////////////////////////////////////////////////////////////////////
    //  default_wide_encoding uses char_encoding::standard
    ///////////////////////////////////////////////////////////////////////////
    struct default_wide_encoding : char_encoding::standard_wide
    {
    };
}}}

#endif

