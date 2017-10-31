//Purpose:
//  Test the BOOST_SPIRIT_DER_* macros in:
//    x3/nonterminal/rule.hpp
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

    bool ret = boost::spirit::x3::phrase_parse(in, end, g.y, boost::spirit::x3::ascii::space, attr);
    ret &= (in == end);
    return ret;
}


int main()
{
    derived g;
    bool result=test("xxxx");
    BOOST_TEST(result);
    return boost::report_errors();
}
