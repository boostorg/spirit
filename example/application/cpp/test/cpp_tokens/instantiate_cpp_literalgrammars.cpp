/*=============================================================================
    A Standard compliant C++ preprocessor

    Sample: prints out the preprocessed tokens returned by the pp iterator

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#include "cpp_tokens_config.hpp"          // config data

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)

#include <string>

#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"

#include "cpp/cpp_literal_grammar_gen.hpp"  
#include "cpp/cpp_intlit_grammar.hpp"
#include "cpp/cpp_chlit_grammar.hpp"
#include "cpp/cpp_floatlit_grammar.hpp"

///////////////////////////////////////////////////////////////////////////////
//  decide, which C++ lexer to use
#if defined(USE_SLEX_CPP_LEXER)
// use the slex based C++ lexer
#include "cpplexer/slex/cpp_slex_token.hpp"
using namespace cpplexer::slex;

#elif defined(USE_RE2C_CPP_LEXER)
// use the re2c based C++ lexer
#include "cpplexer/re2clex/cpp_re2c_token.hpp"
using namespace cpplexer::re2clex;

#endif

///////////////////////////////////////////////////////////////////////////////
//  
//  Explicit instantiation of the intlit_grammar_gen, chlit_grammar_gen and
//  floatlit_grammar_gen templates with the correct token type. This 
//  instantiates the corresponding parse function, which in turn instantiates 
//  the corresponding parser object.
//
///////////////////////////////////////////////////////////////////////////////

template cpp::intlit_grammar_gen<lex_token<std::string::iterator> >;
template cpp::chlit_grammar_gen<lex_token<std::string::iterator> >;
template cpp::floatlit_grammar_gen<lex_token<std::string::iterator> >;

#endif // #if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)

