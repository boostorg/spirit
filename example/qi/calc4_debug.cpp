/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  Same as cal4, but with debugging enabled.
//
//  [ JDG June 29, 2002 ]   spirit1
//  [ JDG March 5, 2007 ]   spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <iostream>
#include <string>

namespace client
{
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////////
    //  Our calculator grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct calculator : qi::grammar<Iterator, int(), ascii::space_type>
    {
        calculator() : calculator::base_type(expression)
        {
            using namespace qi::labels;
            using qi::uint_;
            using qi::on_error;
            using qi::fail;
            using qi::debug;

            using phoenix::construct;
            using phoenix::val;

            expression =
                term                            [_val = _1]
                >> *(   ('+' > term             [_val += _1])
                    |   ('-' > term             [_val -= _1])
                    )
                ;

            term =
                factor                          [_val = _1]
                >> *(   ('*' > factor           [_val *= _1])
                    |   ('/' > factor           [_val /= _1])
                    )
                ;

            factor =
                uint_                           [_val = _1]
                |   '(' > expression            [_val = _1] > ')'
                |   ('-' > factor               [_val = -_1])
                |   ('+' > factor               [_val = _1])
                ;

            expression.name("expression");
            term.name("term");
            factor.name("factor");

            on_error<fail>
            (
                expression
              , std::cout
                    << val("Error! Expecting ")
                    << _4                               // what failed?
                    << val(" here: \"")
                    << construct<std::string>(_3, _2)   // iterators to error-pos, end
                    << val("\"")
                    << std::endl
            );

            debug(expression);
            debug(term);
            debug(factor);
        }

        qi::rule<Iterator, int(), ascii::space_type> expression, term, factor;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Expression parser...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type an expression...or [q or Q] to quit\n\n";

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef client::calculator<iterator_type> calculator;

    calculator calc; // Our grammar

    std::string str;
    int result;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, calc, space, result);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "result = " << result << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


