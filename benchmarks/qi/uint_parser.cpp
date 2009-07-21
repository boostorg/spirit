/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if defined(BOOST_MSVC)
#pragma inline_depth(255)
#pragma inline_recursion(on)
#define _SECURE_SCL 0 
#endif // defined(BOOST_MSVC)

#include "../high_resolution_timer.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
#include <climits>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>

#define MAX_ITERATION 1000000

void check(int a, int b)
{
    if (a != b)
    {
        std::cout << "Parse Error, got: " << a << " and " << b << std::endl;
        abort();
    }
}

int main()
{
    namespace qi = boost::spirit::qi;
    using qi::int_;
    
    std::cout << "initializing input strings..." << std::endl;
    std::vector<int> src(MAX_ITERATION);
    std::vector<std::string> src_str(MAX_ITERATION);
    for (int i = 0; i < MAX_ITERATION; ++i)
    {
        src[i] = std::rand() * std::rand();
        if (std::rand() % 2)
            src[i] = -src[i];
        src_str[i] = boost::lexical_cast<std::string>(src[i]);
    }
    
    // test the C libraries atoi function (the most low level function for
    // string conversion available)
    {
        std::vector<int> v(MAX_ITERATION);
        std::vector<char const*> f(MAX_ITERATION);

        // get the C string
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            f[i] = src_str[i].c_str();
        }
        
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            v[i] = atoi(f[i]);
        }

        std::cout << "atoi: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }
    
    // test the C libraries strtol function (the most low level function for
    // string conversion available)
    {
        std::vector<int> v(MAX_ITERATION);
        std::vector<char const*> f(MAX_ITERATION);

        // get the C string
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            f[i] = src_str[i].c_str();
        }

        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            v[i] = strtol(f[i], 0, 10);
        }

        std::cout << "strtol: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }
    
    // test sscanf
    {
        std::vector<int> v(MAX_ITERATION);
        std::vector<char const*> f(MAX_ITERATION);

        // get the C string
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            f[i] = src_str[i].c_str();
        }

        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            sscanf(f[i], "%d", &v[i]);
        }

        std::cout << "sscanf: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }

    // test iostream
    {
        std::istringstream ss;
        std::vector<int> v(MAX_ITERATION);
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            ss.clear();
            ss.str(src_str[i]);
            ss >> v[i];
        }

        std::cout << "std::iostream: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }
    
    // test boost::lexical_cast
    {
        std::vector<int> v(MAX_ITERATION);
        std::vector<char const*> f(MAX_ITERATION);

        // get the C string
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            f[i] = src_str[i].c_str();
        }

        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            v[i] = boost::lexical_cast<int>(src[i]);
        }

        std::cout << "boost::lexical_cast: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }
    
    // test the Qi int_ parser routines 
    {
        std::vector<int> v(MAX_ITERATION);
        std::vector<char const*> f(MAX_ITERATION);
        std::vector<char const*> l(MAX_ITERATION);

        // get the first/last iterators
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            f[i] = src_str[i].c_str();
            l[i] = f[i];
            while (*l[i])
                l[i]++;
        }
    
        util::high_resolution_timer t;

        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            qi::parse(f[i], l[i], int_, v[i]);
        }

        std::cout << "spirit int_: " << t.elapsed() << " [s]" << std::flush << std::endl;
        
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            check(v[i], src[i]);
        }
    }

    return 0;
}

