/*=============================================================================
    A Standard compliant C++ preprocessor
    
    Sample: prints out the preprocessed tokens returned by the pp iterator

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledegments.
=============================================================================*/

#include <iostream>
#include <fstream>
#include <string>

#include "cpp_tokens_config.hpp"                  // global configuration

#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"

#include "cpp/cpp_context.hpp"

///////////////////////////////////////////////////////////////////////////////
//  include lexer specifics, import lexer names
#if defined(USE_SLEX_CPP_LEXER)
// use the slex based C++ lexer
#include "cpplexer/slex/cpp_slex_token.hpp"

#if !defined(CPP_SEPARATE_LEXER_INSTANTIATION)
#include "cpplexer/slex/cpp_slex_lexer.hpp"
#endif // !defined(CPP_SEPARATE_LEXER_INSTANTIATION)

using namespace cpplexer::slex;

#elif defined(USE_RE2C_CPP_LEXER)
// use the re2c based C++ lexer
#include "cpplexer/re2clex/cpp_re2c_token.hpp"

#if !defined(CPP_SEPARATE_LEXER_INSTANTIATION)
#include "cpplexer/re2clex/cpp_re2c_lexer.hpp"
#endif // !defined(CPP_SEPARATE_LEXER_INSTANTIATION)

using namespace cpplexer::re2clex;

#endif

///////////////////////////////////////////////////////////////////////////////
//  import required names
using namespace boost::spirit;

using std::string;
using std::getline;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

///////////////////////////////////////////////////////////////////////////////
//  main program
int
main(int argc, char *argv[])
{
    if (2 != argc) {
        cout << "Usage: <cpp_tokens file>" << endl;
        return 1;
    }
    
    ifstream infile(argv[1]);
    string teststr;
    if (infile.is_open()) {
        string line;
        for (getline(infile, line); infile.good(); getline(infile, line)) {
            teststr += line;
            teststr += '\n';
        }
    }
    else {
        teststr = argv[1];
    }

// The template lex_token<> is defined in both namespaces: cpplexer::slex and 
// cpplexer::re2clex. The 'using namespace' directive above tells the compiler,
// which of them to use.
    typedef cpp::context<lex_token<std::string::iterator> > context_t;

// The C preprocessor iterator shouldn't be constructed directly. It is to be
// generated through a cpp::context<> object. This cpp:context<> object is 
// additionally to be used to initialize and define different parameters of 
// the actual preprocessing.
// The preprocessing of the input stream is done on the fly behind the scenes
// during iteration over the context_t::iterator_t stream.
context_t ctx (teststr.begin(), teststr.end(), argv[1]);
context_t::iterator_t first = ctx.begin();
context_t::iterator_t last = ctx.end();
context_t::token_t current_token;

    try {
        while (first != last) {
        // retrieve next token
            current_token = *first;
        
        // find token name
        string tokenname (cpplexer::get_token_name(cpplexer::token_id(current_token)));

        // output token info
            cout << "matched token " 
                << tokenname 
                << "(#" << cpplexer::token_id(ID_FROM_TOKEN(current_token)) 
                << ") at " << current_token.get_position().file << " (" 
                    << current_token.get_position().line << "/" 
                    << current_token.get_position().column 
                << "): >" << current_token.get_value() << "<"
                << endl;
            ++first;
        }
    }
    catch (cpp::abort_preprocess_exception &e) {
    // abort the preprocessing: simply abort compilation
        cerr 
            << e.file_name() << "(" << e.line_no() << "): "
            << "aborting preprocessing." 
            << endl;
        return 1;
    }
    catch (cpp::cpp_exception &e) {
    // some preprocessing error
        cerr 
            << e.file_name() << "(" << e.line_no() << "): "
            << e.description() << endl;
        return 2;
    }
    catch (std::exception &e) {
    // use last recognized token to retrieve the error position
        cerr 
            << current_token.get_position().file 
            << "(" << current_token.get_position().line << "): "
            << "unexpected exception: " << e.what()
            << endl;
        return 3;
    }
    catch (...) {
    // use last recognized token to retrieve the error position
        cerr 
            << current_token.get_position().file 
            << "(" << current_token.get_position().line << "): "
            << "unexpected exception." << endl;
        return 4;
    }
    return 0;
}
