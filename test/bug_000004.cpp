/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// chset<>::operator~(range<>) bug
// operator&(chset<>, range<>) bug
// operator&(range<>, chset<>) bug
#include <boost/limits.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/spirit/utility/chset.hpp>

using namespace std;
using namespace boost;
using namespace unit_test_framework;
using namespace spirit;

//-----------------------------------------------------------------------------

void bug_000004_test()
{
    const char min = numeric_limits<char>::min();
    const char max = numeric_limits<char>::max();

    {
        chset<> set(~range<>(min, max));
        BOOST_CHECK(set.test(min) == false);
        BOOST_CHECK(set.test(min) == false);
    }

    {
        chset<> set(chset<>(anychar_p) & range<>(min, max));
        BOOST_CHECK(set.test(min) == true);
        BOOST_CHECK(set.test(min) == true);
    }

    {
        chset<> set(range<>(min, max) & chset<>(anychar_p));
        BOOST_CHECK(set.test(min) == true);
        BOOST_CHECK(set.test(min) == true);
    }
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int argc, char* argv[])
{
    test_suite* test = BOOST_TEST_SUITE("bug_000004 test");

    test->add(BOOST_TEST_CASE(&bug_000004_test));

    return test;
}

//-----------------------------------------------------------------------------
