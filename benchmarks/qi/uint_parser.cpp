/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include "../measure.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/spirit/include/qi.hpp>

namespace
{
    // Random number string generator
    std::string
    gen_int(int digits)
    {
        std::string result;
        if (rand()%2)                       // Prepend a '-'
            result += '-';
        result += '1' + (rand()%9);         // The first digit cannot be '0'
        
        for (int i = 1; i < digits; ++i)    // Generate the remaining digits
            result += '0' + (rand()%10);
        return result;
    }
    
    std::string numbers[9];
    char const* first[9];
    char const* last[9];

    ///////////////////////////////////////////////////////////////////////////
    struct atoi_test
    {
        void benchmark(int x)
        {
            this->val += atoi(first[0]);
            this->val += atoi(first[1]);
            this->val += atoi(first[2]);
            this->val += atoi(first[3]);
            this->val += atoi(first[4]);
            this->val += atoi(first[5]);
            this->val += atoi(first[6]);
            this->val += atoi(first[7]);
            this->val += atoi(first[8]);
        }

        int val;    // This is needed to avoid dead-code elimination
    };
    
    ///////////////////////////////////////////////////////////////////////////
    struct strtol_test
    {
        void benchmark(int x)
        {
            this->val += strtol(first[0], const_cast<char**>(&last[0]), 10);
            this->val += strtol(first[1], const_cast<char**>(&last[1]), 10);
            this->val += strtol(first[2], const_cast<char**>(&last[2]), 10);
            this->val += strtol(first[3], const_cast<char**>(&last[3]), 10);
            this->val += strtol(first[4], const_cast<char**>(&last[4]), 10);
            this->val += strtol(first[5], const_cast<char**>(&last[5]), 10);
            this->val += strtol(first[6], const_cast<char**>(&last[6]), 10);
            this->val += strtol(first[7], const_cast<char**>(&last[7]), 10);
            this->val += strtol(first[8], const_cast<char**>(&last[8]), 10);
        }

        int val;    // This is needed to avoid dead-code elimination
    };
    
    ///////////////////////////////////////////////////////////////////////////
    struct spirit_int_test
    {
        void benchmark(int x)
        {
            namespace qi = boost::spirit::qi;
            using qi::int_;
            int n;
            
            qi::parse(first[0], last[0], int_, n); this->val += n;
            qi::parse(first[1], last[1], int_, n); this->val += n;
            qi::parse(first[2], last[2], int_, n); this->val += n;
            qi::parse(first[3], last[3], int_, n); this->val += n;
            qi::parse(first[4], last[4], int_, n); this->val += n;
            qi::parse(first[5], last[5], int_, n); this->val += n;
            qi::parse(first[6], last[6], int_, n); this->val += n;
            qi::parse(first[7], last[7], int_, n); this->val += n;
            qi::parse(first[8], last[8], int_, n); this->val += n;
        }

        int val;    // This is needed to avoid dead-code elimination
    };
}

int main()
{
    srand(time(0));
    
    // Generate random integers with 1 .. 9 digits
    // We test only 9 digits to avoid overflow
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "Numbers to test:" << std::endl;
    for (int i = 0; i < 9; ++i)
    {
        numbers[i] = gen_int(i+1);
        first[i] = numbers[i].c_str();
        last[i] = first[i];
        while (*last[i])
            last[i]++;
        std::cout << i+1 << " digit number:" << numbers[i] << std::endl;
    }
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;

    BOOST_SPIRIT_TEST_BENCHMARK(
        10000000,     // This is the maximum repetitions to execute
        (atoi_test)
        (strtol_test)
        (spirit_int_test)
    )
    
    // This is ultimately responsible for preventing all the test code
    // from being optimized away.  Change this to return 0 and you
    // unplug the whole test's life support system.
    return test::live_code != 0;
}

