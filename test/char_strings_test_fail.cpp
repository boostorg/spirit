/*=============================================================================
    Copyright (c) 2004 Joao Abecasis
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/core/primitives/primitives.hpp>
#include <boost/spirit/core/non_terminal/rule.hpp>

int main()
{
    using boost::spirit::rule;
    using boost::spirit::ch_p;

    rule<> chars = ch_p("string");
}
