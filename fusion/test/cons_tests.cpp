/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <string>
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/cons.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/fusion/algorithm/for_each.hpp>
#include <boost/spirit/fusion/algorithm/filter.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/lambda.hpp>

int
main()
{
    std::cout << boost::fusion::tuple_open('[');
    std::cout << boost::fusion::tuple_close(']');
    std::cout << boost::fusion::tuple_delimiter(", ");

/// Testing cons

    {
        std::string hello("hello");
        boost::fusion::cons<int,boost::fusion::cons<std::string> > ns =
            boost::fusion::make_cons(1, boost::fusion::make_cons(hello));

        BOOST_TEST((*boost::fusion::begin(ns) == 1));
        BOOST_TEST((*boost::fusion::next(boost::fusion::begin(ns)) == hello));

        *boost::fusion::begin(ns) += 1;
        *boost::fusion::next(boost::fusion::begin(ns)) += ' ';

        BOOST_TEST((*boost::fusion::begin(ns) == 2));
        BOOST_TEST((*boost::fusion::next(boost::fusion::begin(ns)) == hello + ' '));

        boost::fusion::for_each(ns, boost::lambda::_1 += ' ');

        BOOST_TEST((*boost::fusion::begin(ns) == 2 + ' '));
        BOOST_TEST((*boost::fusion::next(boost::fusion::begin(ns)) == hello + ' ' + ' '));
    }

    {
        boost::fusion::tuple<int, float> t(1, 1.1f);
        boost::fusion::cons<int, boost::fusion::cons<float> > nf =
            boost::fusion::make_cons(1, boost::fusion::make_cons(1.1f));

        BOOST_TEST((t == nf));
        BOOST_TEST((boost::fusion::tuple<int>(1) == boost::fusion::filter(nf, boost::is_same<boost::mpl::_,int>())));

        std::cout << nf << std::endl;
        std::cout << boost::fusion::filter(nf, boost::is_same<boost::mpl::_,int>()) << std::endl;
    }

    return boost::report_errors();
}

