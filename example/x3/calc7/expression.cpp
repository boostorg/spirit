/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "expression_def.hpp"

namespace client
{
    typedef std::string::const_iterator iterator_type;

    typedef x3::ascii::space_type const skipper_type; 
    typedef x3::any_parser<
        iterator_type, ast::expression
      , x3::context<x3::skipper_tag, skipper_type>>
    expression_type;

    template
    expression_type
    expression<iterator_type, skipper_type>();
}
