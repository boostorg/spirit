//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>
#include "test.hpp"

int main()
{
    using namespace spirit_test;
    using namespace boost::spirit;

    {
        using boost::spirit::karma::double_;
        using boost::spirit::karma::omit;

        std::pair<double, double> p (1.0, 2.0);
        BOOST_TEST(test("2.0", omit[double_] << double_, p));
    }

    return boost::report_errors();
}
