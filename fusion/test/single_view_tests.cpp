/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/single_view.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>

struct X {};

template <typename OS>
OS& operator<<(OS& os, X const&)
{
    os << "<X-object>";
    return os;
}

int
main()
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing single_view

    {
        single_view<int> view1(3);
        std::cout << view1 << std::endl;
        
        // allow single_view element to be modified
        *begin(view1) += 4;
        std::cout << view1 << std::endl;
        BOOST_TEST(*begin(view1) == 7);
        BOOST_TEST(view1.val == 7);

        single_view<X> view2;
        std::cout << view2 << std::endl;
    }

    return boost::report_errors();
}

