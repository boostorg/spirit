//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_LEXER_EXAMPLE_WORD_COUNT_LEXER_TOKENS_FEB_10_2008_0739PM)
#define SPIRIT_LEXER_EXAMPLE_WORD_COUNT_LEXER_TOKENS_FEB_10_2008_0739PM

#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Token definition: We use the lexertl based lexer engine as the underlying 
//                    lexer type.
//
//  Note, the token definition type is derived from the 'lexertl_actor_lexer'
//  template, which is a necessary to being able to use lexer semantic actions.
///////////////////////////////////////////////////////////////////////////////
//[wcl_static_token_definition
template <typename Lexer>
struct word_count_lexer_tokens : boost::spirit::lex::lexer<Lexer>
{
    word_count_lexer_tokens()
      : c(0), w(0), l(0)
      , word("[^ \t\n]+")     // define tokens
      , eol("\n")
      , any(".")
    {
        using boost::spirit::_1;
        using boost::phoenix::ref;
        using boost::phoenix::distance;

        // associate tokens with the lexer
        this->self 
            =   word  [++ref(w), ref(c) += distance(_1)]
            |   eol   [++ref(c), ++ref(l)] 
            |   any   [++ref(c)]
            ;
    }

    std::size_t c, w, l;
    boost::spirit::lex::token_def<> word, eol, any;
};
//]

#endif
