//
// C++ Lexer implemented with Spirit (http://spirit.sourceforge.net/)
//
// Copyright© 2002 Juan Carlos Arevalo-Baeza, All rights reserved
// email: jcab@JCABs-Rumblings.com
// Created: 8-Nov-2002
//
// The basics:
// The lexer is an iterator that iterates over tokens generated on the fly
// from a text input.
// In order to use it, you create a "lex_iterator" object, and initialize it
// with the beginning and end iterators of the text input, and a string that
// represents the name of the input file (for inclusion into the tokens).
// This purposefully doesn't export any Spirit parser definitions, so access
// to the input sequence must be done through a generic run-time interface
// (virtual functions). The multi_pass iterator adapter helps handle this
// very well.
//

#ifndef cpp_lexer_hpp_included
#define cpp_lexer_hpp_included

#include "cpp_lexer_token.hpp"

#include <boost/spirit/iterator/multi_pass.hpp>

///////////////////////////////////////////////////////////////////////////////
// Lexer proper.

struct lex_input_interface {
protected:
    virtual ~lex_input_interface() {}

public:
    virtual void add_ref() = 0;
    virtual void dec_ref() = 0;

    virtual Token get() = 0;

    virtual boost::spirit::file_position get_position() = 0;

    Token const& eof();
};

lex_input_interface*
NewLexer(char const* first,
         char const* last,
         char const* fname = "<filename>");

struct lex_input_policy {
public:
    typedef Token result_type;

    lex_input_interface* interf;

    lex_input_policy(lex_input_policy const& other):
        interf(other.interf)
    {
        interf->add_ref();
    }
    lex_input_policy& operator=(lex_input_policy const& other)
    {
        if (other.interf != interf) {
            other.interf->add_ref();
            interf->dec_ref();
            interf = other.interf;
        }
        return *this;
    }

    template < typename IteratorT >
    lex_input_policy(IteratorT const& first,
                     IteratorT const& last,
                     char const* fname = "<filename>"):
        interf(NewLexer(first, last, fname))
    {
    }
    ~lex_input_policy() {
        interf->dec_ref();
    }
    result_type operator()() {
        return interf->get();
    }
    static Token const eof;
};

struct lex_iterator:
    boost::spirit::multi_pass<
        lex_input_policy,
        boost::spirit::multi_pass_policies::functor_input
/*      ,
        multi_pass_policies::first_owner,
        multi_pass_policies::no_check,
        multi_pass_policies::std_deque
*/
    >
{
    typedef boost::spirit::multi_pass<
                lex_input_policy,
                boost::spirit::multi_pass_policies::functor_input
            > base_t;
    typedef lex_iterator self_t;

    lex_iterator() {}

    template < typename IteratorT >
    lex_iterator(IteratorT const& first,
                 IteratorT const& last,
                 char const* fname = "<filename>"):
        base_t(lex_input_policy(first, last, fname))
    {}
};

#endif
