#include <vector>
#include <algorithm>
#include <iostream>
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/primitives.hpp"

using namespace std;
using namespace phoenix;

//////////////////////////////////
template <int N>
struct static_int {

    template <typename TupleT>
    struct result { typedef int type; };

    template <typename TupleT>
    int eval(TupleT const&) const { return N; }
};

//////////////////////////////////
template <int N>
phoenix::actor<static_int<N> >
int_const()
{
    return static_int<N>();
}

//////////////////////////////////
int
main()
{
    cout << (int_const<5>() + int_const<6>())() << endl;
    return 0;
}
