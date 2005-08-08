/*=============================================================================
    Copyright (c) 2003 Joel de Guzman
    Copyright (c) 2005 Eric Niebler

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/algorithm/any.hpp>
#include <boost/spirit/fusion/algorithm/for_each.hpp>
#include <boost/lambda/lambda.hpp>

int
main()
{
/// Testing any

    {
        boost::fusion::tuple<int,short,double> t(1, 2, 3.3);
        BOOST_TEST((boost::fusion::any(t, boost::lambda::_1 == 2)));
    }

    {
        boost::fusion::tuple<int,short,double> t(1, 2, 3.3);
        BOOST_TEST((!boost::fusion::any(t, boost::lambda::_1 == 3)));
    }

    return boost::report_errors();
}

