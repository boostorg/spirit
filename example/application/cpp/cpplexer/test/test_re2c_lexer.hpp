/*=============================================================================
    A Standard compliant C++ preprocessor

    Test the Re2C based C++ lexer
    Configuration data
        
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_TEST_RE2C_LEXER_HPP__D17C10F4_25B5_4952_9B81_BB92A1B96B18__INCLUDED_)
#define _TEST_RE2C_LEXER_HPP__D17C10F4_25B5_4952_9B81_BB92A1B96B18__INCLUDED_

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
///////////////////////////////////////////////////////////////////////////////

#define CPP_SEPARATE_LEXER_INSTANTIATION

#endif // !defined(_TEST_RE2C_LEXER_HPP__D17C10F4_25B5_4952_9B81_BB92A1B96B18__INCLUDED_)
