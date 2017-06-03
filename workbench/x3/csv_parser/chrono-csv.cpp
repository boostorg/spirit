//Purpose:
//  Alternative to nonius.cpp to benchmark code.
//WHY:
//  gcc compiler trips over, IIRC, the concept class.
//  I guess gcc thinks it's a keyword.
//OriginalSource:
//  Attachments to post:
/*
http://boost.2283326.n4.nabble.com/Design-structure-X3-parser-more-like-Qi-parser-tp4690205p4690362.html
 */
//==========================
#include "input.hpp"
#include <boost/preprocessor/stringize.hpp>
#include BOOST_PP_STRINGIZE(BENCH_INC)
#include <iostream>
#include <chrono>

#define ITERATIONS 1000
int main(int argc, char** argv)
{
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    for (unsigned int i=0; i<ITERATIONS; ++i)
    {
        bench(bench_parser);
    }
    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    auto total_time=std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "mean: " << (double(total_time)/ITERATIONS) << "ms" << std::endl;
    return EXIT_SUCCESS;
}
