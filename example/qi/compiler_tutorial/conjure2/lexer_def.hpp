/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "lexer.hpp"

namespace client { namespace lexer
{
    template <typename BaseIterator>
    conjure_tokens<BaseIterator>::conjure_tokens()
      : identifier("[a-zA-Z_][a-zA-Z_0-9]*", token::identifier)
      , lit_uint("[0-9]+", token::lit_uint)
      , true_or_false("true|false", token::true_or_false)
    {
        lex::_pass_type _pass;

        this->self = lit_uint | true_or_false;

        add_keyword("void");
        add_keyword("int");
        add_keyword("if");
        add_keyword("else");
        add_keyword("while");
        add_keyword("return");

        this->self.add
                ("\\|\\|", token::logical_or)
                ("&&", token::logical_and)
                ("==", token::equal)
                ("!=", token::not_equal)
                ("<", token::less)
                ("<=", token::less_equal)
                (">", token::greater)
                (">=", token::greater_equal)
                ("\\+", token::plus)
                ("\\-", token::minus)
                ("\\*", token::times)
                ("\\/", token::divide)
                ("!", token::not_)
            ;

        this->self += lex::char_('(') | ')' | '{' | '}' | ',' | '=' | ';';

        this->self +=
                identifier
            |   lex::string("\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/", token::comment)
                [
                    lex::_pass = lex::pass_flags::pass_ignore
                ]
            |   lex::string("[ \t\n\r]+", token::whitespace)
                [
                    lex::_pass = lex::pass_flags::pass_ignore
                ]
            ;
    }

    template <typename BaseIterator>
    bool conjure_tokens<BaseIterator>::add_keyword(std::string const& keyword)
    {
        // add the token to the lexer
        token::type id = token::type(this->get_next_id());
        this->self.add(keyword, id);

        // store the mapping for later retrieval
        std::pair<typename keyword_map_type::iterator, bool> p =
            keywords_.insert(typename keyword_map_type::value_type(keyword, id));

        return p.second;
    }
}}

