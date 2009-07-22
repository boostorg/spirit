/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

// determine the type of the live_code. we can use this to test the
// result of the code we are benchmarking
#define BOOST_SPIRIT_TEST_LIVE_CODE_TYPE long long
#include "measure.hpp"

namespace
{
    struct f
    {
        typedef int type;

        void benchmark(int x)
        {
            this->val += x; // Here is where you put code that you want
                            // to benchmark. Make sure it returns something.
                            // Anything.
        }
        
        static int initial()
        {
            return 1; // our initial value
        }

        int val; // this is where the value is accumulated
    };
}

int main()
{
    BOOST_SPIRIT_TEST_BENCHMARK(
        1000000,    // This is the maximum repetitions to execute
        (f)         // Place your tests here. For now, we have only one test: (f)
                    // If you have 3 tests a, b and c, this line will contain (a)(b)(c)
    )
    
    // This is ultimately responsible for preventing all the test code
    // from being optimized away.  Change this to return 0 and you
    // unplug the whole test's life support system.
    return test::live_code != 0;
}

