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

#if !defined(_CPP_EXPRESSION_GRAMMAR_GEN_HPP__42399258_6CDC_4101_863D_5C7D95B5A6CA__INCLUDED_)
#define _CPP_EXPRESSION_GRAMMAR_GEN_HPP__42399258_6CDC_4101_863D_5C7D95B5A6CA__INCLUDED_

#include <list>

#include "cpp/cpp_iteration_context.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace grammars {

///////////////////////////////////////////////////////////////////////////////
//  
//  expression_grammar_gen template class
//
//      This template helps separating the compilation of the 
//      expression_grammar class from the compilation of the main 
//      pp_iterator. This is done to safe compilation time.
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
struct expression_grammar_gen {

    static bool evaluate(
        typename std::list<TokenT>::const_iterator const &first, 
        typename std::list<TokenT>::const_iterator const &last, 
        TokenT const &tok);
};

///////////////////////////////////////////////////////////////////////////////
}   //  namespace grammars
}   //  namespace cpp 

#endif // !defined(_CPP_EXPRESSION_GRAMMAR_GEN_HPP__42399258_6CDC_4101_863D_5C7D95B5A6CA__INCLUDED_)
