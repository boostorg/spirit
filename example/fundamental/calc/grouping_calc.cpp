/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
////////////////////////////////////////////////////////////////////////////
//
//  Full calculator example
//  [ demonstrating grouping parser ]
//
//  [ Hartmut Kaiser 10/7/2002 ]
//
////////////////////////////////////////////////////////////////////////////

//#define BOOST_SPIRIT_DEBUG        // define this, if debug output is required

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;
using namespace phoenix;

////////////////////////////////////////////////////////////////////////////
//
//  Our calculator grammar using phoenix and grouping to do the semantics
//
//  Note:   The top rule propagates the expression result (value) upwards
//          to the calculator grammar self.val closure member which is
//          then visible outside the grammar (i.e. since self.val is the
//          member1 of the closure, it becomes the attribute passed by
//          the calculator to an attached semantic action. See the
//          driver code that uses the calculator below).
//
////////////////////////////////////////////////////////////////////////////
struct calc_closure : boost::spirit::closure<calc_closure, double>
{
    member1 val;
};

struct calculator : public grammar<calculator, calc_closure::context_t>
{
    template <typename ScannerT>
    struct definition
    {
        definition(calculator const& self)
        {
            top = addsub_expr[self.val = arg1];

            addsub_expr
                =   group_d[muldiv_expr >> '+' >> muldiv_expr]
                    [
                        addsub_expr.val = arg1 + arg3
                    ]
                |   group_d[muldiv_expr >> '-' >> muldiv_expr]
                    [
                        addsub_expr.val = arg1 - arg3
                    ]
                ;

            muldiv_expr
                =   group_d[factor >> '*' >> factor]
                    [
                        muldiv_expr.val = arg1 * arg3
                    ]
                |   group_d[factor >> '/' >> factor]
                    [
                        muldiv_expr.val = arg1 / arg3
                    ]
                |   factor
                    [
                        muldiv_expr.val = arg1
                    ]
                ;

            factor
                =   ureal_p[factor.val = arg1]
                |   '(' >> addsub_expr[factor.val = arg1] >> ')'
                |   '+' >> addsub_expr[factor.val = arg1]
                |   '-' >> addsub_expr[factor.val = -arg1]
                ;

            BOOST_SPIRIT_DEBUG_NODE(top);
            BOOST_SPIRIT_DEBUG_NODE(addsub_expr);
            BOOST_SPIRIT_DEBUG_NODE(muldiv_expr);
            BOOST_SPIRIT_DEBUG_NODE(factor);
        }

        typedef rule<ScannerT, calc_closure::context_t> rule_t;
        rule_t addsub_expr, muldiv_expr, factor;
        rule<ScannerT> top;

        rule<ScannerT> const&
        start() const { return top; }
    };
};

////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
////////////////////////////////////////////////////////////////////////////
int
main()
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\tExpression parser using Phoenix and grouping...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "Type an expression...or [q or Q] to quit\n\n";

    calculator calc;    //  Our parser

    string str;
    while (getline(cin, str))
    {
        if (str[0] == 'q' || str[0] == 'Q')
            break;

        double n = 0;
        parse_info<> info = parse(str.c_str(), calc[var(n) = arg1], space_p);

        //  calc[var(n) = arg1] invokes the calculator and extracts
        //  the result of the computation. See calculator grammar
        //  note above.

        if (info.full)
        {
            cout << "-------------------------\n";
            cout << "Parsing succeeded\n";
            cout << "result = " << n << endl;
            cout << "-------------------------\n";
        }
        else
        {
            cout << "-------------------------\n";
            cout << "Parsing failed\n";
            cout << "stopped at: \": " << info.stop << "\"\n";
            cout << "-------------------------\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}


