//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2009 Tor Brede Vekterli
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// #define BOOST_SPIRIT_LEXERTL_DEBUG
#define BOOST_VARIANT_MINIMIZE_SIZE

#include <boost/detail/lightweight_test.hpp>
#include <boost/config/warning_disable.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

#include <iostream>
#include <string>

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::lex;

enum tokenids
{
    IDANY = lex::min_token_id + 10 // Lower 8 bits is 0x0a, same as '\n'
};

template <typename Lexer>
struct word_count_tokens : lexer<Lexer>
{
    word_count_tokens()
    {
        this->self.add_pattern
            ("TEST", "A")
        ;
        word = "{TEST}";
        this->self.add
            (word)
            ('\n')
            (".", IDANY)
        ;
    }
    token_def<std::string> word;
};

template <typename Iterator>
struct word_count_grammar : grammar<Iterator>
{
    template <typename TokenDef>
    word_count_grammar(TokenDef const& tok)
      : word_count_grammar::base_type(start)
      , c(0), w(0), l(0)
    {
        using boost::phoenix::ref;
        using boost::phoenix::size;

        start =  *(   tok.word      [++ref(w)]
                  |   lit('\n')     [++ref(l)]
                  |   token(IDANY)  [++ref(c)] // Will never actually be invoked
                  )
              ;
    }
    std::size_t c, w, l;
    rule<Iterator> start;
};


int main(int argc, char* argv[])
{
    typedef lexertl::token<
        const char*, boost::mpl::vector<std::string>
    > token_type;

    typedef lexertl::lexer<token_type> lexer_type;
    typedef word_count_tokens<lexer_type>::iterator_type iterator_type;
    word_count_tokens<lexer_type> word_count;          // Our lexer
    word_count_grammar<iterator_type> g (word_count);  // Our parser

    std::string str ("A\nBCDEFGHI");
    char const* first = str.c_str();
    char const* last = &first[str.size()];

    BOOST_TEST(tokenize_and_parse(first, last, word_count, g));
    BOOST_TEST(g.l == 1 && g.w == 1 && g.c == 8);

    return boost::report_errors();
}
