///////////////////////////////////////////////////////////////////////////////
//
// has_reverse.hpp
//
/////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1000
#pragma once
#endif

#ifndef HAS_REVERSE_EN_14_12_2004
#define HAS_REVERSE_EN_14_12_2004

#include "./is_std_list.hpp"

namespace boost
{
    // Specialize this for user-defined types
    template<typename T>
    struct has_reverse
        : is_std_list<T>
    {
    };
}

#endif
