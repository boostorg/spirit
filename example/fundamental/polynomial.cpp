//
//  A very simple parser grammar .
//  This parser parses a simple polynomial expression ( of the form
//  aX^2 + bX^3 + ...  ) .
//
// Written by: Andy Elvey
// Slightly modified by Dan Nuffer
//
//    This software is provided 'as-is', without any express or implied
//    warranty. In no event will the copyright holder be held liable for
//    any damages arising from the use of this software.
//
//    Permission is granted to anyone to use this software for any purpose,
//    including commercial applications, and to alter it and redistribute
//    it freely, subject to the following restrictions:
//
//    1.  The origin of this software must not be misrepresented; you must
//        not claim that you wrote the original software. If you use this
//        software in a product, an acknowledgment in the product documentation
//        would be appreciated but is not required.
//
//    2.  Altered source versions must be plainly marked as such, and must
//        not be misrepresented as being the original software.
//
//    3.  This notice may not be removed or altered from any source
//        distribution.
//
// Uses:  The Spirit parser framework, which was written by
//  Joel de Guzman isis-tech.n3.net
//

//#define BOOST_SPIRIT_DEBUG  ///$$$ DEFINE THIS WHEN DEBUGGING $$$///

#include <boost/spirit/core.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
using namespace boost::spirit;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
//  Start grammar definition
//----------------------------------------------------------------------------

struct polynomial_grammar :
    public grammar<polynomial_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        typedef rule<ScannerT> rule_t;

        rule_t program, stmts, term, constant, sign, var, caret;
        rule_t IDENT, STRING_LITERAL, VAR;

        definition(polynomial_grammar const&)
        {
            //-----------------------------------------------------------------
            // OPERATORS
            //-----------------------------------------------------------------
            chlit<>     PLUS('+');
            chlit<>     MINUS('-');
            chlit<>     CARET('^');

            //-----------------------------------------------------------------
            // TOKENS
            //-----------------------------------------------------------------


            IDENT =
                nocase_d
                [
                    lexeme_d
                    [
                        (alpha_p >> *(alnum_p | '_'))
                    ]
                ]
            ;

            STRING_LITERAL =
                lexeme_d
                [
                    ch_p('\'')
                    >>  +(
                            str_p("\'\'")
                            | anychar_p - ch_p('\'')
                         )
                    >> ch_p('\'')
                ]
            ;

            VAR = nocase_d[ lexeme_d[ alpha_p ] ] ;


            //-----------------------------------------------------------------
            // RULES
            //-----------------------------------------------------------------

            //  Now - the actual BNF grammar for the parser

            program =
                +(stmts)
            ;

            stmts =
                ( term | constant )
            ;

            term =
                +(
                    !(sign)
                    >> !(uint_p)
                    >> VAR
                    >> !(CARET)
                    >> !(uint_p)
                 )
            ;   //  e.g. 3x^2

            constant =
                !(sign) >> +(uint_p)
            ;

            sign =
                ( PLUS | MINUS )
            ;

            var =
                VAR
            ;

            caret =
                CARET
            ;

        }

        rule_t const& start() const
        {
            return program;
        }
    };
};


int
main(int /*argc*/, char* /*argv[]*/)
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\t A polynomial parser...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "Type a polynomial (e.g. 3a^4 + 7a^5 - 8a^3 - 4a - 5) or\n"
        << "[q or Q] to quit\n\n";


    polynomial_grammar g;

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        if (parse(str.c_str(), g, space_p).full)
        {
            cout << "parsing succeeded\n";
        }
        else
        {
            cout << "parsing failed\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}

