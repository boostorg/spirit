/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/algorithm/find_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/is_same.hpp>

struct X
{
    operator int() const
    {
        return 12345;
    }
};

int
main()
{
    using namespace boost::fusion;
    using boost::is_same;
    using boost::mpl::_;
    using boost::mpl::vector;

/// Testing find_if

    {
        typedef tuple<int, char, int, double> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36);

        std::cout << *find_if(t, is_same<_, char>()) << std::endl;
//        BOOST_TEST((*find_if(t, is_same<_, char>()) == 'x'));

        std::cout << *find_if(t, is_same<_, int>()) << std::endl;
//        BOOST_TEST((*find_if(t, is_same<_, int>()) == 12345));

        std::cout << *find_if(t, is_same<_, double>()) << std::endl;
//        BOOST_TEST((*find_if(t, is_same<_, double>()) == 3.36));
    }

    {
        typedef vector<int, char, X, double> mpl_vec;
//        BOOST_TEST((*find_if(mpl_vec(), is_same<_, X>()) == 12345));
    }

    return boost::report_errors();
}

