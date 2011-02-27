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

#include "statement.hpp"
#include "vm.hpp"
#include "compiler.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Statement parser...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type some statements... ";
    std::cout << "An empty line ends input, compiles, runs and prints results\n\n";

    std::string str;
    std::string source;
    while (std::getline(std::cin, str))
    {
        if (str.empty())
            break;
        source += str + '\n';
    }

    typedef std::string::const_iterator iterator_type;
    iterator_type iter = source.begin();
    iterator_type end = source.end();

    client::vmachine vm;                        // Our virtual machine
    client::program program;                    // Our VM program
    client::statement<iterator_type> parser;    // Our parser
    client::ast::statement ast;                 // Our AST
    client::compiler compile(program);          // Our compiler

    boost::spirit::ascii::space_type space;
    bool r = phrase_parse(iter, end, parser, space, ast);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        compile(ast);
        vm.execute(program());

        std::cout << "Results------------------\n\n";
        program.print_variables(vm.get_stack());
        std::cout << "-------------------------\n\n";
    }
    else
    {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
    }

    return 0;
}


