#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

struct nontrivial
{
    ~nontrivial() {}
};

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    tuple<int, char> t1(4, 'a');
    get<1>(t1);

    tuple<nontrivial> t3(nontrivial());
    return 0;
}

