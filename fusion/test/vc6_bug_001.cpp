/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

struct nontrivial
{
    ~nontrivial() {}
};

int
main()
{
    using namespace boost::fusion;

    tuple<int, char> t1(4, 'a');
    get<1>(t1);

    nontrivial nt;
    tuple<nontrivial> t3(nt);
    return boost::report_errors();
}

