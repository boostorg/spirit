/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "expression_def.hpp"

namespace client
{
    typedef std::string::const_iterator iterator_type;

    typedef x3::ascii::space_type skipper_type;
    typedef decltype(expression_parser<iterator_type>(x3::ascii::space)) expression_parser_type;

    template expression_parser_type
    expression_parser<iterator_type, skipper_type>(skipper_type const&);
}


