/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/algorithm/push_back.hpp>
#include <boost/mpl/vector_c.hpp>
#include <string>

int
main()
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing push_back


    {
        typedef boost::mpl::vector_c<int, 1, 2, 3, 4, 5> mpl_vec;
        std::cout << boost::fusion::push_back(mpl_vec(), boost::mpl::int_<6>()) << std::endl;
        BOOST_TEST((boost::fusion::push_back(mpl_vec(), boost::mpl::int_<6>())
            == make_tuple(1, 2, 3, 4, 5, 6)));
    }

    return boost::report_errors();
}

