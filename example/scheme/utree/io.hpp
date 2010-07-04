//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_UTREE_IO)
#define BOOST_SPIRIT_UTREE_IO

#include <utree/utree.hpp>
#include <utree/operators.hpp>
#include <input/parse_sexpr_impl.hpp>
#include <output/generate_sexpr_impl.hpp>

namespace scheme
{
    // Printing
    inline std::ostream& operator<<(std::ostream& out, utree const& x)
    {
        output::generate_sexpr(out, x);
        return out;
    }

    // Parsing
    inline std::istream& operator>>(std::istream& in, utree& x)
    {
        input::parse_sexpr(in, x);
        return in;
    }
}

#endif
