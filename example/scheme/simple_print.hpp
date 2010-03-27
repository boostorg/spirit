/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(UTREE_SIMPLE_PRINT)
#define UTREE_SIMPLE_PRINT

#include "utree.hpp"
#include <iostream>

namespace detail
{
    inline std::ostream& println(std::ostream& out, scheme::utree const& val)
    {
        out << val << std::endl;
        return out;
    }
}

#endif