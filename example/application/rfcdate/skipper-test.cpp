/*=============================================================================
    skipper-test.cpp

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
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
using namespace boost::spirit;

int main(int , char** )
    {
    // Strip brain-dead RFC style input down to the bones using
    // rfc_skipper.

    char weird[] =                                      \
        "From: (Some \r\n"                              \
        "       comment) simons (stuff) \r\n"           \
        "        @      computer (inserted) . (between) org(tokens)";
    string output;
    parse(weird, (    str_p("From:")
                      >> *( anychar_p [append(output)] )
                 ),
          rfc_skipper_p);
    cout << "Stripped address is: '" << output << "'" << endl;
    assert(output == "simons@computer.org");

    return 0;
    }
