/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 Giovanni Bajo
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// see http://sf.net/tracker/?func=detail&atid=393386&aid=742038&group_id=28447

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <stdio.h>

using namespace boost::spirit;

template <typename IterT>
void test_assign(IterT b, IterT e)
{
    typedef scanner<IterT> scanner_t;
    std::string dst;
    rule<scanner_t> r = (*alpha_p)[assign(dst)];

    parse(b, e, r);

    std::string::iterator d = dst.begin();

    while (b != e)
    {
        if (*d != *b)
        assert(*d == *b);
        ++b; 
        ++d;
    }
}


int main()
{
    std::string src = "abcdef";
    const char* tmpfilename = "sf_bug_742038.tmp";

    test_assign(src.begin(), src.end());

    position_iterator<std::string::iterator> b(src.begin(), src.end(), "");
    position_iterator<std::string::iterator> e;
    test_assign(b, e);

    std::fstream f(tmpfilename, std::ios::out);
    f << src;
    f.close();

    file_iterator<> b1(tmpfilename);
    file_iterator<> e1(b1.make_end());
    test_assign(b1, e1);

    ::remove(tmpfilename);

    std::cout << "Test completed succesfully" << std::endl;

    return 0;
}
