/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/algorithm/find.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>
#include <boost/mpl/vector.hpp>

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
    using boost::mpl::identity;
    using boost::mpl::vector;

/// Testing find

    {
        typedef tuple<int, char, int, double> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36);

        std::cout << *boost::fusion::find(t, identity<char>()) << std::endl;
        BOOST_TEST(*boost::fusion::find(t, identity<char>()) == 'x');

        std::cout << *boost::fusion::find(t, identity<int>()) << std::endl;
        BOOST_TEST(*boost::fusion::find(t, identity<int>()) == 12345);

        std::cout << *boost::fusion::find(t, identity<double>()) << std::endl;
        BOOST_TEST(*boost::fusion::find(t, identity<double>()) == 3.36);
    }

    {
        typedef vector<int, char, X, double> mpl_vec;
        BOOST_TEST((*boost::fusion::find(mpl_vec(), identity<X>()) == 12345));
    }

    return boost::report_errors();
}

