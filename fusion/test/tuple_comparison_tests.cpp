/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Järvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/not_equal_to.hpp>
#include <boost/spirit/fusion/sequence/less.hpp>
#include <boost/spirit/fusion/sequence/less_equal.hpp>
#include <boost/spirit/fusion/sequence/greater.hpp>
#include <boost/spirit/fusion/sequence/greater_equal.hpp>

void
equality_test()
{
    using namespace boost::fusion;

    tuple<int, char> t1(5, 'a');
    tuple<int, char> t2(5, 'a');
    BOOST_TEST(t1 == t2);

    tuple<int, char> t3(5, 'b');
    tuple<int, char> t4(2, 'a');
    BOOST_TEST(t1 != t3);
    BOOST_TEST(t1 != t4);
    BOOST_TEST(!(t1 != t2));
}

void
ordering_test()
{
    using namespace boost::fusion;

    tuple<int, float> t1(4, 3.3f);
    tuple<short, float> t2(5, 3.3f);
    tuple<long, double> t3(5, 4.4);
    BOOST_TEST(t1 < t2);
    BOOST_TEST(t1 <= t2);
    BOOST_TEST(t2 > t1);
    BOOST_TEST(t2 >= t1);
    BOOST_TEST(t2 < t3);
    BOOST_TEST(t2 <= t3);
    BOOST_TEST(t3 > t2);
    BOOST_TEST(t3 >= t2);
}

void extended_tests()
{
    using namespace boost::fusion;
    typedef tuple<char,int> test_tuple;
    const test_tuple a0('a',0), a1('a',1), a2('a',2),
        b0('b',0), b1('b',1), b2('b',2),
        c0('c',0), c1('c',1), c2('c',2);

    BOOST_TEST(b1 == b1);
    BOOST_TEST(!(b1 == a0));
    BOOST_TEST(!(b1 == a1));
    BOOST_TEST(!(b1 == a2));
    BOOST_TEST(!(b1 == b0));
    BOOST_TEST(!(b1 == b2));
    BOOST_TEST(!(b1 == c0));
    BOOST_TEST(!(b1 == c1));
    BOOST_TEST(!(b1 == c2));
    BOOST_TEST(!(a0 == b1));
    BOOST_TEST(!(a1 == b1));
    BOOST_TEST(!(a2 == b1));
    BOOST_TEST(!(b0 == b1));
    BOOST_TEST(!(b2 == b1));
    BOOST_TEST(!(c0 == b1));
    BOOST_TEST(!(c1 == b1));
    BOOST_TEST(!(c2 == b1));

    BOOST_TEST(!(b1 != b1));
    BOOST_TEST(b1 != a0);
    BOOST_TEST(b1 != a1);
    BOOST_TEST(b1 != a2);
    BOOST_TEST(b1 != b0);
    BOOST_TEST(b1 != b2);
    BOOST_TEST(b1 != c0);
    BOOST_TEST(b1 != c1);
    BOOST_TEST(b1 != c2);
    BOOST_TEST(a0 != b1);
    BOOST_TEST(a1 != b1);
    BOOST_TEST(a2 != b1);
    BOOST_TEST(b0 != b1);
    BOOST_TEST(b2 != b1);
    BOOST_TEST(c0 != b1);
    BOOST_TEST(c1 != b1);
    BOOST_TEST(c2 != b1);
    
    BOOST_TEST(!(b1 < b1));
    BOOST_TEST(!(b1 < a0));
    BOOST_TEST(!(b1 < a1));
    BOOST_TEST(!(b1 < a2));
    BOOST_TEST(!(b1 < b0));
    BOOST_TEST(b1 < b2);
    BOOST_TEST(b1 < c0);
    BOOST_TEST(b1 < c1);
    BOOST_TEST(b1 < c2);
    BOOST_TEST(a0 < b1);
    BOOST_TEST(a1 < b1);
    BOOST_TEST(a2 < b1);
    BOOST_TEST(b0 < b1);
    BOOST_TEST(!(b2 < b1));
    BOOST_TEST(!(c0 < b1));
    BOOST_TEST(!(c1 < b1));
    BOOST_TEST(!(c2 < b1));

    BOOST_TEST(b1 <=  b1);
    BOOST_TEST(!(b1 <= a0));
    BOOST_TEST(!(b1 <= a1));
    BOOST_TEST(!(b1 <= a2));
    BOOST_TEST(!(b1 <= b0));
    BOOST_TEST(b1 <= b2);
    BOOST_TEST(b1 <= c0);
    BOOST_TEST(b1 <= c1);
    BOOST_TEST(b1 <= c2);
    BOOST_TEST(a0 <= b1);
    BOOST_TEST(a1 <= b1);
    BOOST_TEST(a2 <= b1);
    BOOST_TEST(b0 <= b1);
    BOOST_TEST(!(b2 <= b1));
    BOOST_TEST(!(c0 <= b1));
    BOOST_TEST(!(c1 <= b1));
    BOOST_TEST(!(c2 <= b1));

    BOOST_TEST(!(b1 > b1));
    BOOST_TEST(b1 > a0);
    BOOST_TEST(b1 > a1);
    BOOST_TEST(b1 > a2);
    BOOST_TEST(b1 > b0);
    BOOST_TEST(!(b1 > b2));
    BOOST_TEST(!(b1 > c0));
    BOOST_TEST(!(b1 > c1));
    BOOST_TEST(!(b1 > c2));
    BOOST_TEST(!(a0 > b1));
    BOOST_TEST(!(a1 > b1));
    BOOST_TEST(!(a2 > b1));
    BOOST_TEST(!(b0 > b1));
    BOOST_TEST(b2 > b1);
    BOOST_TEST(c0 > b1);
    BOOST_TEST(c1 > b1);
    BOOST_TEST(c2 > b1);

    BOOST_TEST(b1 >= b1);
    BOOST_TEST(b1 >= a0);
    BOOST_TEST(b1 >= a1);
    BOOST_TEST(b1 >= a2);
    BOOST_TEST(b1 >= b0);
    BOOST_TEST(!(b1 >= b2));
    BOOST_TEST(!(b1 >= c0));
    BOOST_TEST(!(b1 >= c1));
    BOOST_TEST(!(b1 >= c2));
    BOOST_TEST(!(a0 >= b1));
    BOOST_TEST(!(a1 >= b1));
    BOOST_TEST(!(a2 >= b1));
    BOOST_TEST(!(b0 >= b1));
    BOOST_TEST(b2 >= b1);
    BOOST_TEST(c0 >= b1);
    BOOST_TEST(c1 >= b1);
    BOOST_TEST(c2 >= b1);

    return;
}

int
main()
{
    equality_test();
    ordering_test();
    extended_tests();
    return boost::report_errors();
}
