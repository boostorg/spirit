/*=============================================================================
    A Standard compliant C++ preprocessor

    Re2C based C++ lexer
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_RE2C_FUNCTOR_HPP__80E4E0B2_7C71_44FE_9F65_901BBCD5B840__INCLUDED_)
#define _RE2C_FUNCTOR_HPP__80E4E0B2_7C71_44FE_9F65_901BBCD5B840__INCLUDED_

#include "cpplexer/re2clex/cpp_re2c_lexer.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {

///////////////////////////////////////////////////////////////////////////////
//
//  re2c_functor class
//
//      The re2c_functor template class is provided for simplicity reasons. It 
//      imports the unified re2clex::lex_functor lexer interface template under 
//      a new name into the cpplexer namespace. 
//
//      As an alternative the lex_functor is to be explicitely decorated with
//      it's namespace (or imported by an using directive). This way it is 
//      possible by simply switching the used namespace to get different lexer 
//      implementations.
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT>
struct re2c_functor 
:   public re2clex::lex_functor<IteratorT>
{
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpplexer

#endif // !defined(_RE2C_FUNCTOR_HPP__80E4E0B2_7C71_44FE_9F65_901BBCD5B840__INCLUDED_)
