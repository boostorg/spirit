/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// range_run bug
// see http://sourceforge.net/tracker/index.php?func=detail&aid=719322&group_id=28447&atid=393386

#include <boost/test/included/unit_test_framework.hpp>
#include <boost/spirit/utility/impl/chset/basic_chset.hpp>

using namespace boost;
using namespace unit_test_framework;
using namespace spirit;

//-----------------------------------------------------------------------------

void sf_bug_719322_test()
{
    basic_chset<int> s;
    s.set(3, 3);
    s.set(1, 5);
    BOOST_CHECK(s.test(5));
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int argc, char* argv[])
{
    test_suite* test = BOOST_TEST_SUITE("chset test");

    test->add(BOOST_TEST_CASE(&sf_bug_719322_test));

    return test;
}

//-----------------------------------------------------------------------------
