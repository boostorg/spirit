/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2003 Giovanni Bajo
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <cassert>
#include <cstdio>
#include <iostream>
#include <boost/concept_check.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>

using namespace std;
using namespace boost::spirit;


///////////////////////////////////////////////////////////////////////////////
namespace {

static const char* TMP_FILE = "file_iter.tmp";

bool CreateTempFile(void)
{
    FILE* f = fopen(TMP_FILE, "wb");

    if (!f)
        return false;

    for (int i=0;i<256;i++)
    {
        unsigned char ci = (unsigned char)i;

        if (fwrite(&ci,1,1,f) == 0)
        {
            fclose(f);
            return false;
        }
    }

    fclose(f);
    return true;
}

template <typename ITER>
void RunTest(void)
{
    // Check constructor opening a file
    ITER a(TMP_FILE);
    assert(a);

    // Assert dereference (twice: derefence
    //  must not move the iterator)
    assert(*a == 0);
    assert(*a == 0);

    // Check random access
    assert(a[123] == 123);

    // Check copy constructor and operator==
    ITER c(a);
    assert(c == a);
    assert(!(c != a));

    // Check make_end()
    ITER b(a.make_end());
    assert(b);
    assert(a != b);
    assert(a+256 == b);
    assert(a == b-256);

    // Check copy constructor on non-trivial position
    assert(*ITER(a+67) == 67);

    // Check increment
    ++a; ++a; a++; a++;
    assert(*a == 4);
    assert(a == c+4);

    // Check decrement
    --a; --a; a--; a--;
    assert(*a == 0);
    assert(a == c);

    // Check end iterator increment/decrement
    --b; b--;
    assert(*b == 254);
    assert(a+254 == b);
    ++b; b++;
    assert(a+256 == b);

    // Check order
    a += 128;
    assert(c < a);
    assert(a < b);
    assert(a > c);
    assert(b > a);

    // Check assignment
    a = b;
    assert(a == b);
    a = c;
    assert(a == c);

    // Check weak order
    assert(a <= c);
    assert(a >= c);
    assert(a <= b);
    assert(!(a >= b));

    // Check increment through end
    a += 255;
    assert(a != b);
    ++a;
    assert(a == b);
    ++a;
    assert(a != b);
}

///////////////////////////////////////////////////////////////////////////////
}

typedef unsigned char character_t;
typedef file_iterator<character_t> iter;
BOOST_CLASS_REQUIRE(iter, boost, RandomAccessIteratorConcept);


int main(void)
{
    if (!CreateTempFile())
    {
        cerr << "ERROR: Cannot create temporary file file_iter.tmp" << endl;
        return 2;
    }

    RunTest<iter>();

    // Check if the file handles were closed correctly
    assert(remove(TMP_FILE) == 0);

    cerr << "Test completed successfully" << endl;
    return 0;
}
