#include <vector>
#include <algorithm>
#include <iostream>
#include "boost/spirit/phoenix/operators.hpp"
#include "boost/spirit/phoenix/primitives.hpp"

using namespace std;
using namespace phoenix;

int
main()
{
    int init[] = { 2, 10, 4, 5, 1, 6, 8, 3, 9, 7 };
    vector<int> c(init, init + 10);
    typedef vector<int>::iterator iterator;

    //  Find the first odd number in container c
    iterator it = find_if(c.begin(), c.end(), arg1 % 2 == 1);

    if (it != c.end())
        cout << *it;    //  if found, print the result
    return 0;
}
