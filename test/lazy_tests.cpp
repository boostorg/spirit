/*=============================================================================
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/dynamic/lazy.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/detail/lightweight_test.hpp>

using namespace boost;
using namespace spirit;
using namespace phoenix;

int main()
{
    int result;
    BOOST_TEST(parse("123", lazy_p(val(int_p))[assign_a(result)]).full);
    BOOST_TEST((result == 123));
    return boost::report_errors();
}

