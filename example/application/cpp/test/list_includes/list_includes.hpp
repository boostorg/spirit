/*=============================================================================
    A Standard compliant C++ preprocessor

    Sample: List include dependencies of a given source file
            Configuration data
        
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_LIST_INCLUDES_HPP__843DB412_3AA8_4BCF_8081_AA4A5FDE0BE7__INCLUDED_)
#define _LIST_INCLUDES_HPP__843DB412_3AA8_4BCF_8081_AA4A5FDE0BE7__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
// decide, which C++ lexer to use (choose one!)
#define USE_SLEX_CPP_LEXER         // use the SLex based C++ lexer
//#define USE_RE2C_CPP_LEXER       // use the Re2C based C++ lexer

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
//
//  Use the separation model for the instantiation of the lex_functor object.
//
//  If this is defined, you should explicitly instantiate the lex_functor
//  template with the correct parameters in a separate compilation unit of
//  your program (see the file instantiate_slex_lexer.cpp). 
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

#endif // !defined(_LIST_INCLUDES_HPP__843DB412_3AA8_4BCF_8081_AA4A5FDE0BE7__INCLUDED_)
