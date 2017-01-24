//Purpose:
//  Reproduce the link problem reported in post:
/*
http://boost.2283326.n4.nabble.com/Linking-error-when-changing-gt-to-gt-gt-tc4689820.html
 */
//    
//===========================================
#include <boost/detail/lightweight_test.hpp>
#include <iostream>
#include "derived.hpp"

bool test(const std::string& str)
{
    iterator_type in = str.begin();
    iterator_type end = str.end();
    unused_type attr;
    derived g;

    bool ret = boost::spirit::x3::phrase_parse(in, end, g.x, boost::spirit::x3::ascii::space, attr);
    ret &= (in == end);
    return ret;
}


int main()
{
    derived g;
    bool result=test("xx");
    BOOST_TEST(result);
    return boost::report_errors();
}
