/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the abstact lexer interface
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_LEX_INTERFACE_HPP__E83F52A4_90AC_4FBE_A9A7_B65F7F94C497__INCLUDED_)
#define _CPP_LEX_INTERFACE_HPP__E83F52A4_90AC_4FBE_A9A7_B65F7F94C497__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {

///////////////////////////////////////////////////////////////////////////////
//
//  The lex_input_interface decouples the lex_iterator_shim from the actual 
//  lexer. This is done to allow compile time reduction.
//  Thanks to JCAB for having this idea.
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT, typename PositionT = boost::spirit::file_position>
struct lex_input_interface 
{
    virtual TokenT get() = 0;
    virtual void set_position(PositionT const &pos) = 0;

//  The NewLexer function allows the opaque generation of a new lexer object.
//  It is coupled to the token type to allow to distinguish different 
//  lexer/token configurations at compile time.
    static lex_input_interface *
    new_lexer(typename TokenT::iterator_t const &first, 
        typename TokenT::iterator_t const &last, PositionT const &pos)
    { return TokenT::lexer_gen_t::new_lexer (first, last, pos); }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpplexer

#endif // !defined(_CPP_LEX_INTERFACE_HPP__E83F52A4_90AC_4FBE_A9A7_B65F7F94C497__INCLUDED_)
