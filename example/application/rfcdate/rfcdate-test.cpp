/*=============================================================================
    rfcdate-test.cpp

    Copyright (c) 2002-2003 by Peter Simons <simons@computer.org>.

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the copyright holder be held liable for
    any damages arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute
    it freely, subject to the following restrictions:

    1.  The origin of this software must not be misrepresented; you must
        not claim that you wrote the original software. If you use this
        software in a product, an acknowledgment in the product documentation
        would be appreciated but is not required.

    2.  Altered source versions must be plainly marked as such, and must
        not be misrepresented as being the original software.

    3.  This notice may not be removed or altered from any source
        distribution.

=============================================================================*/

#include "rfc-skipper.hpp"
#include "rfcdate-parser.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <cassert>

using namespace std;
using namespace boost::spirit;
using namespace phoenix;

struct test_case
    {
    const char* input;
    bool        success;
    const char* output;
    int         tzoffset;
    };

const test_case tests[] =
    {
    { "12\r\n (te\\((HEU12)st)(\r\n )\tJUN\t82",  true,  "1982-06-12 00:00:00",       +0 },
    { "12 jUN 1982",                              true,  "1982-06-12 00:00:00",       +0 },
    { "1 jAN 1970",                               true,  "1970-01-01 00:00:00",       +0 },
    { "Thu, 4 Sep 1973 14:12:17",                 true,  "1973-09-04 14:12:17",       +0 },
    { "Tho, 4 Sep 1973 14:12",                    false, "",                          +0 },
    { "Thu, 31 Sep 1973 14:12",                   true,  "1973-09-31 14:12:00",       +0 },
    { "Thu, 31 (\r)Sep 1999 14:12",               false, "",                          +0 },
    { "Thu, 1 Aug 2002 12:34:55 -1234",           true,  "2002-08-01 12:34:55",    -1234 },
    { "17 Mar 2017 00:00:13 +1234",               true,  "2017-03-17 00:00:13",    +1234 },
    { "17 Mar 2017 00:00:13 1234",                false, "",                          +0 },
    { "1 Jan 2000 00:00:00 Ut",                   true,  "2000-01-01 00:00:00",       +0 },
    { "1 Jan 2000 00:00:00 GmT",                  true,  "2000-01-01 00:00:00",       +0 },
    { "1 Jan 2000 00:00:00 est",                  true,  "2000-01-01 00:00:00",     -500 },
    { "1 Jan 9999 00:00:00 est",                  true,  "9999-01-01 00:00:00",     -500 },
    { "1 Jan 199 00:00:00 est",                   false, "",                          +0 },
    { "1 Jan 2000 00:00:00 edt",                  true,  "2000-01-01 00:00:00",     -400 },
    { "1 Jan 2000 00:00:00 cst",                  true,  "2000-01-01 00:00:00",     -600 },
    { "1 Jan 2000 00:00:00 pdt",                  true,  "2000-01-01 00:00:00",     -700 },
    { "1 Jan 2000 00:00:00 Z",                    true,  "2000-01-01 00:00:00",    -0000 },
    { "1 Jan 2000 00:00:00 m",                    true,  "2000-01-01 00:00:00",    -1200 },
    { "1 Jan 2000 00:00:00 Y",                    true,  "2000-01-01 00:00:00",    +1200 },
    };

struct runner
    {
    void operator()(const test_case& test)
        {
        timestamp tstamp;
        cout << "Parsing '" << test.input << "' --> ";
        parse_info<> rc =
            parse(test.input, rfcdate_p[assign(tstamp)], rfc_skipper_p);
        assert(rc.full == test.success);
        if (rc.full)
            {
            char buf[1024];
            size_t len = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstamp);
            assert(len > 0);
            cout << buf << " tzoffset " << tstamp.tzoffset << endl;
            assert(strcmp(buf, test.output) == 0);
            assert(tstamp.tzoffset == ((test.tzoffset / 100) * 60 + (test.tzoffset % 100)) * 60);
            }
        else
            cout << " syntax error" << endl;
        }
    };

int main(int , char** )
    {
    // Verify that the parser returns the expected results.

    for_each (tests, tests + sizeof(tests) / sizeof(test_case), runner());


    // Traditionally, I had problems with returning the results of
    // parsing to the user-specified location. This short test
    // verifies, sort of, that this is working as expected.

    const char input[] = "Thu, 31 Sep 1973 14:12";
    timestamp  result1, result2;
    assert(boost::spirit::parse(input, rfcdate_p[var(result1) = arg1], rfc_skipper_p).full);
    assert(boost::spirit::parse(input, rfcdate_p[var(result2) = arg1], rfc_skipper_p).full);
    assert(memcmp(&result1, &result2, sizeof(timestamp)) == 0);


    // Done.

    return 0;
    }
