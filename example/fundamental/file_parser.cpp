/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002 Jeff Westfahl
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A parser that echoes a file
//
//  [ JMW 8/05/2002 ]
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
using namespace boost::spirit;

////////////////////////////////////////////////////////////////////////////
//
//  Types
//
////////////////////////////////////////////////////////////////////////////
typedef char                    char_t;
typedef file_iterator<char_t>   iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule<scanner_t>         rule_t;

////////////////////////////////////////////////////////////////////////////
//
//  Actions
//
////////////////////////////////////////////////////////////////////////////
void echo(iterator_t first, iterator_t const& last)
{
    while (first != last)
        std::cout << *first++;
}

////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    if (2 > argc)
    {
        std::cout << "Must specify a filename!\n";
        return -1;
    }

    // Create a file iterator for this file
    iterator_t first(argv[1]);

    if (!first)
    {
        std::cout << "Unable to open file!\n";
        return -1;
    }

    // Create an EOF iterator
    iterator_t last = first.make_end();

    // A simple rule
    rule_t r = *(anychar_p);

    // Parse
    parse_info <iterator_t> info = parse(
        first,
        last,
        r[&echo]
    );

    // This really shouldn't fail...
    if (info.full)
        std::cout << "Parse succeeded!\n";
    else
        std::cout << "Parse failed!\n";

   return 0;
}
