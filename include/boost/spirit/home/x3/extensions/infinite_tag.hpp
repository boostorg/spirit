/*=============================================================================
    Copyright (c) 2014 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_X3_INFINITE_TAG_JUNE_29_2014_0245PM)
#define SPIRIT_X3_INFINITE_TAG_JUNE_29_2014_0245PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace x3
{
    // Infinite loop tag type
    struct inf_type {};
    const inf_type inf = inf_type();
}}}

#endif
