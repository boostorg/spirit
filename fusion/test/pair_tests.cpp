#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/tuple_size.hpp>
#include <boost/spirit/fusion/sequence/tuple_element.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <utility>
#include <iostream>
#include <string>

int main()
{
    using namespace boost::fusion;
    BOOST_MPL_ASSERT_RELATION((tuple_size<std::pair<int,float> >::value),==,2);

    BOOST_MPL_ASSERT((boost::is_same<tuple_element<0, std::pair<int, float> >::type, int>));
    BOOST_MPL_ASSERT((boost::is_same<tuple_element<1, std::pair<int, float> >::type, float>));

    std::pair<int, std::string> pr(1, "hello");
    assert(get<0>(pr) == 1);
    assert(get<1>(pr) == "hello");

    get<0>(pr) = 2;
    get<1>(pr) = "world";
    assert(get<0>(pr) == 2);
    assert(get<1>(pr) == "world");

    const std::pair<int, std::string> pr2(pr);
    assert(get<0>(pr2) == 2);
    assert(get<1>(pr2) == "world");
    
    return 0;
}
