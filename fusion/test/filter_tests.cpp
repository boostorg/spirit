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
#include <boost/spirit/fusion/algorithm/filter.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/not.hpp>

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

    using boost::mpl::_;
    using boost::mpl::not_;
    using boost::is_class;
    using boost::mpl::vector;
    using boost::is_same;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing filter

    X x; Y y;
    typedef tuple<Y, char, long, X, bool, double> tuple_type;
    tuple_type t(y, '@', 987654, x, true, 6.6);

    {
        std::cout << filter(t, not_<is_class<_> >()) << std::endl;
        BOOST_TEST((filter(t, not_<is_class<_> >())
            == make_tuple('@', 987654, true, 6.6)));
    }

    {
        std::cout << filter(t, is_class<_>()) << std::endl;
        BOOST_TEST((filter(t, is_class<_>())
            == make_tuple(y, x)));
    }

    {
        typedef vector<Y, char, long, X, bool, double> mpl_vec;
        BOOST_TEST((filter(mpl_vec(), not_<is_class<_> >())
            == make_tuple('\0', 0, false, 0.0)));
        BOOST_TEST((filter(mpl_vec(), is_class<_>())
            == make_tuple(y, x)));
    }

    {
        typedef tuple<int> tuple_type;
        typedef meta::filter<tuple_type, is_class<_> >::type none_remain_type;
        BOOST_MPL_ASSERT((meta::equal_to<meta::begin<none_remain_type>::type, meta::end<none_remain_type>::type>));
        typedef meta::filter<tuple_type, not_<is_class<_> > >::type some_remain_type;
        BOOST_MPL_ASSERT_NOT((meta::equal_to<meta::begin<some_remain_type>::type, meta::end<some_remain_type>::type>));
    }

    return boost::report_errors();
}

