/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// see https://sourceforge.net/tracker/index.php?func=detail&aid=718903&group_id=28447&atid=393386

#ifdef BOOST_MSVC
#pragma warning(disable:4786)
#endif

#include <boost/cstdlib.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/chset.hpp>

using namespace std;
using namespace boost;
using namespace spirit;

int main(int, char *[])
{
        empty_match_parser<chset<char> >
                e(epsilon_p(chset_p("abc")));

    return exit_success;
}
