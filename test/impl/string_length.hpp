/*=============================================================================
    Copyright (c) 2004 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_TEST_IMPL_STRING_len_HPP)
#define SPIRIT_TEST_IMPL_STRING_len_HPP

// We use our own string_len function instead of std::strlen
// to avoid the namespace confusion on different compilers. Some
// have itin namespace std. Some have it in global namespace. 
// Some have it in both.
namespace
{
    template <typename Char>
    inline int
    string_length(Char const* str)
    {
        int len = 0;
        while (*str++)
            ++len;
        return len;
    }
}

#endif

