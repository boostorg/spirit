/*=============================================================================
  Spirit V1.6.1
  Copyright (c) 2003 Martin Wille
  http://spirit.sourceforge.net/

  Permission to copy, use, modify, sell and distribute this software
  is granted provided this copyright notice appears in all copies.
  This software is provided "as is" without express or implied
  warranty, and with no claim as to its suitability for any purpose.
 =============================================================================*/
#ifndef BOOST_SPIRIT_TEST_IMPL_UTIL_IPP
#define BOOST_SPIRIT_TEST_IMPL_UTIL_IPP

#include <cstdlib>
#include <cstring>

namespace test
{

bool    verbose_runtests; // set to true when started by
                          // libs/spirit/tests/runtests.sh

void init(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    verbose_runtests = !!::getenv("VERBOSE_RUNTESTS");
}

void banner(char const *text)
{
    if (!verbose_runtests)
        return;

    using namespace std;

    int len = ::strlen(text);
    std::cout << "/////////////////////////////////////////////////////////\n";
    len = (57-len)/2;
    while(len-->0) std::cout << ' ';
    std::cout << text << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
}

}
#endif // BOOST_SPIRIT_TEST_IMPL_UTIL_IPP
