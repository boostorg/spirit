/*=============================================================================
    A Standard compliant C++ preprocessor

    SLex (Spirit Lex) based C++ lexer
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_SLEX_FUNCTOR_HPP__557F0176_1340_4DC0_8E5A_1297F6AF9D46__INCLUDED_)
#define _SLEX_FUNCTOR_HPP__557F0176_1340_4DC0_8E5A_1297F6AF9D46__INCLUDED_

#include "cpplexer/slex/cpp_slex_lexer.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {

///////////////////////////////////////////////////////////////////////////////
//
//  slex_functor class
//
//      The slex_functor template class is provided for simplicity reasons. It 
//      imports the unified slex::lex_functor lexer interface template under 
//      a new name into the cpplexer namespace. 
//
//      As an alternative the lex_functor is to be explicitely decorated with
//      it's namespace (or imported by an using directive). This way it is 
//      possible by simply switching the used namespace to get different lexer 
//      implementations.
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT>
struct slex_functor 
:   public slex::lex_functor<IteratorT>
{
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpplexer

#endif // !defined(_SLEX_FUNCTOR_HPP__557F0176_1340_4DC0_8E5A_1297F6AF9D46__INCLUDED_)
