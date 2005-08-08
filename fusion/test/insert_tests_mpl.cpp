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
#include <boost/spirit/fusion/algorithm/insert.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/int.hpp>
#include <string>

int
main()
{
    using namespace boost::fusion;
/*    using boost::mpl::vector_c;
    using boost::mpl::advance;
    using boost::mpl::int_;*/
    namespace fusion = boost::fusion;
    namespace mpl = boost::mpl;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing insert

    //Needed to split insert_tests.cpp because of compiler limit in MSVC 6.5

    {
        typedef mpl::vector_c<int, 1, 2, 3, 4, 5> mpl_vec;
        typedef mpl::begin<mpl_vec>::type mpl_vec_begin;
        typedef mpl::advance<mpl_vec_begin, mpl::int_<3> >::type mpl_vec_at3;

        std::cout << fusion::insert(mpl_vec(), mpl_vec_at3(), mpl::int_<66>()) << std::endl;
        BOOST_TEST((fusion::insert(mpl_vec(), mpl_vec_at3(), mpl::int_<66>())
            == make_tuple(1, 2, 3, 66, 4, 5)));
    }

    return boost::report_errors();
}