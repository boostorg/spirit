/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/algorithm/remove.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/identity.hpp>

struct X
{
    operator char const*() const
    {
        return "<X-object>";
    }
};

struct Y
{
    operator char const*() const
    {
        return "<Y-object>";
    }
};

int
main()
{
    using namespace boost::fusion;
    using boost::mpl::identity;
    using boost::mpl::vector;
    namespace fusion = boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing remove

    X x; Y y;
    typedef tuple<Y, char, long, X, bool, double> tuple_type;
    tuple_type t(y, '@', 987654, x, true, 6.6);

    {
        std::cout << fusion::remove(t, identity<X>()) << std::endl;
        BOOST_TEST((fusion::remove(t, identity<X>())
            == make_tuple(y, '@', 987654, true, 6.6)));
    }

    {
        std::cout << fusion::remove(t, identity<Y>()) << std::endl;
        BOOST_TEST((fusion::remove(t, identity<Y>())
            == make_tuple('@', 987654, x, true, 6.6)));
    }

    {
        std::cout << fusion::remove(t, identity<long>()) << std::endl;
        BOOST_TEST((fusion::remove(t, identity<long>())
            == make_tuple(y, '@', x, true, 6.6)));
    }

    {
        typedef vector<Y, char, long, X, bool, double> mpl_vec;
        BOOST_TEST((fusion::remove(mpl_vec(), identity<X>())
            == tuple<Y, char, long, bool, double>()));
        BOOST_TEST((fusion::remove(mpl_vec(), identity<Y>())
            == tuple<char, long, X, bool, double>()));
        BOOST_TEST((fusion::remove(mpl_vec(), identity<long>())
            == tuple<Y, char, X, bool, double>()));
    }

    return boost::report_errors();
}

