//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  MSVC specific #pragma's
#if defined(BOOST_MSVC)
#pragma inline_depth(255)
#pragma inline_recursion(on)
#endif // defined(BOOST_MSVC)

#include <climits>
#include <cassert>

#include <iostream> 
#include <boost/spirit/include/karma.hpp>

#include "../high_resolution_timer.hpp"

using namespace std;
using namespace boost::spirit;

#define MAX_ITERATION 10000000

///////////////////////////////////////////////////////////////////////////////
struct random_fill
{
    double operator()() const
    {
        double scale = std::rand() / 100 + 1;
        return double(std::rand() * std::rand()) / scale;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    namespace karma = boost::spirit::karma;
    char buffer[512]; // we don't expect more than 512 bytes to be generated 

    cout << "Converting " << MAX_ITERATION 
         << " randomly generated double values to strings." << flush << endl;

    std::srand(0);
    std::vector<double> v (MAX_ITERATION);
    std::generate(v.begin(), v.end(), random_fill()); // randomly fill the vector

    // test the Karma double_ generation routines 
    {
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            char *ptr = buffer;
            karma::generate(ptr, double_, v[i]);
            *ptr = '\0';
        }

        cout << "double_: " << t.elapsed() << " [s]" << flush << endl;
    }
    
    // test the C libraries gcvt function (the most low level function for
    // string conversion available)
    {
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            gcvt(v[i], 10, buffer);
        }
        
        cout << "gcvt:    " << t.elapsed() << " [s]" << flush << endl;
    }
    return 0;
}

