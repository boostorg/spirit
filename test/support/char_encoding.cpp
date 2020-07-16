/*=============================================================================
    Copyright (c) 2020 Nikita Kniazev

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>

#include <boost/core/lightweight_test.hpp>

#if defined(_MSC_VER) && _MSC_VER < 1700
# pragma warning(disable: 4428) // universal-character-name encountered in source
#endif

int main()
{
    {
        using boost::spirit::char_encoding::standard_wide;
        BOOST_TEST(0xFFFFu == standard_wide::toucs4(L'\uFFFF'));
        BOOST_TEST(0x7FFFu == standard_wide::toucs4(L'\u7FFF'));
        BOOST_TEST(0x0000u == standard_wide::toucs4(L'\u0000'));
    }

    return boost::report_errors();
}
