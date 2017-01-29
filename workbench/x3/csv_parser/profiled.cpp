//Purpose:
//  Alternative to nonius.cpp to benchmark code.
//WHY:
//  valgrind couldn't measure the nonius.cpp code
//  because of some instruction it couldn't emulate.
//OriginalSource:
//  Attachments to post:
/*
http://boost.2283326.n4.nabble.com/Design-structure-X3-parser-more-like-Qi-parser-tp4690205p4690362.html
 */
//==========================
#include "input.hpp"
#include <boost/preprocessor/stringize.hpp>
#include BOOST_PP_STRINGIZE(BENCH_INC)

#define ITERATIONS 10
int main(int argc, char** argv)
{

    for (unsigned int i=0; i<ITERATIONS; ++i)
    {
        bench(bench_parser);
    }

    return EXIT_SUCCESS;
}
