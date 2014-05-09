/*=============================================================================
  Copyright (c) 2014 Thomas Benard

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
  ==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_KEYWORD_NOV_29_2013)
#define BOOST_SPIRIT_X3_KEYWORD_NOV_29_2013

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/string/detail/string_parse.hpp>
#include <boost/spirit/home/x3/support/utility/utf8.hpp>
#include <boost/spirit/home/x3/support/utility/unrefcv.hpp>
#include <boost/spirit/home/x3/support/utility/integer_sequence.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>
#include <boost/spirit/home/support/char_encoding/standard.hpp>
#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>

#include <boost/spirit/home/x3/string/symbols.hpp>

#include <string>

namespace boost { namespace spirit { namespace x3 { namespace detail {

    // Helper to filter out unused attributes
    template <int... Is>
        struct attribute_index {};

    template <typename Context, typename Keywords, int AttrIdx,int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq;

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, bool HasAttr, int ... Is>
        struct gen_attribute_seq_detail : gen_attribute_seq< Context, Keywords, AttrIdx+1, ParserIdx +1, N-1, Is ..., AttrIdx> {};

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq_detail<Context,Keywords,AttrIdx, ParserIdx, N, false, Is...> : gen_attribute_seq< Context, Keywords, AttrIdx, ParserIdx +1, N-1, Is..., -1 > {};

    template <typename Context, typename Keywords, int AttrIdx,int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq : 
            gen_attribute_seq_detail<Context, Keywords, AttrIdx, ParserIdx, N, traits::has_attribute< typename std::tuple_element<ParserIdx, Keywords>::type, Context >::value, Is...> {};

    template <typename Context, typename Keywords, int AttrIdx,int ParserIdx, int ... Is>
        struct gen_attribute_seq<Context, Keywords, AttrIdx, ParserIdx, 0, Is...> : attribute_index<Is...> {};

    template <int N, int... Is>
        struct gen_unused_attribute_seq : gen_unused_attribute_seq<N - 1, - 1, Is...> {};

    template <int... Is>
        struct gen_unused_attribute_seq<0, Is...> : attribute_index<Is...> {};

    template <typename Attribute, int Index>
        struct get_attribute
        {
            typedef typename fusion::result_of::at_c<Attribute,Index>::type type;
            static typename add_reference<type>::type call(Attribute &s)
            {
                return fusion::at_c<Index>(s);
            }

        };

    template <typename Attribute>
        struct get_attribute<Attribute, -1 >
        {
            static unused_type& call(Attribute &s)
            {
                static unused_type result;
                return result;
            }
        };

    struct keyword_string
    {
        template <typename Lookup, typename Keyword>
            static void add(Lookup &lookup, Keyword const &parser, int index)
            {
                lookup.add(traits::get_string_begin<char>(parser.key.str),
                        traits::get_string_end<char>(parser.key.str),
                        index);
            }
        template <typename Lookup, typename Keyword,typename Action>
            static void add(Lookup &lookup, action<Keyword,Action> const &parser, int index)
            {
                lookup.add(traits::get_string_begin<char>(parser.subject.key.str),
                        traits::get_string_end<char>(parser.subject.key.str),
                        index);
            }
    };


}}}}


namespace boost { namespace spirit { namespace x3 {

    // simple keyword parser
    template <typename Key, typename Subject>
        struct keyword_parser : unary_parser<Subject, keyword_parser<Key,Subject> >
    {
        typedef unary_parser<Subject, keyword_parser<Key,Subject> > base_type;

        static const bool is_pass_through_unary =true;

        keyword_parser(Key const &key, Subject const& subject)
            : base_type(subject)
              , key(key)
        {}

        template <typename Iterator, typename Context, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
                    , Context const& context, Attribute& attr) const
            {
                Iterator save = first;
                if (! key.parse(first, last, context, unused))
                {
                    first = save;
                    return false;
                }
                return this->subject.parse(first,last,context,attr);
            }
        Key key;
    };
    // Two stage generator for the kwd parser / directive
    struct kwd_gen
    {

        template <typename Key>
            struct kwd_level_1
            {
                kwd_level_1(Key const& key)
                    :key(key)
                {
                }

                template <typename Subject>
                    keyword_parser< Key
                    ,typename extension::as_parser<Subject>::value_type >
                    operator[](Subject const& subject) const
                    {
                        return {key,as_parser(subject)};
                    }
                Key key;
            };

        template <typename Key>
            kwd_level_1<typename extension::as_parser<Key>::value_type >
            operator()(Key const &key) const
            {
                return {as_parser(key)};
            }
    };
    kwd_gen const kwd = kwd_gen();

    // keywords parser 

    template <typename ...Keywords> 
        struct keywords_parser : parser< keywords_parser< Keywords ...> >
    {             

        typedef parser< keywords_parser< Keywords...> > base_type;

        static const std::size_t nb_keywords = sizeof...(Keywords);

        keywords_parser(Keywords ... keywords)
            : keywords(std::forward<Keywords>(keywords)...)
        {
            add_keywords(0,keywords...);
        }

        // construct the lookup table
        void add_keywords(int index)
        {
        }

        template <typename keywordType, typename ... rest>
            void add_keywords(int index, keywordType const &keyword, rest const&... keywords)
            {
                detail::keyword_string::add(lookup,keyword,index);
                add_keywords(index+1,keywords ...);
            }

        // Subject parser calling function
        template <int ParserIdx, typename GetAttribute, typename Iterator, typename Context, typename Attribute>
            bool parse_subject(Iterator& first, Iterator const& last
                    , Context const& context, Attribute& attr) const
            {
                return std::get<ParserIdx>(keywords).subject.parse(first,last,context,GetAttribute::call(attr));
            }
        // Internal dispatching function used to call the correct parser depending on the keyword
        template <typename Iterator, typename Context, typename Attribute, std::size_t... Is, int... As>
            bool parse_internal(Iterator& first, Iterator const& last
                    , Context const& context, Attribute& attr, index_sequence<Is...>, detail::attribute_index<As...> ) const
            {
                typedef bool (keywords_parser::* subject_caller) (Iterator &, Iterator const&, Context const&, Attribute &) const;
                static subject_caller parse_functions[nb_keywords] =
                {
                    &keywords_parser::parse_subject<Is,detail::get_attribute<Attribute, As>,Iterator,Context,Attribute>...
                };
                x3::skip_over(first, last, context);

                while(std::size_t *subparser_idx =
                        lookup.find(first,last,tst_pass_through()))
                {
                    if(!(this->*parse_functions[*subparser_idx])(first,last,context,attr))
                        return false;
                    x3::skip_over(first, last, context);
                }
                return true;
            }

        // Parser entry
        template <typename Iterator, typename Context, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
                    , Context const& context, Attribute& attr) const
            {
                return parse_internal(
                        first,last,context,attr,
                        make_index_sequence<nb_keywords>{},
                        typename std::conditional< 
                        std::is_same< typename std::remove_const<Attribute>::type, unused_type >::value, 
                        detail::gen_unused_attribute_seq<nb_keywords>,
                        detail::gen_attribute_seq<Context,std::tuple<Keywords...>,0,0,nb_keywords> 
                        >::type{}
                        );
            }

        tst<char, std::size_t> lookup;
        std::tuple<Keywords ...> keywords;
    };

    template <typename ... Keywords>
        keywords_parser<Keywords...>keywords (Keywords ... keyword_list) 
        {
            return {std::forward<Keywords>(keyword_list)...};
        }
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename Key, typename Subject, typename Context>
        struct attribute_of<x3::keyword_parser<Key, Subject>, Context>
        : attribute_of<Subject, Context> {};

}}}}


#endif
