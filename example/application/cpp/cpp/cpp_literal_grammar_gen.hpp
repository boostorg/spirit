/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_LITERAL_GRAMMAR_GEN_HPP__67794A6C_468A_4AAB_A757_DEDDB182F5A0__INCLUDED_)
#define _CPP_LITERAL_GRAMMAR_GEN_HPP__67794A6C_468A_4AAB_A757_DEDDB182F5A0__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace grammars {

///////////////////////////////////////////////////////////////////////////////
//  
//  cpp_intlit_grammar_gen template class
//
//      This template helps separating the compilation of the intlit_grammar 
//      class from the compilation of the expression_grammar. This is done 
//      to safe compilation time.
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
struct intlit_grammar_gen {

    static unsigned long evaluate(TokenT const &tok);
};

///////////////////////////////////////////////////////////////////////////////
//  
//  cpp_chlit_grammar_gen template class
//
//      This template helps separating the compilation of the chlit_grammar 
//      class from the compilation of the expression_grammar. This is done 
//      to safe compilation time.
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
struct chlit_grammar_gen {

    static unsigned int evaluate(TokenT const &tok);
};

///////////////////////////////////////////////////////////////////////////////
//  
//  cpp_floatlit_grammar_gen template class
//
//      This template helps separating the compilation of the floatlit_grammar 
//      class from the compilation of the expression_grammar. This is done 
//      to safe compilation time.
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
struct floatlit_grammar_gen {

    static double evaluate(TokenT const &tok);
};

///////////////////////////////////////////////////////////////////////////////
}   //  namespace grammars
}   //  namespace cpp 

#endif // !defined(_CPP_LITERAL_GRAMMAR_GEN_HPP__67794A6C_468A_4AAB_A757_DEDDB182F5A0__INCLUDED_)
