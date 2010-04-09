//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>

#include "../../output/generate_sexpr.hpp"
#include "../../output/generate_sexpr_impl.hpp"

// explicit template instantiation for the function parse_sexpr 
namespace scheme { namespace output
{
    template bool generate_sexpr(BOOST_TYPEOF(std::cout)&, utree const& result);
}}

