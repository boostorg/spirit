/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the lexer iterator 
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_LEX_ITERATOR_HPP__AF0C37E3_CBD8_4F33_A225_51CF576FA61F__INCLUDED_)
#define _CPP_LEX_ITERATOR_HPP__AF0C37E3_CBD8_4F33_A225_51CF576FA61F__INCLUDED_

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "cpplexer/cpp_lex_interface.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace impl {

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_iterator_functor_shim
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT> 
class lex_iterator_functor_shim 
{
public:
    template <typename IteratorT>
    lex_iterator_functor_shim(IteratorT const &first, IteratorT const &last, 
            typename TokenT::position_t const &pos) 
    :   functor_ptr(lex_input_interface<TokenT>::new_lexer(first, last, pos)) 
    {}

// interface to the boost::spirit::multi_pass_policies::functor_input policy
    typedef TokenT result_type;

// VC7.1 gives a linker error, if the following is defined static
    /*static*/ result_type const eof;
    
    result_type operator()() 
    { 
        BOOST_ASSERT(0 != functor_ptr.get());
        return functor_ptr->get(); 
    }
    void set_position(typename TokenT::position_t const &pos)
    {
        BOOST_ASSERT(0 != functor_ptr.get());
        functor_ptr->set_position(pos);
    }
    
private:
    boost::shared_ptr<lex_input_interface<TokenT> > functor_ptr;
};

///////////////////////////////////////////////////////////////////////////////
//  eof token
// VC7.1 gives a linker error, if the following is defined static
//template <typename LexT>
//typename lex_iterator_functor_shim<LexT>::result_type const
//    lex_iterator_functor_shim<LexT>::eof;

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_iterator
//
//      A generic C++ lexer interface class, which allows to plug in different
//      lexer implementations (template parameter LexT). The following 
//      requirement apply:
//
//          - the lexer type should have a function implemented, which returnes
//            the next lexed token from the input stream:
//                typename LexT::token_t get();
//          - at the end of the input stream this function should return the
//            eof token equivalent
//          - the lexer should implement a constructor taking two iterators
//            pointing to the beginning and the end of the input stream and
//            a third parameter containing the name of the parsed input file 
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
class lex_iterator 
:   public boost::spirit::multi_pass<
        impl::lex_iterator_functor_shim<TokenT>,
        boost::spirit::multi_pass_policies::functor_input
    >
{
    typedef impl::lex_iterator_functor_shim<TokenT> input_policy_t;
    typedef 
        boost::spirit::multi_pass<input_policy_t, 
                boost::spirit::multi_pass_policies::functor_input>
        base_t;
    typedef lex_iterator<TokenT>                    self_t;
    
public:
    typedef typename input_policy_t::result_type    token_t;
    
    lex_iterator()
    {}
    
    template <typename IteratorT>
    lex_iterator(IteratorT const &first, IteratorT const &last, 
            typename TokenT::position_t const &pos)
    :   base_t(input_policy_t(first, last, pos))
    {}
    
    void set_position(typename TokenT::position_t pos)
    { 
        using namespace cpplexer;
        
    // If the lookahead token is a newline or a C++ comment (wich contains the
    // trailing newline character), we have to increment the linecounter. 
        get_input().set_position(pos); 
        if (T_NEWLINE == token_id(get_input()) || 
            T_CPPCOMMENT == token_id(get_input()))
        {
            ++pos.line;
        }
        get_functor().set_position(pos); 
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpplexer

#endif // !defined(_CPP_LEX_ITERATOR_HPP__AF0C37E3_CBD8_4F33_A225_51CF576FA61F__INCLUDED_)
