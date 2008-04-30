/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A Roman Numerals Parser (demonstrating the symbol table). This is
//  discussed in the "Symbols" chapter in the Spirit User's Guide.
//
//  [ JDG August 22, 2002 ] spirit1
//  [ JDG March 13, 2007 ]  spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
using namespace boost::spirit::arg_names;
using boost::phoenix::ref;

///////////////////////////////////////////////////////////////////////////////
//  Parse roman hundreds (100..900) numerals using the symbol table.
//  Notice that the data associated with each slot is passed
//  to attached semantic actions.
///////////////////////////////////////////////////////////////////////////////
struct hundreds_ : symbols<char, unsigned>
{
    hundreds_()
    {
        add
            ("C"    , 100)
            ("CC"   , 200)
            ("CCC"  , 300)
            ("CD"   , 400)
            ("D"    , 500)
            ("DC"   , 600)
            ("DCC"  , 700)
            ("DCCC" , 800)
            ("CM"   , 900)
        ;
    }

} hundreds;

///////////////////////////////////////////////////////////////////////////////
//  Parse roman tens (10..90) numerals using the symbol table.
///////////////////////////////////////////////////////////////////////////////
struct tens_ : symbols<char, unsigned>
{
    tens_()
    {
        add
            ("X"    , 10)
            ("XX"   , 20)
            ("XXX"  , 30)
            ("XL"   , 40)
            ("L"    , 50)
            ("LX"   , 60)
            ("LXX"  , 70)
            ("LXXX" , 80)
            ("XC"   , 90)
        ;
    }

} tens;

///////////////////////////////////////////////////////////////////////////////
//  Parse roman ones (1..9) numerals using the symbol table.
///////////////////////////////////////////////////////////////////////////////
struct ones_ : symbols<char, unsigned>
{
    ones_()
    {
        add
            ("I"    , 1)
            ("II"   , 2)
            ("III"  , 3)
            ("IV"   , 4)
            ("V"    , 5)
            ("VI"   , 6)
            ("VII"  , 7)
            ("VIII" , 8)
            ("IX"   , 9)
        ;
    }

} ones;

///////////////////////////////////////////////////////////////////////////////
//  roman (numerals) grammar
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct roman : grammar_def<Iterator, unsigned()>
{
    roman()
    {
        start
            =   +char_('M') [_val += 1000]
            ||  hundreds    [_val += _1]
            ||  tens        [_val += _1]
            ||  ones        [_val += _1];

        //  Note the use of the || operator. The expression
        //  a || b reads match a or b and in sequence. Try
        //  defining the roman numerals grammar in YACC or
        //  PCCTS. Spirit rules! :-)
    }

    rule<Iterator, unsigned()> start;
};

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tRoman Numerals Parser\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type a Roman Numeral ...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;
    typedef roman<iterator_type> roman;

    roman def; //  Our grammar definition
    grammar<roman> roman_parser(def); // Our grammar

    std::string str;
    unsigned result;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = parse(iter, end, roman_parser[ref(result) = _1]);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "result = " << result << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(iter, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \": " << rest << "\"\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


