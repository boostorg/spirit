//Purpose:
//  Test solution to the link problem reported in post:
/*
http://boost.2283326.n4.nabble.com/Linking-error-when-changing-gt-to-gt-gt-tc4689820.html
 */
//    
//===========================================
#include <boost/detail/lightweight_test.hpp>
#include <iostream>
#include "types.hpp"
#include "example.hpp"

template<typename Parser, typename Attribute>
bool test(const std::string& str, Parser&& p, Attribute&& attr)
{
    using iterator_type = std::string::const_iterator;
    iterator_type in = str.begin();
    iterator_type end = str.end();

    bool ret = boost::spirit::x3::phrase_parse(in, end, p, boost::spirit::x3::ascii::space, attr);
    ret &= (in == end);
    return ret;
}

int main()
{
    client::ast::VariableDec attr;
    bool result=test("var foo : foo<bar, baz<bahama>> ;", client::var_dec() , attr);
    BOOST_TEST(result);
    return boost::report_errors();
}
