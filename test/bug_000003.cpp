/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// impl::detach_clear bug
// http://sourceforge.net/mailarchive/forum.php?thread_id=2008510&forum_id=25901
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/spirit/utility/chset.hpp>

using namespace boost;
using namespace unit_test_framework;
using namespace spirit;

//-----------------------------------------------------------------------------

void bug_000003_compile_test()
{
    chset<> set;
    set = 'a';
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int argc, char* argv[])
{
    test_suite* test = BOOST_TEST_SUITE("bug_000003 test");

    test->add(BOOST_TEST_CASE(&bug_000003_compile_test));

    return test;
}

//-----------------------------------------------------------------------------
