/*=============================================================================
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

using namespace boost;
using namespace spirit;

int main()
{
    rule<parser_context<int> > int_rule = int_p;
    int result(0);
    parse("123", int_rule[assign_a(result)]); 
    BOOST_TEST(result == 123);
    return boost::report_errors();
}

