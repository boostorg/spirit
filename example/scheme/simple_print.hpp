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