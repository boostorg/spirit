/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// mismatch closure return type bug
// http://article.gmane.org/gmane.comp.parsers.spirit.general/3678
#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <string>

using namespace boost;
using namespace spirit;

typedef std::string member_type;

struct my_closure: closure<my_closure, member_type>
{
    member1 val;
};

int
main()
{
    rule<scanner<char const*>, my_closure::context_t> my_rule = real_p;
    assert(parse("1", my_rule).full);
    return 0;
}
