/*=============================================================================
    timestamp.hpp

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

#ifndef INCLUDED_TIMESTAMP_HPP
#define INCLUDED_TIMESTAMP_HPP

#include <ctime>
#include <iostream>
#include <string>

struct timestamp : public tm
    {
    timestamp() { memset(this, 0, sizeof(*this)); }
    int tzoffset;
    };

std::ostream& operator<< (std::ostream& os, const timestamp& ts)
    {
    os << "timestamp =\n"
       << "    {\n"
       << "    tm_sec   = " << ts.tm_sec   << "\n"
       << "    tm_min   = " << ts.tm_min   << "\n"
       << "    tm_hour  = " << ts.tm_hour  << "\n"
       << "    tm_mday  = " << ts.tm_mday  << "\n"
       << "    tm_mon   = " << ts.tm_mon   << "\n"
       << "    tm_year  = " << ts.tm_year  << "\n"
       << "    tm_wday  = " << ts.tm_wday  << "\n"
       << "    tm_yday  = " << ts.tm_yday  << "\n"
       << "    tm_isdst = " << ts.tm_isdst << "\n"
       << "    tzoffset = " << ts.tzoffset << "\n"
       << "    }\n";
    return os;
    }

#endif
