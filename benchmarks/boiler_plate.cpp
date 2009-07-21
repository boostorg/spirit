/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include "measure.hpp"
#include <iostream>

namespace
{
    struct f
    {
        typedef int type;

        void operator()(int x)
        {
            this->val ^= x; // here is where you put code that you want
                            // to benchmark. Make sure it returns something.
                            // Anything.
        }

        int val; // this is where the value is accumulated
    };
}

int main()
{
    int result;
    double base_time;
    int ret = test::benchmark<f>::test(result, base_time);

    std::cout
        << "f accumulated result:           "
        << result
        << std::endl
        << "f time:                         "
        << base_time
        << std::endl;

    return ret;
}
