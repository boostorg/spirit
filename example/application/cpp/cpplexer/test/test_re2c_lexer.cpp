/*=============================================================================
    A Standard compliant C++ preprocessor

    Test the Re2C based C++ lexer
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgments.
=============================================================================*/

#include <iostream>
#include <fstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
//  include required boost libraries
#include <boost/assert.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "test_re2c_lexer.hpp"      // config data

#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"
#include "cpplexer/re2clex/cpp_re2c_token.hpp"

#if !defined(CPP_SEPARATE_LEXER_INSTANTIATION)
#include "cpplexer/re2clex/cpp_re2c_lexer.hpp"
#endif

using namespace boost::spirit;
using namespace cpplexer;

using std::string;
using std::getline;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

///////////////////////////////////////////////////////////////////////////////
//  main entry point
int main(int argc, char** argv)
{
    if (2 != argc) {
        cout << "Usage: <cpp file>" << endl;
        return 1;
    }
    
    ifstream infile(argv[1]);
    string teststr;
    if (infile) {
        string line;
        for (getline(infile, line); infile.good(); getline(infile, line)) {
            teststr += line;
            teststr += '\n';
        }
    }
    else {
        teststr = argv[1];
    }

//  As you can see, the slex::lex_functor object, which is hidden behind the 
//  lex_iterator isn't explicit mentioned here. This allows to separate the 
//  compilation of the slex::lex_functor and reduces compilation time.
    using namespace cpplexer::re2clex;

char const *begin = teststr.c_str();
char const *end = begin + teststr.size();
lex_iterator<lex_token<> > first (begin, end, argv[1]);
lex_iterator<lex_token<> > last;
lex_token<> current_token;

    try {
        while (first != last) {
            current_token = *first;
        
        // find token name
        string tokenname (get_token_name(token_id(current_token)));

        // output token info
            cout << "matched token " 
                << tokenname 
                << "(#" << token_id(ID_FROM_TOKEN(current_token)) << "): " 
                << ">" << current_token.get_value() << "<"
                << endl;
            ++first;
        }
    }
    catch (std::exception &e) {
    // use last recognized token to retrieve the error position
        cerr 
            << current_token.get_position().file 
            << "(" << current_token.get_position().line << "): "
            << "unexpected exception: " << e.what()
            << endl;
        return 1;
    }
    catch (...) {
    // use last recognized token to retrieve the error position
        cerr 
            << current_token.get_position().file 
            << "(" << current_token.get_position().line << "): "
            << "unexpected exception." << endl;
        return 2;
    }
    return 0;
}
