/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <cmath>
#include <boost/noncopyable.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace std;
namespace phx = boost::phoenix;

namespace test
{
    struct x : boost::noncopyable // test non-copyable (hold this by reference)
    {
        int m;
    };

    struct xx {
       int m;
    };
}

int
main()
{
    {
        test::x x_;
        bind(&test::x::m, x_)() = 123;
        bind(&test::x::m, arg1)(x_) = 123;
        BOOST_TEST(x_.m == 123);
    }
    {
        test::xx x_= {0};
        bind(&test::xx::m, val(x_))(); // does not compile
        bind(&test::xx::m, ref(x_))() = 1;
        bind(&test::xx::m, cref(x_))();
    }

    return boost::report_errors();
}
