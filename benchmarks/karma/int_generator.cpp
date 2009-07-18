//  Copyright (c) 2001-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  MSVC specific #pragma's
#if defined(BOOST_MSVC)
#pragma inline_depth(255)
#pragma inline_recursion(on)
#endif // defined(BOOST_MSVC)

#include <climits>
#include <cstdlib>

#include <iostream> 
#include "../high_resolution_timer.hpp"

//  This value specifies, how to unroll the integer string generation loop.
//      Set this to some integer in between 0 (no unrolling) and max expected 
//      integer string len (complete unrolling). If not specified, this value
//      defaults to 6.
#define BOOST_KARMA_NUMERICS_LOOP_UNROLL 6

#include <boost/spirit/include/karma.hpp>

using namespace std;
using namespace boost::spirit;

#define MAX_ITERATION 10000000

///////////////////////////////////////////////////////////////////////////////
struct random_fill
{
    int operator()() const
    {
        int scale = std::rand() / 100 + 1;
        return (std::rand() * std::rand()) / scale;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    namespace karma = boost::spirit::karma;
    char buffer[65]; // we don't expect more than 64 bytes to be generated here

    cout << "Converting " << MAX_ITERATION 
         << " randomly generated int values to strings." << flush << endl;

    std::srand(0);
    std::vector<int> v (MAX_ITERATION);
    std::generate(v.begin(), v.end(), random_fill()); // randomly fill the vector

    // test the Karma int_ generation routines 
    {
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            char *ptr = buffer;
            karma::generate(ptr, int_, v[i]);
            *ptr = '\0';
        }

        cout << "int_: " << t.elapsed() << " [s]" << flush << endl;
    }
    
    // test the C libraries ltoa function (the most low level function for
    // string conversion available)
    {
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            ltoa(v[i], buffer, 10);
        }
        
        cout << "ltoa: " << t.elapsed() << " [s]" << flush << endl;
    }
    return 0;
}

