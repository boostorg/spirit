/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/tuple.hpp>
#include <boost/spirit/fusion/generate.hpp>
#include <boost/spirit/fusion/io.hpp>
#include <boost/spirit/fusion/single_view.hpp>

struct X
{
    operator char const*() const
    {
        return "<X-object>";
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing single_view

    {
        single_view<int> view1(3);
        std::cout << generate(view1) << std::endl;

        single_view<X> view2;
        std::cout << generate(view2) << std::endl;
    }

    return 0;
}

