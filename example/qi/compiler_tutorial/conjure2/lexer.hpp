/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_LEXER_HPP)
#define BOOST_SPIRIT_CONJURE_LEXER_HPP

#include "config.hpp"
#include "token_ids.hpp"

#if CONJURE_LEXER_STATIC_TABLES != 0
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "conjure_static_lexer.hpp"
#elif CONJURE_LEXER_STATIC_SWITCH != 0
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "conjure_static_switch_lexer.hpp"
#endif
#include <boost/assert.hpp>

namespace client { namespace lexer 
{
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        namespace lex = boost::spirit::lex;

        template <typename BaseIterator>
        struct get_lexer_type
        {
            // Our token needs to be able to carry several token values: 
            // std::string, unsigned int, and bool
            typedef boost::mpl::vector<std::string, unsigned int, bool> 
                token_value_types;

            // Using the position_token class as the token type to be returned
            // from the lexer iterators allows to retain positional information
            // as every token instance stores an iterator pair pointing to the
            // matched input sequence.
            typedef lex::lexertl::position_token<
                BaseIterator, token_value_types, boost::mpl::false_
            > token_type;

#if CONJURE_LEXER_DYNAMIC_TABLES != 0
            // use the lexer based on runtime generated DFA tables
            typedef lex::lexertl::actor_lexer<token_type> type;
#elif CONJURE_LEXER_STATIC_TABLES != 0
            // use the lexer based on pre-generated static DFA tables
            typedef lex::lexertl::static_actor_lexer<
                token_type
              , boost::spirit::lex::lexertl::static_::lexer_conjure_static
            > type;
#elif CONJURE_LEXER_STATIC_SWITCH != 0
            // use the lexer based on pre-generated static code
            typedef lex::lexertl::static_actor_lexer<
                token_type
              , boost::spirit::lex::lexertl::static_::lexer_conjure_static_switch
            > type;
#else
#error "Configuration problem: please select exactly one type of lexer to build"
#endif
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename BaseIterator>
    struct conjure_tokens 
      : lex::lexer<typename detail::get_lexer_type<BaseIterator>::type>
    {
    private:
      // get the type of any qi::raw_token(...) and qi::token(...) constructs
        typedef typename boost::spirit::result_of::terminal<
            boost::spirit::tag::raw_token(tokenids)
        >::type raw_token_spec;

        typedef typename boost::spirit::result_of::terminal<
            boost::spirit::tag::token(tokenids)
        >::type token_spec;

        typedef std::map<std::string, tokenids> keyword_map_type;

    protected:
        // add a keyword to the mapping table
        bool add_keyword(std::string const& keyword);

    public:
        typedef BaseIterator base_iterator_type;

        conjure_tokens();

        // extract a raw_token(id) when given a registered keyword
        raw_token_spec raw_token (std::string const& kwd) const
        {
            namespace qi = boost::spirit::qi;
            qi::raw_token_type raw_token;

            typename keyword_map_type::const_iterator it = keywords_.find(kwd);
            BOOST_ASSERT(it != keywords_.end());
            return qi::raw_token((it != keywords_.end()) ? (*it).second : ID_INVALID);
        }

        token_spec token (std::string const& kwd) const
        {
            namespace qi = boost::spirit::qi;
            qi::token_type token;

            typename keyword_map_type::const_iterator it = keywords_.find(kwd);
            BOOST_ASSERT(it != keywords_.end());
            return qi::token((it != keywords_.end()) ? (*it).second : ID_INVALID);
        }

        lex::token_def<std::string> identifier;
        lex::token_def<unsigned int> uint_;
        lex::token_def<bool> bool_;
        keyword_map_type keywords_;
    };
}}

#endif


