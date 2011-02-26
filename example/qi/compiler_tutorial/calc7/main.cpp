/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  Now we'll introduce variables and assignment. This time, we'll also
//  be renaming some of the rules -- a strategy for a grander scheme
//  to come ;-)
//
//  This version also shows off grammar modularization. Here you will
//  see how expressions and statements are built as modular grammars.
//
//  [ JDG April 9, 2007 ]       spirit2
//  [ JDG February 18, 2011 : Pure attributes. No semantic actions. ]
//
///////////////////////////////////////////////////////////////////////////////

#include "expression.hpp"
#include "vm.hpp"
#include "compiler.hpp"

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

    typedef std::string::const_iterator iterator_type;
    typedef client::expression<iterator_type> expression;
    typedef client::ast::program ast_program;
    typedef client::compiler compiler;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        client::vmachine mach;  // Our virtual machine
        std::vector<int> code;  // Our VM code
        expression calc;        // Our grammar
        ast_program program;    // Our program (AST)
        compiler compile(code); // Our compiler

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        boost::spirit::ascii::space_type space;
        bool r = phrase_parse(iter, end, calc, space, program);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            compile(program);
            mach.execute(code);
            std::cout << "\nResult: " << mach.top() << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(iter, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


