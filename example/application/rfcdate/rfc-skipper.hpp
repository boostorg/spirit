/*=============================================================================
    rfc-skipper.hpp

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

#ifndef INCLUDED_RFC_SKIPPER_HPP
#define INCLUDED_RFC_SKIPPER_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/chset.hpp>

struct rfc_skipper : public boost::spirit::grammar<rfc_skipper>
    {
    rfc_skipper()
        {
        }
    template<typename scannerT>
    struct definition
        {
        definition(const rfc_skipper& /*self*/)
            {
            using namespace boost::spirit;

            first = lexeme_d
                    [
                        junk    = lwsp | comment,

                        lwsp    = +(    !str_p("\r\n")
                                        >> chset_p(" \t")
                                   ),

                        comment =  ch_p('(')
                                   >>  *(   lwsp
                                        |   ctext
                                        |   qpair
                                        |   comment
                                        )
                                   >> ')',

                        ctext   =  anychar_p - chset_p("()\\\r"),

                        qpair   =  ch_p('\\') >> anychar_p
                    ];
            }
        const boost::spirit::rule<scannerT>& start() const
            {
            return first;
            }
        boost::spirit::subrule<0>     junk;
        boost::spirit::subrule<1>     lwsp;
        boost::spirit::subrule<2>     comment;
        boost::spirit::subrule<3>     ctext;
        boost::spirit::subrule<4>     qpair;
        boost::spirit::rule<scannerT> first;
        };
    };
const rfc_skipper rfc_skipper_p;

#endif
