/*=============================================================================
    Copyright (c) 2004 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>

template <typename Sequence>
struct test_sequence
{
    typedef typename boost::mpl::begin<Sequence>::type first;
    typedef typename boost::mpl::next<first>::type second;
    typedef typename boost::mpl::next<second>::type third;
    typedef typename boost::mpl::next<third>::type fourth;
    typedef typename boost::mpl::end<Sequence>::type last;

    BOOST_STATIC_ASSERT((boost::is_same<
        typename boost::mpl::deref<first>::type, int>::value));

    BOOST_STATIC_ASSERT((boost::is_same<
        typename boost::mpl::deref<second>::type, float>::value));

    BOOST_STATIC_ASSERT((boost::is_same<
        typename boost::mpl::deref<third>::type, bool>::value));

    BOOST_STATIC_ASSERT((boost::is_same<
        typename boost::mpl::deref<fourth>::type, char>::value));
};

int
main()
{
    using namespace boost::fusion;

    typedef tuple<int, float, bool, char> tuple_type;
    test_sequence<tuple_type> tuple_test;

    return boost::report_errors();
}
