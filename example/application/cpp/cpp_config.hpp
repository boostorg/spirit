/*=============================================================================
    A Standard compliant C++ preprocessor

    Global application configuration
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_CONFIG_HPP__F143F90A_A63F_4B27_AC41_9CA4F14F538D__INCLUDED_)

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to implement a #warning directive as 
//
//  To implement #warning directives, undefine the following
//
#define CPP_SUPPORT_WARNING_DIRECTIVE

///////////////////////////////////////////////////////////////////////////////
//  Undefine the following, to enable some MS specific language extensions:
//  __int8, __int16, __int32, __int64, __based, __declspec, __cdecl, 
//  __fastcall, __stdcall, __try, __except, __finally, __leave, __inline,
//  __asm
//#define CPP_SUPPORT_MS_EXTENSIONS

///////////////////////////////////////////////////////////////////////////////
//  Allow the message body of the #error and #warning directives to be 
//  preprocessed before the diagnostic is issued.
//
//  Undefining the following will preprocess the message bodies of #error and
//  #warning messages before the error (warning) is issued
//
#define CPP_PREPROCESS_ERROR_MESSAGE_BODY

///////////////////////////////////////////////////////////////////////////////
//  Allow the #pragma directives to be returned to the caller (optionally after 
//  preprcessing the body) 
//
//  Undefining the following will skip #pragma directives, so that the caller
//  will not see them.
//
#define CPP_RETURN_PRAGMA_DIRECTIVES

///////////////////////////////////////////////////////////////////////////////
//  Allow the body of a #pragma directive to be preprocessed before the 
//  directive is returned to the caller.
//
//  Undefining the following will preprocess the bodies of #pragma directives
//
#define CPP_PREPROCESS_PRAGMA_BODY

///////////////////////////////////////////////////////////////////////////////
//  Allow to return an empty line in place of recognized preprocessor 
//  directives. If this constant is not defined, then for recognized 
//  preprocessor directives is returned no token at all. This allows to make 
//  the overall output more condensed
//
//  Undefining the following will return a single newline token in place of a
//  recognized preprocessor directive
//
#define CPP_RETURN_EMPTY_LINES_FOR_DIRECTIVES

///////////////////////////////////////////////////////////////////////////////
// Decide, which C++ lexer to use (choose one!)
//
//#define USE_SLEX_CPP_LEXER         // use the SLex based C++ lexer
#define USE_RE2C_CPP_LEXER       // use the Re2C based C++ lexer

#if (defined(USE_SLEX_CPP_LEXER) && defined(USE_RE2C_CPP_LEXER)) || \
    (!defined(USE_SLEX_CPP_LEXER) && !defined(USE_RE2C_CPP_LEXER))
#error "Please choose exactly one C++ lexer to use" \
    " (define USE_SLEX_CPP_LEXER or USE_RE2C_CPP_LEXER)"
#endif

///////////////////////////////////////////////////////////////////////////////
//  Undefine the following, if you need debug output, the 
//  BOOST_SPIRIT_DEBUG_FLAGS constants below help to fine control the amount of 
//  the generated debug output
//#define BOOST_SPIRIT_DEBUG

///////////////////////////////////////////////////////////////////////////////
//  debug rules, subrules and grammars only, for possible flags see 
//  spirit/debug.hpp
#define BOOST_SPIRIT_DEBUG_FLAGS  \
    (BOOST_SPIRIT_DEBUG_FLAGS_NODES | BOOST_SPIRIT_DEBUG_FLAGS_CLOSURES) \
    /**/

///////////////////////////////////////////////////////////////////////////////
//  debug flags for the pp-iterator library, possible flags:
#define BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR        0x0001
#define BOOST_SPIRIT_DEBUG_FLAGS_TIME_CONVERSION    0x0002
#define BOOST_SPIRIT_DEBUG_FLAGS_CPP_EXPR_GRAMMAR   0x0004
#define BOOST_SPIRIT_DEBUG_FLAGS_INTLIT_GRAMMAR     0x0008
#define BOOST_SPIRIT_DEBUG_FLAGS_FLOATLIT_GRAMMAR   0x0010
#define BOOST_SPIRIT_DEBUG_FLAGS_CHLIT_GRAMMAR      0x0020
#define BOOST_SPIRIT_DEBUG_FLAGS_DEFINED_GRAMMAR    0x0040
#define BOOST_SPIRIT_DEBUG_FLAGS_PREDEF_MACROS_GRAMMAR  0x0080

#define BOOST_SPIRIT_DEBUG_FLAGS_CPP \
    BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR \
    /**/

///////////////////////////////////////////////////////////////////////////////
//
//  For all recognized preprocessor statements the output parse trees 
//  formatted as xml are printed. The formatted parse trees are streamed to the 
//  std::ostream given through the CPP_DUMP_PARSE_TREE_OUT constant.
//
//  Undefine the following, if you want to see these parse trees. 
//
//#define CPP_DUMP_PARSE_TREE
//#define CPP_DUMP_PARSE_TREE_OUT std::cout

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to use the separate compilation model for the instantiation 
//  of the C++ lexer objects.
//
//  If this is defined, you should explicitly instantiate the C++ lexer
//  template with the correct parameters in a separate compilation unit of
//  your program (see the files instantiate_slex_lexer.cpp and
//  instantiate_re2c_lexer.cpp). 
//
//  To use the lexer inclusion model, undefine the following 
//
#define CPP_SEPARATE_LEXER_INSTANTIATION

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to use the separate compilation model for the instantiation 
//  of the grammar objects.
//
//  If this is defined, you should explicitly instantiate the grammar
//  templates with the correct parameters in a separate compilation unit of
//  your program (see the files instantiate_cpp_grammar.cpp). 
//
//  To use the grammar inclusion model, undefine the following 
//
#define CPP_SEPARATE_GRAMMAR_INSTANTIATION

///////////////////////////////////////////////////////////////////////////////
//  You shouldn't have to change anything below

#if defined(BOOST_MSVC) && !defined(__COMO__)
#pragma warning (disable: 4355) // 'this' used in base member initializer list
#pragma inline_depth(255)
#pragma inline_recursion(on)
#endif // defined(_BOOST_MSVC)

#endif // !defined(_CPP_CONFIG_HPP__F143F90A_A63F_4B27_AC41_9CA4F14F538D__INCLUDED_)
